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
void OSD::ms_handle_fast_connect(Connection *con)
{
  if (con->get_peer_type() != CEPH_ENTITY_TYPE_MON &&
      con->get_peer_type() != CEPH_ENTITY_TYPE_MGR) {
    Session *s = static_cast<Session*>(con->get_priv());
    if (!s) {
      s = new Session(cct);
      con->set_priv(s->get());
      s->con = con;
      dout(10) << " new session (outgoing) " << s << " con=" << s->con
          << " addr=" << s->con->get_peer_addr() << dendl;
      // we don't connect to clients
      assert(con->get_peer_type() == CEPH_ENTITY_TYPE_OSD);
      s->entity_name.set_type(CEPH_ENTITY_TYPE_OSD);
    }
    s->put();
  }
}
```
