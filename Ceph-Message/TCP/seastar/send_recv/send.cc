#include "http/http_response_parser.hh"
#include "core/print.hh"
#include "core/reactor.hh"
#include "core/app-template.hh"
#include "core/future-util.hh"
#include "core/distributed.hh"
#include "core/semaphore.hh"
#include "core/future-util.hh"
#include <chrono>
using namespace seastar;
using namespace std;
namespace bpo = boost::program_options;


template <typename... Args>
void http_debug(const char* fmt, Args&&... args) { // what does this ... represent? is there a good phrase so I can learn more about this in google?
#if HTTP_DEBUG
    print(fmt, std::forward<Args>(args)...);
#endif
}

class http_client {
private:
public:
  class connection {
    private:
    public:


  }


};


int main(int ac, char** av) {
  app_template app;
  cout << "program start" << endl;

  // this part automatically gets parses all the arguments for you
  app.add_options()
      ("server,s", bpo::value<std::string>()->default_value("10.218.111.252:10000"), "Server address")
      ("conn,c", bpo::value<unsigned>()->default_value(100), "total connections")
      ("reqs,r", bpo::value<unsigned>()->default_value(0), "reqs per connection")
      ("duration,d", bpo::value<unsigned>()->default_value(10), "duration of the test in seconds)");


  return app.run(ac, av, [&app] () -> future<int> { // what does this part actually mean? the app already has a pointer?
                                                    // because app-template.hh does not have any direct a future function and I am
                                                    // wondering hiw did this is possible in the first place
    auto& config = app.configuration();
    auto server = config["server"].as<std::string>(); // server ip address
    auto reqs_per_conn = config["reqs"].as<unsigned>(); // requests per connection
    auto total_conn= config["conn"].as<unsigned>(); // number of connections
    auto duration = config["duration"].as<unsigned>(); // duration


    if (total_conn % smp::count != 0) { // future count should have numbers
        print("Error: conn needs to be n * cpu_nr\n");
        return make_ready_future<int>(-1);
    }

    auto http_clients = new distributed<http_client>;
    // Start http requests on all the cores
    auto started = steady_clock_type::now();
    return http_clients->start(std::move(duration), std::move(total_conn), std::move(reqs_per_conn)).then([http_clients, server] {
      return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server});
    });







    cout << "program ended" << endl;
  });
}
