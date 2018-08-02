### 8 / 3
- [ ] modify to passthrough low-level
    - [x] UNDYNE: iov[1]::Hello World From CLIENT!
    - [x] able to change the swap the data with the orignal one
    - [ ] check whether this is available outside the library



```c++

static ssize_t fuse_buf_read(const struct fuse_buf *dst, size_t dst_off,
			     const struct fuse_buf *src, size_t src_off,
			     size_t len)

static ssize_t fuse_buf_copy_one(const struct fuse_buf *dst, size_t dst_off,
				 const struct fuse_buf *src, size_t src_off,
				 size_t len, enum fuse_buf_copy_flags flags)

ssize_t fuse_buf_copy(struct fuse_bufvec *dstv, struct fuse_bufvec *srcv,
		      enum fuse_buf_copy_flags flags)

static int fuse_send_data_iov_fallback(struct fuse_session *se,
				       struct fuse_chan *ch,
				       struct iovec *iov, int iov_count,
				       struct fuse_bufvec *buf,
				       size_t len)

static int fuse_send_data_iov(struct fuse_session *se, struct fuse_chan *ch,
			       struct iovec *iov, int iov_count,
			       struct fuse_bufvec *buf, unsigned int flags)

int fuse_reply_data(fuse_req_t req, struct fuse_bufvec *bufv,
		    enum fuse_buf_copy_flags flags)


```






- [ ] break down the latency of file access + network + client fuse vice versa.
- [ ]  dCat: Dynamic Cache Management for Efficient, Performance-sensitive Infrastructure-as-a-Service
