```
void _send(Message *m) {
  assert(pipe_lock.is_locked());
  out_q[m->get_priority()].push_back(m);
  cond.Signal();
}
```
