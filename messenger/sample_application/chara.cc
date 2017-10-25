#include <rados/librados.hpp>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, const char **argv){
  int ret = 0;

  const char *pool_name = "hello_world_pool";
  string hello("hello world!");
  string object_name("hello_object");
  librados::IoCtx io_ctx;

  // create RADOS
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
    ret = rados.conf_parse_argv(argc, argv); // there is no data
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



  /*
   * now let's do some IO to the pool! We'll write "hello world!" to a
   * new object.
   */

   {
     librados::bufferlist bl;
     bl.append(hello);

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


    /*
      * And if we want to be really cool, we can do multiple things in a single
      * atomic operation. For instance, we can update the contents of our object
      * and set the version at the same time.
      */
     {
       librados::bufferlist bl;
       bl.append(hello);
       bl.append("v2");
       librados::ObjectWriteOperation write_op;
       write_op.write_full(bl);
       librados::bufferlist version_bl;
       version_bl.append('2');
       write_op.setxattr("version", version_bl);
       ret = io_ctx.operate(object_name, &write_op);
       if (ret < 0) {
         std::cerr << "failed to do compound write! error " << ret << std::endl;
         ret = EXIT_FAILURE;
         goto out;
       } else {
         std::cout << "we overwrote our object " << object_name
   		<< " with contents\n" << bl.c_str() << std::endl;
       }
     }




  out:

  return ret;
}
