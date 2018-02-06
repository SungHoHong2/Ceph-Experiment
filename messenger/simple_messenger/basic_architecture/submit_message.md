>  /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc:455

```

void SimpleMessenger::submit_message(Message *m, PipeConnection *con,
				     const entity_addr_t& dest_addr, int dest_type,
				     bool already_locked)
{
  m->trace.event("simple submitting message");
  if (cct->_conf->ms_dump_on_send) {
    m->encode(-1, true);
    ldout(cct, 0) << "submit_message " << *m << "\n";
    m->get_payload().hexdump(*_dout);
    if (m->get_data().length() > 0) {
      *_dout << " data:\n";
      m->get_data().hexdump(*_dout);
    }
    *_dout << dendl;
    m->clear_payload();
  }

  // existing connection?
  if (con) {
    Pipe *pipe = NULL;
    bool ok = static_cast<PipeConnection*>(con)->try_get_pipe(&pipe);
    if (!ok) {
      ldout(cct,0) << "submit_message " << *m << " remote, " << dest_addr
		   << ", failed lossy con, dropping message " << m << dendl;
      m->put();
      return;
    }
    while (pipe && ok) {
      // we loop in case of a racing reconnect, either from us or them
      pipe->pipe_lock.Lock(); // can't use a Locker because of the Pipe ref
      if (pipe->state != Pipe::STATE_CLOSED) {
	ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", have pipe." << dendl;
	pipe->_send(m);

```

> [pipe_send](pipe_send.md) <br>
> after this the transmission ends

```
	pipe->pipe_lock.Unlock();
	pipe->put();
	return;
      }
      Pipe *current_pipe;
      ok = con->try_get_pipe(&current_pipe);
      pipe->pipe_lock.Unlock();
      if (current_pipe == pipe) {
	ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr
		      << ", had pipe " << pipe << ", but it closed." << dendl;
	pipe->put();
	current_pipe->put();
	m->put();
	return;
      } else {
	pipe->put();
	pipe = current_pipe;
      }
    }
  }

  // local?
  if (my_inst.addr == dest_addr) {
    // local
    ldout(cct,20) << "submit_message " << *m << " local" << dendl;
    m->set_connection(local_connection.get());
    dispatch_queue.local_delivery(m, m->get_priority());
    return;
  }

  // remote, no existing pipe.
  const Policy& policy = get_policy(dest_type);
  if (policy.server) {
    ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", lossy server for target type "
		  << ceph_entity_type_name(dest_type) << ", no session, dropping." << dendl;
    m->put();
  } else {
    ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", new pipe." << dendl;
    if (!already_locked) {
      /** We couldn't handle the Message without reference to global data, so
       *  grab the lock and do it again. If we got here, we know it's a non-lossy
       *  Connection, so we can use our existing pointer without doing another lookup. */
      Mutex::Locker l(lock);
      submit_message(m, con, dest_addr, dest_type, true);
    } else {
      connect_rank(dest_addr, dest_type, static_cast<PipeConnection*>(con), m);
    }
  }
}
```
