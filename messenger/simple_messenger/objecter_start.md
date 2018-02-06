```
/*
 * ok, cluster interaction can happen
 */
void Objecter::start(const OSDMap* o)
{
  shared_lock rl(rwlock);

  start_tick();
  if (o) {
    osdmap->deepish_copy_from(*o);
  } else if (osdmap->get_epoch() == 0) {
    _maybe_request_map();
  }
}
```
