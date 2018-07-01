// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
//
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
  *
 * Copyright (C) 2015 XSky <haomai@xsky.com>
 *
 * Author: Haomai Wang <haomaiwang@gmail.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <functional>
#include <map>
#include <thread>

#include <spdk/nvme.h>

#include "include/stringify.h"
#include "include/types.h"
#include "include/compat.h"
#include "common/align.h"
#include "common/errno.h"
#include "common/debug.h"
#include "common/perf_counters.h"
#include "common/io_priority.h"

#include "NVMEDevice.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_bdev
#undef dout_prefix
#define dout_prefix *_dout << "bdev(" << sn << ") "

thread_local SharedDriverQueueData *queue_t;

static constexpr uint16_t data_buffer_default_num = 1024;

static constexpr uint32_t data_buffer_size = 8192;

static constexpr uint16_t inline_segment_num = 32;

enum {
  l_bluestore_nvmedevice_first = 632430,
  l_bluestore_nvmedevice_write_lat,
  l_bluestore_nvmedevice_read_lat,
  l_bluestore_nvmedevice_flush_lat,
  l_bluestore_nvmedevice_write_queue_lat,
  l_bluestore_nvmedevice_read_queue_lat,
  l_bluestore_nvmedevice_flush_queue_lat,
  l_bluestore_nvmedevice_queue_ops,
  l_bluestore_nvmedevice_polling_lat,
  l_bluestore_nvmedevice_buffer_alloc_failed,
  l_bluestore_nvmedevice_last
};


struct IORequest {
  uint16_t cur_seg_idx = 0;
  uint16_t nseg;
  uint32_t cur_seg_left = 0;
  void *inline_segs[inline_segment_num];
  void **extra_segs = nullptr;
};

struct Task;


struct Task {
  NVMEDevice *device;
  IOContext *ctx = nullptr;
  IOCommand command;
  uint64_t offset;
  uint64_t len;
  bufferlist bl;
  std::function<void()> fill_cb;
  Task *next = nullptr;
  int64_t return_code;
  ceph::coarse_real_clock::time_point start;
  IORequest io_request;
  std::mutex lock;
  std::condition_variable cond;
  SharedDriverQueueData *queue = nullptr;
  Task(NVMEDevice *dev, IOCommand c, uint64_t off, uint64_t l, int64_t rc = 0)
    : device(dev), command(c), offset(off), len(l),
      return_code(rc),
      start(ceph::coarse_real_clock::now()) {}
  ~Task() {
    assert(!io_request.nseg);
  }
  void release_segs(SharedDriverQueueData *queue_data) {
    if (io_request.extra_segs) {
      for (uint16_t i = 0; i < io_request.nseg; i++)
        queue_data->data_buf_mempool.push_back(io_request.extra_segs[i]);
      delete io_request.extra_segs;
    } else if (io_request.nseg) {
      for (uint16_t i = 0; i < io_request.nseg; i++)
        queue_data->data_buf_mempool.push_back(io_request.inline_segs[i]);
    }
    ctx->total_nseg -= io_request.nseg;
    io_request.nseg = 0;
  }

  void copy_to_buf(char *buf, uint64_t off, uint64_t len) {
    uint64_t copied = 0;
    uint64_t left = len;
    void **segs = io_request.extra_segs ? io_request.extra_segs : io_request.inline_segs;
    uint16_t i = 0;
    while (left > 0) {
      char *src = static_cast<char*>(segs[i++]);
      uint64_t need_copy = std::min(left, data_buffer_size-off);
      memcpy(buf+copied, src+off, need_copy);
      off = 0;
      left -= need_copy;
      copied += need_copy;
    }
  }
};

static void data_buf_reset_sgl(void *cb_arg, uint32_t sgl_offset)
{
  Task *t = static_cast<Task*>(cb_arg);
  uint32_t i = sgl_offset / data_buffer_size;
  uint32_t offset = i * data_buffer_size;
  assert(i <= t->io_request.nseg);

  for (; i < t->io_request.nseg; i++) {
    offset += data_buffer_size;
    if (offset > sgl_offset) {
      if (offset > t->len)
        offset = t->len;
      break;
    }
  }

  t->io_request.cur_seg_idx = i;
  t->io_request.cur_seg_left = offset - sgl_offset;
  return ;
}

static int data_buf_next_sge(void *cb_arg, void **address, uint32_t *length)
{
  uint32_t size;
  void *addr;
  Task *t = static_cast<Task*>(cb_arg);
  if (t->io_request.cur_seg_idx >= t->io_request.nseg) {
    *length = 0;
    *address = 0;
    return 0;
  }

  addr = t->io_request.extra_segs ? t->io_request.extra_segs[t->io_request.cur_seg_idx] : t->io_request.inline_segs[t->io_request.cur_seg_idx];

  size = data_buffer_size;
  if (t->io_request.cur_seg_idx == t->io_request.nseg - 1) {
      uint64_t tail = t->len % data_buffer_size;
      if (tail) {
        size = (uint32_t) tail;
      }
  }
 
  if (t->io_request.cur_seg_left) {
    *address = (void *)((uint64_t)addr + size - t->io_request.cur_seg_left);
    *length = t->io_request.cur_seg_left;
    t->io_request.cur_seg_left = 0;
  } else {
    *address = addr;
    *length = size;
  }
  
  t->io_request.cur_seg_idx++;
  return 0;
}

int SharedDriverQueueData::alloc_buf_from_pool(Task *t, bool write)
{
  uint64_t count = t->len / data_buffer_size;
  if (t->len % data_buffer_size)
    ++count;
  void **segs;
  if (count > data_buf_mempool.size())
    return -ENOMEM;
  if (count <= inline_segment_num) {
    segs = t->io_request.inline_segs;
  } else {
    t->io_request.extra_segs = new void*[count];
    segs = t->io_request.extra_segs;
  }
  for (uint16_t i = 0; i < count; i++) {
    segs[i] = data_buf_mempool.back();
    data_buf_mempool.pop_back();
  }
  t->io_request.nseg = count;
  t->ctx->total_nseg += count;
  if (write) {
    auto blp = t->bl.begin();
    uint32_t len = 0;
    uint16_t i = 0;
    for (; i < count - 1; ++i) {
      blp.copy(data_buffer_size, static_cast<char*>(segs[i]));
      len += data_buffer_size;
    }
    blp.copy(t->bl.length() - len, static_cast<char*>(segs[i]));
  }

  return 0;
}


#define dout_subsys ceph_subsys_bdev
#undef dout_prefix
#define dout_prefix *_dout << "bdev "

class NVMEManager {
 public:
  struct ProbeContext {
    string sn_tag;
    NVMEManager *manager;
    SharedDriverData *driver;
    bool done;
  };

 private:
  Mutex lock;
  bool init = false;
  std::vector<SharedDriverData*> shared_driver_datas;
  std::thread dpdk_thread;
  std::mutex probe_queue_lock;
  std::condition_variable probe_queue_cond;
  std::list<ProbeContext*> probe_queue;

 public:
  NVMEManager()
      : lock("NVMEDevice::NVMEManager::lock") {}
  int try_get(const string &sn_tag, SharedDriverData **driver);

};

static NVMEManager manager;






// ----------------
#undef dout_prefix
#define dout_prefix *_dout << "bdev(" << name << ") "

NVMEDevice::NVMEDevice(CephContext* cct, aio_callback_t cb, void *cbpriv)
  :   BlockDevice(cct, cb, cbpriv),
      driver(nullptr)
{
}

int NVMEDevice::open(const string& p)
{
  int r = 0;
  dout(1) << __func__ << " path " << p << dendl;

  string serial_number;
  int fd = ::open(p.c_str(), O_RDONLY);
  if (fd < 0) {
    r = -errno;
    derr << __func__ << " unable to open " << p << ": " << cpp_strerror(r)
	 << dendl;
    return r;
  }
  char buf[100];
  r = ::read(fd, buf, sizeof(buf));
  VOID_TEMP_FAILURE_RETRY(::close(fd));
  fd = -1; // defensive
  if (r <= 0) {
    if (r == 0) {
      r = -EINVAL;
    } else {
      r = -errno;
    }
    derr << __func__ << " unable to read " << p << ": " << cpp_strerror(r) << dendl;
    return r;
  }
  /* scan buf from the beginning with isxdigit. */
  int i = 0;
  while (i < r && isxdigit(buf[i])) {
    i++;
  }
  serial_number = string(buf, i);
  r = manager.try_get(serial_number, &driver);
  if (r < 0) {
    derr << __func__ << " failed to get nvme device with sn " << serial_number << dendl;
    return r;
  }

  driver->register_device(this);
  block_size = driver->get_block_size();
  size = driver->get_size();
  name = serial_number;

  //nvme is non-rotational device.
  rotational = false;

  // round size down to an even block
  size &= ~(block_size - 1);

  dout(1) << __func__ << " size " << size << " (" << byte_u_t(size) << ")"
          << " block_size " << block_size << " (" << byte_u_t(block_size)
          << ")" << dendl;


  return 0;
}

void NVMEDevice::close()
{
  dout(1) << __func__ << dendl;

  delete queue_t;
  queue_t = nullptr;
  name.clear();
  driver->remove_device(this);

  dout(1) << __func__ << " end" << dendl;
}

int NVMEDevice::collect_metadata(const string& prefix, map<string,string> *pm) const
{
  (*pm)[prefix + "rotational"] = "0";
  (*pm)[prefix + "size"] = stringify(get_size());
  (*pm)[prefix + "block_size"] = stringify(get_block_size());
  (*pm)[prefix + "driver"] = "NVMEDevice";
  (*pm)[prefix + "type"] = "nvme";
  (*pm)[prefix + "access_mode"] = "spdk";
  (*pm)[prefix + "nvme_serial_number"] = name;

  return 0;
}

int NVMEDevice::flush()
{
  return 0;
}

void NVMEDevice::aio_submit(IOContext *ioc)
{

}

static void write_split(
    NVMEDevice *dev,
    uint64_t off,
    bufferlist &bl,
    IOContext *ioc)
{

}

int NVMEDevice::aio_write(
    uint64_t off,
    bufferlist &bl,
    IOContext *ioc,
    bool buffered)
{


  return 0;
}

int NVMEDevice::write(uint64_t off, bufferlist &bl, bool buffered)
{
  return 0;
}

int NVMEDevice::read(uint64_t off, uint64_t len, bufferlist *pbl,
                     IOContext *ioc,
                     bool buffered)
{
  int r = 0;
  return r;
}

int NVMEDevice::aio_read(
    uint64_t off,
    uint64_t len,
    bufferlist *pbl,
    IOContext *ioc)
{
  return 0;
}

int NVMEDevice::read_random(uint64_t off, uint64_t len, char *buf, bool buffered)
{
  int r = 0;
  return r;
}

int NVMEDevice::invalidate_cache(uint64_t off, uint64_t len)
{
  return 0;
}
