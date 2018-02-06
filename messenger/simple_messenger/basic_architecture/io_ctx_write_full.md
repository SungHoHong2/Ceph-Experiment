```
int librados::IoCtx::write_full(const std::string& oid, bufferlist& bl)
{
  object_t obj(oid);
  return io_ctx_impl->write_full(obj, bl);
}
```

```
int librados::IoCtxImpl::write_full(const object_t& oid, bufferlist& bl)
{
  if (bl.length() > UINT_MAX/2)
    return -E2BIG;
  ::ObjectOperation op;
  prepare_assert_ops(&op);
  op.write_full(bl);
  return operate(oid, &op, NULL);
}
```

```
void write_full(bufferlist& bl) {
  add_data(CEPH_OSD_OP_WRITEFULL, 0, bl.length(), bl);
}
```

```
int librados::IoCtxImpl::operate(const object_t& oid, ::ObjectOperation *o,
				 ceph::real_time *pmtime, int flags)
{
  ceph::real_time ut = (pmtime ? *pmtime :
    ceph::real_clock::now());

  /* can't write to a snapshot */
  if (snap_seq != CEPH_NOSNAP)
    return -EROFS;

  if (!o->size())
    return 0;

  Mutex mylock("IoCtxImpl::operate::mylock");
  Cond cond;
  bool done;
  int r;
  version_t ver;

  Context *oncommit = new C_SafeCond(&mylock, &cond, &done, &r);

```
> [C-SafeCond](S_SafeCond.md)

```

  int op = o->ops[0].op.op;
  ldout(client->cct, 10) << ceph_osd_op_name(op) << " oid=" << oid
			 << " nspace=" << oloc.nspace << dendl;
  Objecter::Op *objecter_op = objecter->prepare_mutate_op(oid, oloc,
							  *o, snapc, ut, flags,
							  oncommit, &ver);
  objecter->op_submit(objecter_op);

```

> [op_submit](op_submit.md)

```


  mylock.Lock();
  while (!done)
    cond.Wait(mylock);
  mylock.Unlock();
  ldout(client->cct, 10) << "Objecter returned from "
	<< ceph_osd_op_name(op) << " r=" << r << dendl;

  set_sync_op_version(ver);

  return r;
}
```
