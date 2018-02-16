/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (C) 2015 Cloudius Systems, Ltd.
 */

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


template <typename... Args>
void http_debug(const char* fmt, Args&&... args) {
#if HTTP_DEBUG
    print(fmt, std::forward<Args>(args)...);
#endif
}

class http_client {
private:
    unsigned _duration;
    unsigned _conn_per_core;
    unsigned _reqs_per_conn;
    std::vector<connected_socket> _sockets;
    semaphore _conn_connected{0};
    semaphore _conn_finished{0};
    timer<> _run_timer;
    bool _timer_based;
    bool _timer_done{false};
    // uint64_t _total_reqs{0};

public:
  http_client(unsigned duration, unsigned total_conn, unsigned reqs_per_conn)
    : _duration(duration)
    , _conn_per_core(total_conn / smp::count)
    , _reqs_per_conn(reqs_per_conn)
    , _run_timer([this] { _timer_done = true; })
    , _timer_based(reqs_per_conn == 0) {
  }

  future<> connect(ipv4_addr server_addr) {
    // Establish all the TCP connections first
    for (unsigned i = 0; i < _conn_per_core; i++) {
        engine().net().connect(make_ipv4_address(server_addr)).then([this] (connected_socket fd) {
            _sockets.push_back(std::move(fd));
            http_debug("Established connection %6d on cpu %3d\n", _conn_connected.current(), engine().cpu_id());
            _conn_connected.signal();
        }).or_terminate();
    }
    return _conn_connected.wait(_conn_per_core);
}

  future<> stop() {
      return make_ready_future();
  }

};


namespace bpo = boost::program_options;
int main(int ac, char** av) {
  cout << "BEGIN" << endl;
  app_template app;
  app.add_options()
      ("server,s", bpo::value<std::string>()->default_value("10.218.111.252:10000"), "Server address")
      ("conn,c", bpo::value<unsigned>()->default_value(100), "total connections")
      ("reqs,r", bpo::value<unsigned>()->default_value(0), "reqs per connection")
      ("duration,d", bpo::value<unsigned>()->default_value(10), "duration of the test in seconds)");

      return app.run(ac, av, [&app] () -> future<int> {

        auto& config = app.configuration();
        auto server = config["server"].as<std::string>();
        auto reqs_per_conn = config["reqs"].as<unsigned>();
        auto total_conn= config["conn"].as<unsigned>();
        auto duration = config["duration"].as<unsigned>();

        if (total_conn % smp::count != 0) {
          print("Error: conn needs to be n * cpu_nr\n"); // smp should not be 0
          return make_ready_future<int>(-1); // terminate the application
        }


         auto http_clients = new distributed<http_client>;
         // auto started = steady_clock_type::now();

        return http_clients->start(move(duration), move(total_conn), move(reqs_per_conn)).then([http_clients, server] {
          cout << "http_clients->start" << endl;
          return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server});
          // return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server});
        }).then([http_clients] {

           return make_ready_future<int>(0); // this terminates the future
        });


          cout << "END" << endl;
          return make_ready_future<int>(0); // this terminates the future
      });
}
