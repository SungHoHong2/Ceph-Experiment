```
Messenger *Messenger::create_client_messenger(CephContext *cct, string lname)
{
  std::string public_msgr_type = cct->_conf->ms_public_type.empty() ? cct->_conf->get_val<std::string>("ms_type") : cct->_conf->ms_public_type;
  auto nonce = ceph::util::generate_random_number<uint64_t>();
  return Messenger::create(cct, public_msgr_type, entity_name_t::CLIENT(),
			   std::move(lname), nonce, 0);
}
```

> /home/sungho/ceph/src/msg/Messenger.cc:29

```
Messenger *Messenger::create(CephContext *cct, const string &type,
			     entity_name_t name, string lname,
			     uint64_t nonce, uint64_t cflags)
{
  int r = -1;
  if (type == "random") {
    r = ceph::util::generate_random_number(0, 1);
  }
  if (r == 0 || type == "simple")
    return new SimpleMessenger(cct, name, std::move(lname), nonce);
```

> [SimpleMessenger](SimpleMessenger.md)

```
  else if (r == 1 || type.find("async") != std::string::npos)
    return new AsyncMessenger(cct, name, type, std::move(lname), nonce);
#ifdef HAVE_XIO
  else if ((type == "xio") &&
	   cct->check_experimental_feature_enabled("ms-type-xio"))
    return new XioMessenger(cct, name, std::move(lname), nonce, cflags);
#endif
  lderr(cct) << "unrecognized ms_type '" << type << "'" << dendl;
  return nullptr;
}
```
