### adding request to the queue

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

  // first, we create a Rados object and initialize it
  librados::Rados rados;
  {
    ret = rados.init("admin"); // just use the client.admin keyring
    if (ret < 0) { // let's handle any error that might have come back
      std::cerr << "couldn't initialize rados! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just set up a rados cluster object" << std::endl;
    }
  }

  /*
   * Now we need to get the rados object its config info. It can
   * parse argv for us to find the id, monitors, etc, so let's just
   * use that.
   */
  {
    ret = rados.conf_parse_argv(argc, argv);
    if (ret < 0) {
      // This really can't happen, but we need to check to be a good citizen.
      std::cerr << "failed to parse config options! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just parsed our config options" << std::endl;
      // We also want to apply the config file if the user specified
      // one, and conf_parse_argv won't do that for us.
      for (int i = 0; i < argc; ++i) {
	if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
	  ret = rados.conf_read_file(argv[i+1]);
	  if (ret < 0) {
	    // This could fail if the config file is malformed, but it'd be hard.
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

  /*
   * next, we actually connect to the cluster
   */
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
[rados_connect](rados_connect.md)

```
  /*
   * create an "IoCtx" which is used to do IO to a pool
   */
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

[rados_ioctx_create](rados_ioctx_create.md)

```

  /*
   * now let's do some IO to the pool! We'll write "hello world!" to a
   * new object.
   */
   {
    librados::bufferlist bl;
    bl.append(hello);

    /*
     * now that we have the data to write, let's send it to an object.
     * We'll use the synchronous interface for simplicity.
     */
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

> [io_ctx_write_full](io_ctx_write_full.md)

```
  /*
   * now let's read that object back! Just for fun, we'll do it using
   * async IO instead of synchronous. (This would be more useful if we
   * wanted to send off multiple reads at once; see
   * http://docs.ceph.com/docs/master/rados/api/librados/#asychronous-io )
   */
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

  out:
  rados.shutdown();
  return ret;
}
```

<br>

### UNSOLVED Fragments

>  /home/sungho/ceph/src/simple/Pipe.cc:318

```
void SimpleMessenger::ready()
{
  ldout(cct,10) << "ready " << get_myaddr() << dendl;
  dispatch_queue.start();

```
> [dispatcher_queue_start](dispatcher_queue_start.md)
```
  lock.Lock();
  if (did_bind)
    accepter.start();
  lock.Unlock();
}

```
>  /home/sungho/ceph/src/msg/simple/Pipe.cc:796

>  /home/sungho/ceph/src/msg/simple/Pipe.cc:367

>  /home/sungho/ceph/src/msg/simple/Pipe.cc:221

```

void Pipe::start_reader()
{
  assert(pipe_lock.is_locked());
  assert(!reader_running);
  if (reader_needs_join) {
    reader_thread.join();
    reader_needs_join = false;
  }
  reader_running = true;
  reader_thread.create("ms_pipe_read", msgr->cct->_conf->ms_rwthread_stack_bytes);
}

void Pipe::start_writer()
{
  assert(pipe_lock.is_locked());
  assert(!writer_running);
  writer_running = true;
  writer_thread.create("ms_pipe_write", msgr->cct->_conf->ms_rwthread_stack_bytes);
}

int Pipe::do_sendmsg(struct msghdr *msg, unsigned len, bool more){
r = ::sendmsg(sd, msg, MSG_NOSIGNAL | (more ? MSG_MORE : 0));

int Pipe::read_message(Message **pm, AuthSessionHandler* auth_handler)
int Pipe::tcp_read(char *buf, unsigned len)
ssize_t Pipe::tcp_read_nonblocking(char *buf, unsigned len)
ssize_t got = buffered_recv(buf, len, MSG_DONTWAIT );
ssize_t ret = do_recv(buf, left, flags );
ssize_t got = ::recv( sd, buf, len, flags );

```
