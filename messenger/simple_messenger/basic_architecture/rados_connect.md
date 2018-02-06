> break 64
```
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

```
int librados::Rados::connect()
{
  return client->connect();
}
```

> home/sungho/ceph/src/librados/RadosClient.cc

```
int librados::RadosClient::connect()
{
  common_init_finish(cct);
```
> [common_init_finish](common_init_finish.md)

```
  int err;

  // already connected?
  if (state == CONNECTING)
    return -EINPROGRESS;
  if (state == CONNECTED)
    return -EISCONN;
  state = CONNECTING; // ended up in the connecting part

  // get monmap
  err = monclient.build_initial_monmap();
  if (err < 0)
    goto out;

  err = -ENOMEM;
  messenger = Messenger::create_client_messenger(cct, "radosclient");
  if (!messenger)
    goto out;

```
>  /home/sungho/ceph/src/msg/Messenger.cc:28 <br>
> [create_client_messenger](create_client_messenger.md)
```

  // require OSDREPLYMUX feature.  this means we will fail to talk to
  // old servers.  this is necessary because otherwise we won't know
  // how to decompose the reply data into its constituent pieces.
  messenger->set_default_policy(Messenger::Policy::lossy_client(CEPH_FEATURE_OSDREPLYMUX));

  ldout(cct, 1) << "starting msgr at " << messenger->get_myaddr() << dendl;

  ldout(cct, 1) << "starting objecter" << dendl;

  objecter = new (std::nothrow) Objecter(cct, messenger, &monclient,
```

> [objecter](objecter.md)

```
			  &finisher,
			  cct->_conf->rados_mon_op_timeout,
			  cct->_conf->rados_osd_op_timeout);
  if (!objecter)
    goto out;
  objecter->set_balanced_budget();

  monclient.set_messenger(messenger);
  mgrclient.set_messenger(messenger);

  objecter->init();

```

[objecter_init](objecter_init.md)

```
  messenger->add_dispatcher_head(&mgrclient);
  messenger->add_dispatcher_tail(objecter);
  messenger->add_dispatcher_tail(this);

  messenger->start();

```
> [add_dispatcher_tail](add_dispatcher_tail.md) <br>
> [messenger_start](messenger_start.md)
```
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
```
> [mon_client_init](mon_client_init.md)

```
  err = monclient.authenticate(conf->client_mount_timeout);
  if (err) {
    ldout(cct, 0) << conf->name << " authentication error " << cpp_strerror(-err) << dendl;
    shutdown();
    goto out;
  }
  messenger->set_myname(entity_name_t::CLIENT(monclient.get_global_id()));

  // MgrClient needs this (it doesn't have MonClient reference itself)
  monclient.sub_want("mgrmap", 0, 0);
  monclient.renew_subs();

  if (service_daemon) {
    ldout(cct, 10) << __func__ << " registering as " << service_name << "."
		   << daemon_name << dendl;
    mgrclient.service_daemon_register(service_name, daemon_name,
				      daemon_metadata);
  }
  mgrclient.init();

  objecter->set_client_incarnation(0);
  objecter->start();
```

> [objecter_start](objecter_start.md)

```

  lock.Lock();
  timer.init();
  finisher.start();
```

> [finisher_start](finisher_start.md)

```

  state = CONNECTED;
  instance_id = monclient.get_global_id();

  lock.Unlock();

  ldout(cct, 1) << "init done" << dendl;
  err = 0;

 out:
  if (err) {
    state = DISCONNECTED;

    if (objecter) {
      delete objecter;
      objecter = NULL;
    }
    if (messenger) {
      delete messenger;
      messenger = NULL;
    }
  }

  return err;
}
```
