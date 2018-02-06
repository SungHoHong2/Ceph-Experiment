```
/**
 * Add a new Dispatcher to the end of the list. If you add
 * a Dispatcher which is already included, it will get a duplicate
 * entry. This will reduce efficiency but not break anything.
 *
 * @param d The Dispatcher to insert into the list.
 */
void add_dispatcher_tail(Dispatcher *d) {
  bool first = dispatchers.empty();
  dispatchers.push_back(d);
  if (d->ms_can_fast_dispatch_any())
    fast_dispatchers.push_back(d);
  if (first)
    ready();
}
```
