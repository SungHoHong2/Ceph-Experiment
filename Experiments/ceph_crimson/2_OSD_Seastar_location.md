### Implementing Seastar in OSD
- the peer address part will determine whether it will use the normal POSIX interface or the Seastar interface
- the `TCP_READ` and the `TCP_WRITE` are the final ones that are receiving and sending the data.
- the messenger simply adds the data in a queue or read from the queue and the TCP_READ and TCP_WRITE actually gets the data from external nodes


```c++
int SimpleMessenger::_send_message(Message *m, Connection *con);
void SimpleMessenger::submit_message(Message *m, PipeConnection *con,  // add additional interface here
				     const entity_addr_t& dest_addr, int dest_type,
				     bool already_locked);

    m->encode(-1, true);
    m->get_payload().hexdump(*_dout);

    if (m->get_data().length() > 0) {
      *_dout << " data:\n";
      m->get_data().hexdump(*_dout);
    }

    m->clear_payload();

    if (con)
    Pipe *pipe = NULL;
    bool ok = static_cast<PipeConnection*>(con)->try_get_pipe(&pipe);
    pipe->_send(m);
          void _send(Message *m) {
                assert(pipe_lock.is_locked());
                out_q[m->get_priority()].push_back(m);
                cond.Signal();
          }


    pipe->pipe_lock.Unlock();
    pipe->put();


    if (my_addr == dest_addr);
        m->set_connection(local_connection.get());
        dispatch_queue.local_delivery(m, m->get_priority());

```
