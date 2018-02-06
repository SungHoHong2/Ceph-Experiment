> /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc:55
```
SimpleMessenger::SimpleMessenger(CephContext *cct, entity_name_t name,
				 string mname, uint64_t _nonce)
  : SimplePolicyMessenger(cct, name,mname, _nonce),
    accepter(this, _nonce),
    dispatch_queue(cct, this, mname),
    reaper_thread(this),
    nonce(_nonce),
    lock("SimpleMessenger::lock"), need_addr(true), did_bind(false),
    global_seq(0),
    cluster_protocol(0),
    reaper_started(false), reaper_stop(false),
    timeout(0),
    local_connection(new PipeConnection(cct, this)) // extends to Pipeline ConnectionRef which is a pointer of Connection
{
  ANNOTATE_BENIGN_RACE_SIZED(&timeout, sizeof(timeout),
                             "SimpleMessenger read timeout");
  init_local_connection();
}
```

> [Connection](Connection.md)
> [Pipeline connection](Pipe.md)


```
void SimpleMessenger::init_local_connection()
{
  local_connection->peer_addr = my_inst.addr;
  local_connection->peer_type = my_inst.name.type();
  local_connection->set_features(CEPH_FEATURES_ALL);
  ms_deliver_handle_fast_connect(local_connection.get());
}
```


```
/**
 * Notify each fast Dispatcher of a new Connection. Call
 * this function whenever a new Connection is initiated or
 * reconnects.
 *
 * @param con Pointer to the new Connection.
 */
void ms_deliver_handle_fast_connect(Connection *con) {
  for (list<Dispatcher*>::iterator p = fast_dispatchers.begin();
       p != fast_dispatchers.end();
       ++p)
    (*p)->ms_handle_fast_connect(con);
}
```

```
/**
 * This function will be called synchronously whenever a Connection is
 * newly-created or reconnects in the Messenger, if you support fast
 * dispatch. It is guaranteed to be called before any messages are
 * dispatched.
 *
 * @param con The new Connection which has been established. You are not
 * granted a reference to it -- take one if you need one!
 */
virtual void ms_handle_fast_connect(Connection *con) {}
```
> I believe this is one good example of ms_handle_fast_connect
```
void ms_handle_fast_connect(Connection *con) override {
  lock.Lock();
  lderr(g_ceph_context) << __func__ << " " << con << dendl;
  Session *s = static_cast<Session*>(con->get_priv());
  if (!s) {
    s = new Session(con);
    con->set_priv(s->get());
    lderr(g_ceph_context) << __func__ << " con: " << con << " count: " << s->count << dendl;
  }
  s->put();
  got_connect = true;
  cond.Signal();
  lock.Unlock();
}
```
