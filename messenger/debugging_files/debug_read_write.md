### WRITE DEBUG

- **main.cc**
    - LINE: 67
```cpp
/*
 * create an "IoCtx" which is used to do IO to a pool
 */
{
  ret = rados.ioctx_create("test_pool", io_ctx);
  if (ret < 0) {
    std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
    ret = EXIT_FAILURE;
    goto out;
  } else {
    std::cout << "we just created an ioctx for our pool" << std::endl;
  }
}
```

<br>

- **/ceph/src/librados/librados.cc**
    - LINE: 1189
    1. string has been written in the bufferlist
    2. object was created with a title

```cpp
{
  librados::bufferlist bl;
  bl.append(hello);

   ret = io_ctx.write_full(object_name, bl);
   if (ret < 0) {
     std::cerr << "couldn't write object! error " << ret << std::endl;
     ret = EXIT_FAILURE;
     goto out;
   } else {
     std::cout << "we just wrote new object " << object_name
         << ", with contents\n" << hello << std::endl;
   }
}
```

```cpp
int librados::IoCtx::write_full(const std::string& oid, bufferlist& bl)
{
  object_t obj(oid);
  return io_ctx_impl->write_full(obj, bl);
}
```

```cpp
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

```cpp
void add_data(int op, uint64_t off, uint64_t len, bufferlist& bl) {
  OSDOp& osd_op = add_op(op);
  osd_op.op.extent.offset = off;
  osd_op.op.extent.length = len;
  osd_op.indata.claim_append(bl);
}
```

<br>

- **/ceph/src/librados/IoCtxImpl.cc**
    - LINE: 668

```cpp
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

  <br>

    - operate function LINE: 694
    - Objector is initiated in the operate function

```cpp

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

  int op = o->ops[0].op.op;
  ldout(client->cct, 10) << ceph_osd_op_name(op) << " oid=" << oid
			 << " nspace=" << oloc.nspace << dendl;
  Objecter::Op *objecter_op = objecter->prepare_mutate_op(oid, oloc,
							  *o, snapc, ut, flags,
							  oncommit, &ver);
  objecter->op_submit(objecter_op);

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

<br>

- **/ceph/src/osdc/Objector.cc**

```cpp
void Objecter::op_submit(Op *op, ceph_tid_t *ptid, int *ctx_budget)
{
  shunique_lock rl(rwlock, ceph::acquire_shared);
  ceph_tid_t tid = 0;
  if (!ptid)
    ptid = &tid;
  op->trace.event("op submit");
  _op_submit_with_budget(op, rl, ptid, ctx_budget);
}

void Objecter::_op_submit_with_budget(Op *op, shunique_lock& sul,
				      ceph_tid_t *ptid,
				      int *ctx_budget)
{
  assert(initialized);

  assert(op->ops.size() == op->out_bl.size());
  assert(op->ops.size() == op->out_rval.size());
  assert(op->ops.size() == op->out_handler.size());

  // throttle.  before we look at any state, because
  // _take_op_budget() may drop our lock while it blocks.
  if (!op->ctx_budgeted || (ctx_budget && (*ctx_budget == -1))) {
    int op_budget = _take_op_budget(op, sul);
    // take and pass out the budget for the first OP
    // in the context session
    if (ctx_budget && (*ctx_budget == -1)) {
      *ctx_budget = op_budget;
    }
  }

  if (osd_timeout > timespan(0)) {
    if (op->tid == 0)
      op->tid = ++last_tid;
    auto tid = op->tid;
    op->ontimeout = timer.add_event(osd_timeout,
				    [this, tid]() {
				      op_cancel(tid, -ETIMEDOUT); });
  }

  _op_submit(op, sul, ptid);
}
```

<br>

- **Relations of Objector - Messenger - AsyncMessenger**

```cpp

void Objecter::op_submit(Op *op, ceph_tid_t *ptid, int *ctx_budget)
{
  shunique_lock rl(rwlock, ceph::acquire_shared);
  ceph_tid_t tid = 0;
  if (!ptid)
    ptid = &tid;
  op->trace.event("op submit");
  _op_submit_with_budget(op, rl, ptid, ctx_budget);
}



void Objecter::_op_submit_with_budget(Op *op, shunique_lock& sul,
				      ceph_tid_t *ptid,
				      int *ctx_budget)
{
  assert(initialized);

  assert(op->ops.size() == op->out_bl.size());
  assert(op->ops.size() == op->out_rval.size());
  assert(op->ops.size() == op->out_handler.size());

  // throttle.  before we look at any state, because
  // _take_op_budget() may drop our lock while it blocks.
  if (!op->ctx_budgeted || (ctx_budget && (*ctx_budget == -1))) {
    int op_budget = _take_op_budget(op, sul);
    // take and pass out the budget for the first OP
    // in the context session
    if (ctx_budget && (*ctx_budget == -1)) {
      *ctx_budget = op_budget;
    }
  }

  if (osd_timeout > timespan(0)) {
    if (op->tid == 0)
      op->tid = ++last_tid;
    auto tid = op->tid;
    op->ontimeout = timer.add_event(osd_timeout,
				    [this, tid]() {
				      op_cancel(tid, -ETIMEDOUT); });
  }

  _op_submit(op, sul, ptid);
}




void Objecter::_op_submit(Op *op, shunique_lock& sul, ceph_tid_t *ptid)
{
  // rwlock is locked

  ldout(cct, 10) << __func__ << " op " << op << dendl;

  // pick target
  assert(op->session == NULL);
  OSDSession *s = NULL;

  bool check_for_latest_map = _calc_target(&op->target, nullptr)
    == RECALC_OP_TARGET_POOL_DNE;

  // Try to get a session, including a retry if we need to take write lock
  int r = _get_session(op->target.osd, &s, sul);
  if (r == -EAGAIN ||
      (check_for_latest_map && sul.owns_lock_shared())) {
    epoch_t orig_epoch = osdmap->get_epoch();
    sul.unlock();
    if (cct->_conf->objecter_debug_inject_relock_delay) {
      sleep(1);
    }
    sul.lock();
    if (orig_epoch != osdmap->get_epoch()) {
      // map changed; recalculate mapping
      ldout(cct, 10) << __func__ << " relock raced with osdmap, recalc target"
		     << dendl;
      check_for_latest_map = _calc_target(&op->target, nullptr)
	== RECALC_OP_TARGET_POOL_DNE;
      if (s) {
	put_session(s);
	s = NULL;
	r = -EAGAIN;
      }
    }
  }
  if (r == -EAGAIN) {
    assert(s == NULL);
    r = _get_session(op->target.osd, &s, sul);
  }
  assert(r == 0);
  assert(s);  // may be homeless

  _send_op_account(op);

  // send?

  assert(op->target.flags & (CEPH_OSD_FLAG_READ|CEPH_OSD_FLAG_WRITE));

  if (osdmap_full_try) {
    op->target.flags |= CEPH_OSD_FLAG_FULL_TRY;
  }

  bool need_send = false;

  if (osdmap->get_epoch() < epoch_barrier) {
    ldout(cct, 10) << " barrier, paused " << op << " tid " << op->tid
		   << dendl;
    op->target.paused = true;
    _maybe_request_map();
  } else if ((op->target.flags & CEPH_OSD_FLAG_WRITE) &&
             osdmap->test_flag(CEPH_OSDMAP_PAUSEWR)) {
    ldout(cct, 10) << " paused modify " << op << " tid " << op->tid
		   << dendl;
    op->target.paused = true;
    _maybe_request_map();
  } else if ((op->target.flags & CEPH_OSD_FLAG_READ) &&
	     osdmap->test_flag(CEPH_OSDMAP_PAUSERD)) {
    ldout(cct, 10) << " paused read " << op << " tid " << op->tid
		   << dendl;
    op->target.paused = true;
    _maybe_request_map();
  } else if (op->respects_full() &&
	     (_osdmap_full_flag() ||
	      _osdmap_pool_full(op->target.base_oloc.pool))) {
    ldout(cct, 0) << " FULL, paused modify " << op << " tid "
		  << op->tid << dendl;
    op->target.paused = true;
    _maybe_request_map();
  } else if (!s->is_homeless()) {
    need_send = true;
  } else {
    _maybe_request_map();
  }

  MOSDOp *m = NULL;
  if (need_send) {
    m = _prepare_osd_op(op);
  }

  OSDSession::unique_lock sl(s->lock);
  if (op->tid == 0)
    op->tid = ++last_tid;

  ldout(cct, 10) << "_op_submit oid " << op->target.base_oid
		 << " '" << op->target.base_oloc << "' '"
		 << op->target.target_oloc << "' " << op->ops << " tid "
		 << op->tid << " osd." << (!s->is_homeless() ? s->osd : -1)
		 << dendl;

  _session_op_assign(s, op);

  if (need_send) {
    _send_op(op, m);
  }

  // Last chance to touch Op here, after giving up session lock it can
  // be freed at any time by response handler.
  ceph_tid_t tid = op->tid;
  if (check_for_latest_map) {
    _send_op_map_check(op);
  }
  if (ptid)
    *ptid = tid;
  op = NULL;

  sl.unlock();
  put_session(s);

  ldout(cct, 5) << num_in_flight << " in flight" << dendl;
}

```




<br>

- **/ceph/src/librados/.cc**
    - LINE: 0

```cpp
```
