```
int librados::Rados::ioctx_create(const char *name, IoCtx &io)
{
  rados_ioctx_t p;
  int ret = rados_ioctx_create((rados_t)client, name, &p);
  if (ret)
    return ret;
  io.close();
  io.io_ctx_impl = (IoCtxImpl*)p;
  return 0;
}
```

```
extern "C" int rados_ioctx_create(rados_t cluster, const char *name, rados_ioctx_t *io)
{
  tracepoint(librados, rados_ioctx_create_enter, cluster, name);
  librados::RadosClient *client = (librados::RadosClient *)cluster;
  librados::IoCtxImpl *ctx;

  int r = client->create_ioctx(name, &ctx);
  if (r < 0) {
    tracepoint(librados, rados_ioctx_create_exit, r, NULL);
    return r;
  }

  *io = ctx;
  ctx->get();
  tracepoint(librados, rados_ioctx_create_exit, 0, ctx);
  return 0;
}
```

```
int librados::RadosClient::create_ioctx(const char *name, IoCtxImpl **io)
{
  int64_t poolid = lookup_pool(name);
  if (poolid < 0) {
    return (int)poolid;
  }

  *io = new librados::IoCtxImpl(this, objecter, poolid, CEPH_NOSNAP);
  return 0;
}
```


```
librados::IoCtxImpl::IoCtxImpl(RadosClient *c, Objecter *objecter,
			       int64_t poolid, snapid_t s)
  : ref_cnt(0), client(c), poolid(poolid), snap_seq(s),
    assert_ver(0), last_objver(0),
    notify_timeout(c->cct->_conf->client_notify_timeout),
    oloc(poolid), aio_write_list_lock("librados::IoCtxImpl::aio_write_list_lock"),
    aio_write_seq(0), objecter(objecter)
{
}
```
