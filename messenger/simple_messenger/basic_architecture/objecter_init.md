```

void Objecter::init()
{
  assert(!initialized);
  m_request_state_hook = new RequestStateHook(this);
  AdminSocket* admin_socket = cct->get_admin_socket();
  int ret = admin_socket->register_command("objecter_requests",
					   "objecter_requests",
					   m_request_state_hook,
					   "show in-progress osd requests");

  /* Don't warn on EEXIST, happens if multiple ceph clients
   * are instantiated from one process */
  if (ret < 0 && ret != -EEXIST) {
    lderr(cct) << "error registering admin socket command: "
	       << cpp_strerror(ret) << dendl;
  }

  update_crush_location();
  cct->_conf->add_observer(this);
  initialized = true;
}
```

```
void md_config_t::add_observer(md_config_obs_t* observer_)
{
  Mutex::Locker l(lock);
  const char **keys = observer_->get_tracked_conf_keys();
  for (const char ** k = keys; *k; ++k) {
    obs_map_t::value_type val(*k, observer_);
    observers.insert(val);
  }
}
```
