### 8 / 3
- [ ] modify to passthrough low-level
    - [x] UNDYNE: iov[1]::Hello World From CLIENT!
    - so we need to track down `iov[1]` to see where the data can be viewed


```c++


static int fuse_send_data_iov_fallback(struct fuse_session *se,
				       struct fuse_chan *ch,
				       struct iovec *iov, int iov_count,
				       struct fuse_bufvec *buf,
				       size_t len)
static int fuse_send_msg(struct fuse_session *se, struct fuse_chan *ch, struct iovec *iov, int count);
               printf("UNDYNE: iov[1]::%s\n", (char *)iov[1].iov_base);

```






- [ ] break down the latency of file access + network + client fuse vice versa.
- [ ]  dCat: Dynamic Cache Management for Efficient, Performance-sensitive Infrastructure-as-a-Service
