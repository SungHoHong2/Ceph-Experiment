```
void Objecter::_send_op(Op *op)
{
  // rwlock is locked
  // op->session->lock is locked

  // backoff?
  auto p = op->session->backoffs.find(op->target.actual_pgid);
  if (p != op->session->backoffs.end()) {
    hobject_t hoid = op->target.get_hobj();
    auto q = p->second.lower_bound(hoid);
    if (q != p->second.begin()) {
      --q;
      if (hoid >= q->second.end) {
	++q;
      }
    }
    if (q != p->second.end()) {
      ldout(cct, 20) << __func__ << " ? " << q->first << " [" << q->second.begin
		     << "," << q->second.end << ")" << dendl;
      int r = cmp(hoid, q->second.begin);
      if (r == 0 || (r > 0 && hoid < q->second.end)) {
	ldout(cct, 10) << __func__ << " backoff " << op->target.actual_pgid
		       << " id " << q->second.id << " on " << hoid
		       << ", queuing " << op << " tid " << op->tid << dendl;
	return;
      }
    }
  }

  assert(op->tid > 0);
  MOSDOp *m = _prepare_osd_op(op);

  if (op->target.actual_pgid != m->get_spg()) {
    ldout(cct, 10) << __func__ << " " << op->tid << " pgid change from "
		   << m->get_spg() << " to " << op->target.actual_pgid
		   << ", updating and reencoding" << dendl;
    m->set_spg(op->target.actual_pgid);
    m->clear_payload();  // reencode
  }

  ldout(cct, 15) << "_send_op " << op->tid << " to "
		 << op->target.actual_pgid << " on osd." << op->session->osd
		 << dendl;

  ConnectionRef con = op->session->con;
  assert(con);

  // preallocated rx buffer?
  if (op->con) {
    ldout(cct, 20) << " revoking rx buffer for " << op->tid << " on "
		   << op->con << dendl;
    op->con->revoke_rx_buffer(op->tid);
  }
  if (op->outbl &&
      op->ontimeout == 0 &&  // only post rx_buffer if no timeout; see #9582
      op->outbl->length()) {
    ldout(cct, 20) << " posting rx buffer for " << op->tid << " on " << con
		   << dendl;
    op->con = con;
    op->con->post_rx_buffer(op->tid, *op->outbl);
  }

  op->incarnation = op->session->incarnation;

  if (op->trace.valid()) {
    m->trace.init("op msg", nullptr, &op->trace);
  }
  op->session->con->send_message(m);
}
```

```
int PipeConnection::send_message(Message *m)
{
  assert(msgr);
  return static_cast<SimpleMessenger*>(msgr)->send_message(m, this);
}
```


```

int SimpleMessenger::_send_message(Message *m, const entity_inst_t& dest)
{
  // set envelope
  m->get_header().src = get_myname();
  m->set_cct(cct);

  if (!m->get_priority()) m->set_priority(get_default_send_priority());

  ldout(cct,1) <<"--> " << dest.name << " "
          << dest.addr << " -- " << *m
    	  << " -- ?+" << m->get_data().length()
	  << " " << m
	  << dendl;

  if (dest.addr == entity_addr_t()) {
    ldout(cct,0) << "send_message message " << *m
                 << " with empty dest " << dest.addr << dendl;
    m->put();
    return -EINVAL;
  }

  lock.Lock();
  Pipe *pipe = _lookup_pipe(dest.addr);
  submit_message(m, (pipe ? pipe->connection_state.get() : NULL),
                 dest.addr, dest.name.type(), true);
  lock.Unlock();
  return 0;
}
```


[submit_message](submit_message.md)
