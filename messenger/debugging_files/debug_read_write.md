### WRITE DEBUG

- **main.cc**
```cpp
/*
 * create an "IoCtx" which is used to do IO to a pool
 */
{
  ret = rados.ioctx_create("test_pool", io_ctx);
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

- **main.cc**
```cpp
```


<br>

- **main.cc**
```cpp
```
