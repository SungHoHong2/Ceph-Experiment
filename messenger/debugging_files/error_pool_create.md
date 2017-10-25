### Solution

<br>

### Debug Log

- **/ceph/examples/librados/hello_world_cpp**
- LINE 94
- LINE 106
```cpp
{
  ret = rados.pool_create(pool_name);
  if (ret < 0) {
    std::cerr << "couldn't create pool! error " << ret << std::endl;
    return EXIT_FAILURE;
  } else {
    std::cout << "we just created a new pool named " << pool_name << std::endl;
  }
}

{
  ret = rados.ioctx_create(pool_name, io_ctx);
  if (ret < 0) {
    std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
    ret = EXIT_FAILURE;
    goto out;
  } else {
    std::cout << "we just created an ioctx for our pool" << std::endl;
  }
}

```

<br>

- **/ceph/examples/librados/librados.cc**
    - LINE 2363
```cpp
int librados::Rados::pool_create(const char *name)
{
  string str(name);
  return client->pool_create(str);
}

```

<br>

- **/ceph/examples/librados/RadosClient.cc**
    - LINE: 671 - int r = wait_for_osdmap();
```cpp
int librados::RadosClient::pool_create(string& name, unsigned long long auid,
				       int16_t crush_rule)
{
int r = wait_for_osdmap();
if (r < 0) {
  return r;
}

Mutex mylock ("RadosClient::pool_create::mylock");
int reply;
Cond cond;
bool done;
Context *onfinish = new C_SafeCond(&mylock, &cond, &done, &reply);
reply = objecter->create_pool(name, onfinish, auid, crush_rule);

if (reply < 0) {
  delete onfinish;
} else {
  mylock.Lock();
  while(!done)
    cond.Wait(mylock);
  mylock.Unlock();
}
return reply;
}
```

<br>

- **/ceph/examples/librados/hello_world_cpp**
```cpp

```


<br>

- **/ceph/examples/librados/hello_world_cpp**
```cpp

```
