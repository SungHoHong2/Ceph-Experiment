```
#include <rados/librados.hpp>
#include <iostream>
#include <string>

int main(int argc, const char **argv)
{
  int ret = 0;

  // we will use all of these below
  // const char *pool_name = "hello_world_pool";
  std::string hello("hello world!");
  std::string object_name("hello_object");
  librados::IoCtx io_ctx;
```
> first, we create a Rados object and initialize it
```
  librados::Rados rados;
  {
    ret = rados.init("admin");
```
>  use the client.admin keyring
```
    if (ret < 0) { // let's handle any error that might have come back
      std::cerr << "couldn't initialize rados! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just set up a rados cluster object" << std::endl;
    }
  }

```

> get the rados object its config info.

```
  {
    ret = rados.conf_parse_argv(argc, argv);
    if (ret < 0) {
      std::cerr << "failed to parse config options! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      for (int i = 0; i < argc; ++i) {
	       if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
	          ret = rados.conf_read_file(argv[i+1]);
	           if (ret < 0) {
	              std::cerr << "failed to parse config file " << argv[i+1]
	              << "! error" << ret << std::endl;
	               ret = EXIT_FAILURE;
	                goto out;
      	        }
      	  break;
      	}
      }
    }
  }

```
> connect to the cluster

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
> create an "IoCtx" which is used to do IO to a pool

```
  {
    ret = rados.ioctx_create("scbench", io_ctx);
    if (ret < 0) {
      std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just created an ioctx for our pool" << std::endl;
    }
  }

```

> write hello world in the real object

```
   {
    /*
     * "bufferlist"s are Ceph's native transfer type, and are carefully
     * designed to be efficient about copying. You can fill them
     * up from a lot of different data types, but strings or c strings
     * are often convenient. Just make sure not to deallocate the memory
     * until the bufferlist goes out of scope and any requests using it
     * have been finished!
     */
    librados::bufferlist bl;
    bl.append(hello);

```

> with the data above send it to an object

```
    ret = io_ctx.write_full(object_name, bl);
    if (ret < 0) {
      std::cerr << "couldn't write object! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just wrote new object " << object_name
	        << ", with contents\n" << hello << std::endl;
    }
  }

```

> read the object back with async IO

```
  {
    librados::bufferlist read_buf;
    int read_len = 4194304; // this is way more than we need
    // allocate the completion from librados
    librados::AioCompletion *read_completion = librados::Rados::aio_create_completion();
    // send off the request.
    ret = io_ctx.aio_read(object_name, read_completion, &read_buf, read_len, 0);
    if (ret < 0) {
      std::cerr << "couldn't start read object! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    }
    // wait for the request to complete, and check that it succeeded.
    read_completion->wait_for_complete();
    ret = read_completion->get_return_value();
    if (ret < 0) {
      std::cerr << "couldn't read object! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we read our object " << object_name
	  << ", and got back " << ret << " bytes with contents\n";
      std::string read_string;
      read_buf.copy(0, ret, read_string);
      std::cout << read_string << std::endl;
    }
  }
```

```
  rados.shutdown();
  return ret;
}

```
