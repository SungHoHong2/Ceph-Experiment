```
int SimpleMessenger::start()
{
  lock.Lock();
  ldout(cct,1) << "messenger.start" << dendl;

  // register at least one entity, first!
  assert(my_inst.name.type() >= 0);

  assert(!started);
  started = true;
  stopped = false;

  if (!did_bind) {
    my_inst.addr.nonce = nonce;
    init_local_connection();
  }

  lock.Unlock();

  reaper_started = true;
  reaper_thread.create("ms_reaper");
  return 0;
}

```
