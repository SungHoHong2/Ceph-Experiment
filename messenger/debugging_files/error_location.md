
### Running hello_world_cpp

- **main.c**
    - run the file with argument -c ceph.conf
    - LINE 77
```cpp
{
  ret = rados.connect();
  if (ret < 0) {
    std::cerr << "couldn't connect to cluster! error " << ret << std::endl;
    ret = EXIT_FAILURE;
    goto out;
  } else {
    std::cout << "we just connected to the rados cluster" << std::endl;
  }
}
```

<br>

- **RadosClient.cc**
    - LINE 286
```cpp
messenger->start();

ldout(cct, 1) << "setting wanted keys" << dendl;
monclient.set_want_keys(
    CEPH_ENTITY_TYPE_MON | CEPH_ENTITY_TYPE_OSD | CEPH_ENTITY_TYPE_MGR);
ldout(cct, 1) << "calling monclient init" << dendl;
err = monclient.init();
if (err) {
  ldout(cct, 0) << conf->name << " initialization error " << cpp_strerror(-err) << dendl;
  shutdown();
  goto out;
}
err = monclient.authenticate(conf->client_mount_timeout);
if (err) {
  ldout(cct, 0) << conf->name << " authentication error " << cpp_strerror(-err) << dendl;
  shutdown();
  goto out;
}
```


<br>


- **MonClient.cc**
    - LINE 376
    - LINE 380 keyring checking causes the error
```cpp
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
```


<br>


- **Keyring.cc**
    - LINE 35
```cpp
  const md_config_t *conf = cct->_conf;
  string filename;

  int ret = ceph_resolve_file_search(conf->keyring, filename);
  if (!ret) {
    ret = load(cct, filename);
    if (ret < 0)
      lderr(cct) << "failed to load " << filename
		 << ": " << cpp_strerror(ret) << dendl;
  } else {
    lderr(cct) << "unable to find a keyring on " << conf->keyring
	       << ": " << cpp_strerror(ret) << dendl;
  }
```

<br>

- **Config.cc**
```cpp

  list<string> ls;
  get_str_list(filename_list, ls);

  int ret = -ENOENT;
  list<string>::iterator iter;
  for (iter = ls.begin(); iter != ls.end(); ++iter) {
    int fd = ::open(iter->c_str(), O_RDONLY);
    if (fd < 0) {
      ret = -errno;
      continue;
    }
    close(fd);
    result = *iter;
    return 0;
  }

```
