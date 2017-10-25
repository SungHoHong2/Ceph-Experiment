### Solution

<br>

### Debug Log

- **/ceph/examples/librados/hello_world_cpp**
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
```cpp



```



<br>

- **/ceph/examples/librados/hello_world_cpp**
```cpp

```
