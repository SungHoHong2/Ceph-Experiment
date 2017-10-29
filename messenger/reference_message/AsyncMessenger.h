void init_local_connection() {
  Mutex::Locker l(lock);
  _init_local_connection();
}

void _init_local_connection() {
  assert(lock.is_locked());
  local_connection->peer_addr = my_inst.addr;
  local_connection->peer_type = my_inst.name.type();
  local_connection->set_features(CEPH_FEATURES_ALL);
  ms_deliver_handle_fast_connect(local_connection.get());
}
