```
void DispatchQueue::start()
{
  assert(!stop);
  assert(!dispatch_thread.is_started());
  dispatch_thread.create("ms_dispatch");
  local_delivery_thread.create("ms_local");
}
```
