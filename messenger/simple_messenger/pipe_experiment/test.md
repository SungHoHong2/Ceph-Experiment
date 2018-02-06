```

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
    _send_op(op);
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

```
int Objecter::_get_session(int osd, OSDSession **session, shunique_lock& sul)
{
  assert(sul && sul.mutex() == &rwlock);

  if (osd < 0) {
    *session = homeless_session;
    ldout(cct, 20) << __func__ << " osd=" << osd << " returning homeless"
		   << dendl;
    return 0;
  }

  map<int,OSDSession*>::iterator p = osd_sessions.find(osd);
  if (p != osd_sessions.end()) {
    OSDSession *s = p->second;
    s->get();
    *session = s;
    ldout(cct, 20) << __func__ << " s=" << s << " osd=" << osd << " "
		   << s->get_nref() << dendl;
    return 0;
  }
  if (!sul.owns_lock()) {
    return -EAGAIN;
  }
  OSDSession *s = new OSDSession(cct, osd);
  osd_sessions[osd] = s;
  s->con = messenger->get_connection(osdmap->get_inst(osd));
  s->con->set_priv(s->get());
  logger->inc(l_osdc_osd_session_open);
  logger->set(l_osdc_osd_sessions, osd_sessions.size());
  s->get();
  *session = s;
  ldout(cct, 20) << __func__ << " s=" << s << " osd=" << osd << " "
		 << s->get_nref() << dendl;
  return 0;
}
```


```
ConnectionRef SimpleMessenger::get_connection(const entity_inst_t& dest)
{
  Mutex::Locker l(lock);
  if (my_inst.addr == dest.addr) {
    // local
    return local_connection;
  }

  // remote
  while (true) {
    Pipe *pipe = _lookup_pipe(dest.addr);
    if (pipe) {
      ldout(cct, 10) << "get_connection " << dest << " existing " << pipe << dendl;
    } else {
      pipe = connect_rank(dest.addr, dest.name.type(), NULL, NULL);
      ldout(cct, 10) << "get_connection " << dest << " new " << pipe << dendl;
    }
    Mutex::Locker l(pipe->pipe_lock);
    if (pipe->connection_state)
      return pipe->connection_state;
    // we failed too quickly!  retry.  FIXME.
  }
}
```



```
/* connect_rank
 * NOTE: assumes messenger.lock held.
 */
Pipe *SimpleMessenger::connect_rank(const entity_addr_t& addr,
				    int type,
				    PipeConnection *con,
				    Message *first)
{
  assert(lock.is_locked());
  assert(addr != my_inst.addr);

  ldout(cct,10) << "connect_rank to " << addr << ", creating pipe and registering" << dendl;

  // create pipe
  Pipe *pipe = new Pipe(this, Pipe::STATE_CONNECTING,
			static_cast<PipeConnection*>(con));
  pipe->pipe_lock.Lock();
  pipe->set_peer_type(type);
  pipe->set_peer_addr(addr);
  pipe->policy = get_policy(type);
  pipe->start_writer();
  if (first)
    pipe->_send(first);
  pipe->pipe_lock.Unlock();
  pipe->register_pipe();
  pipes.insert(pipe);

  return pipe;
}
```


```
void Pipe::start_writer()
int Pipe::connect()
```

```
rc = ::connect(sd, peer_addr.get_sockaddr(), peer_addr.get_sockaddr_len());
rc = tcp_read((char*)&banner, strlen(CEPH_BANNER));
rc = do_sendmsg(&msg, msglen);
```
