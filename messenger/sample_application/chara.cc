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



  return ret;
}
