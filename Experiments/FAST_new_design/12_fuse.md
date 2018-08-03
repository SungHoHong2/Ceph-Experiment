### 8 / 3
- [ ] modify to passthrough low-level
    - [x] UNDYNE: iov[1]::Hello World From CLIENT!
    - [x] able to change the swap the data with the orignal one
    - [ ] implement your result in low level passthrough




    - [ ] swapping data of the local and the remote
        - located the actual function that determines the output of of the FUSE
        - however, the available parameters are
          - file descriptor
          - offset
        - the buffer that is used to send the data to the user is allocated and free in the library







```c++

static void lo_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		    off_t offset, struct fuse_file_info *fi)

        struct fuse_bufvec buf = FUSE_BUFVEC_INIT(size);
        buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
        buf.buf[0].fd = fi->fh;
        buf.buf[0].pos = offset;


int fuse_reply_data(fuse_req_t req, struct fuse_bufvec *bufv,
		    enum fuse_buf_copy_flags flags)
        struct iovec iov[2];
        struct fuse_out_header out;


static int fuse_send_data_iov(struct fuse_session *se, struct fuse_chan *ch,
			       struct iovec *iov, int iov_count,
			       struct fuse_bufvec *buf, unsigned int flags)


static int fuse_send_data_iov_fallback(struct fuse_session *se,
				       struct fuse_chan *ch,
				       struct iovec *iov, int iov_count,
				       struct fuse_bufvec *buf,
				       size_t len)

               struct iovec *iov, int iov_count,
  			       struct fuse_bufvec *buf, unsigned int flags)

               posix_memalign(&mbuf, pagesize, len);
               fuse_buf_copy(&mem_buf, buf, 0);
               fuse_send_msg(se, ch, iov, iov_count); <-- this sends the data;


ssize_t fuse_buf_copy(struct fuse_bufvec *dstv, struct fuse_bufvec *srcv,
		      enum fuse_buf_copy_flags flags)

static ssize_t fuse_buf_copy_one(const struct fuse_buf *dst, size_t dst_off,
				 const struct fuse_buf *src, size_t src_off,
				 size_t len, enum fuse_buf_copy_flags flags)

static ssize_t fuse_buf_read(const struct fuse_buf *dst, size_t dst_off,
			     const struct fuse_buf *src, size_t src_off,
			     size_t len)

```





- [ ] break down the latency of file access + network + client fuse vice versa.
- [ ]  dCat: Dynamic Cache Management for Efficient, Performance-sensitive Infrastructure-as-a-Service
