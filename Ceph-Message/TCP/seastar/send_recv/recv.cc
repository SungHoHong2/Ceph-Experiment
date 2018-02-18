#include "http/httpd.hh"
#include "http/handlers.hh"
#include "http/function_handlers.hh"
#include "http/file_handler.hh"
#include "apps/httpd/demo.json.hh"
#include "http/api_docs.hh"

namespace bpo = boost::program_options;
using namespace seastar;
using namespace httpd;

int main(int ac, char** av) {
  app_template app;
  app.add_options()("port", bpo::value<uint16_t>()->default_value(10000),"HTTP Server port");
  return
}
