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
         auto started = steady_clock_type::now();

        return http_clients->start(move(duration), move(total_conn), move(reqs_per_conn)).then([http_clients, server] {
          return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server});
        });




          cout << "END" << endl;
          return make_ready_future<int>(0); // this terminates the future
      });
}
