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

#include "include/stringify.h"
#include "include/types.h"
#include "include/compat.h"
#include "common/align.h"
#include "common/errno.h"
#include "common/debug.h"
#include "common/perf_counters.h"
#include "common/io_priority.h"


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



static void data_buf_reset_sgl(void *cb_arg, uint32_t sgl_offset)
{

}

static int data_buf_next_sge(void *cb_arg, void **address, uint32_t *length)
{
  return 0;
}


class NVMEManager {
 public:
  struct ProbeContext {
  };

 private:
  Mutex lock;
  bool init = false;

 public:
  NVMEManager()
      : lock("NVMEDevice::NVMEManager::lock") {}

};


NVMEDevice::NVMEDevice(CephContext* cct, aio_callback_t cb, void *cbpriv)
  :   BlockDevice(cct, cb, cbpriv),
      driver(nullptr)
{
}

int NVMEDevice::open(const string& p)
{
  return 0;
}


int NVMEDevice::collect_metadata(const string& prefix, map<string,string> *pm) const
{

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
