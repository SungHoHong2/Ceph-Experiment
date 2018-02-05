```
void common_init_finish(CephContext *cct)
{
  cct->init_crypto();
  ZTracer::ztrace_init();

  int flags = cct->get_init_flags();
  if (!(flags & CINIT_FLAG_NO_DAEMON_ACTIONS))
    cct->start_service_thread();

  if ((flags & CINIT_FLAG_DEFER_DROP_PRIVILEGES) &&
      (cct->get_set_uid() || cct->get_set_gid())) {
    cct->get_admin_socket()->chown(cct->get_set_uid(), cct->get_set_gid());
  }

  md_config_t *conf = cct->_conf;

  if (!conf->admin_socket.empty() && !conf->admin_socket_mode.empty()) {
    int ret = 0;
    std::string err;

    ret = strict_strtol(conf->admin_socket_mode.c_str(), 8, &err);
    if (err.empty()) {
      if (!(ret & (~ACCESSPERMS))) {
        cct->get_admin_socket()->chmod(static_cast<mode_t>(ret));
      } else {
        lderr(cct) << "Invalid octal permissions string: "
            << conf->admin_socket_mode << dendl;
      }
    } else {
      lderr(cct) << "Invalid octal string: " << err << dendl;
    }
  }
}
```
