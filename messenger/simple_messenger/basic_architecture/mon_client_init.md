```
int MonClient::init()
{
  ldout(cct, 10) << __func__ << dendl;

  messenger->add_dispatcher_head(this);

  entity_name = cct->_conf->name;

  Mutex::Locker l(monc_lock);

  string method;
  if (!cct->_conf->auth_supported.empty())
    method = cct->_conf->auth_supported;
  else if (entity_name.get_type() == CEPH_ENTITY_TYPE_OSD ||
	   entity_name.get_type() == CEPH_ENTITY_TYPE_MDS ||
	   entity_name.get_type() == CEPH_ENTITY_TYPE_MON ||
	   entity_name.get_type() == CEPH_ENTITY_TYPE_MGR)
    method = cct->_conf->auth_cluster_required;
  else
    method = cct->_conf->auth_client_required;
  auth_supported.reset(new AuthMethodList(cct, method));
  ldout(cct, 10) << "auth_supported " << auth_supported->get_supported_set() << " method " << method << dendl;

  int r = 0;
  keyring.reset(new KeyRing); // initializing keyring anyway

  if (auth_supported->is_supported_auth(CEPH_AUTH_CEPHX)) {
    r = keyring->from_ceph_context(cct);
    if (r == -ENOENT) {
      auth_supported->remove_supported_auth(CEPH_AUTH_CEPHX);
      if (!auth_supported->get_supported_set().empty()) {
	r = 0;
	no_keyring_disabled_cephx = true;
      } else {
	lderr(cct) << "ERROR: missing keyring, cannot use cephx for authentication" << dendl;
      }
    }
  }

  if (r < 0) {
    return r;
  }

  rotating_secrets.reset(
    new RotatingKeyRing(cct, cct->get_module_type(), keyring.get()));

  initialized = true;

  timer.init();
  finisher.start();
  schedule_tick();

  return 0;
}
```
