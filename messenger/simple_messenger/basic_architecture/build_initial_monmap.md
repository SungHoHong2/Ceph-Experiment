> three threads are running the same function
```
int MonClient::build_initial_monmap()
{
  ldout(cct, 10) << __func__ << dendl;
  return monmap.build_initial(cct, cerr);
}
```
> goto build_initial  home/sungho/ceph/src/mon/MonMap.cc:441
```
int MonMap::build_initial(CephContext *cct, ostream& errout)
{
  const md_config_t *conf = cct->_conf;
  // file?
  const auto monmap = conf->get_val<std::string>("monmap");
  if (!monmap.empty()) {
    int r;
    try {
      r = read(monmap.c_str());
    }
    catch (const buffer::error &e) {
      r = -EINVAL;
    }
    if (r >= 0)
      return 0;
    errout << "unable to read/decode monmap from " << monmap
	 << ": " << cpp_strerror(-r) << std::endl;
    return r;
  }

  // fsid from conf?
  const auto new_fsid = conf->get_val<uuid_d>("fsid");
  if (!new_fsid.is_zero()) {
    fsid = new_fsid;
  }

```
> getting the uuid

```

  // -m foo?
  const auto mon_host = conf->get_val<std::string>("mon_host");
  if (!mon_host.empty()) {
    int r = build_from_host_list(mon_host, "noname-");
    if (r < 0) {
      errout << "unable to parse addrs in '" << mon_host << "'"
             << std::endl;
      return r;
    }
    created = ceph_clock_now();
    last_changed = created;
    return 0;
  }
```
