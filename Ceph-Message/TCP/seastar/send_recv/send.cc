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
    uint64_t _total_reqs{0};

public:
  http_client(unsigned duration, unsigned total_conn, unsigned reqs_per_conn)
    : _duration(duration)
    , _conn_per_core(total_conn / smp::count)
    , _reqs_per_conn(reqs_per_conn)
    , _run_timer([this] { _timer_done = true; })
    , _timer_based(reqs_per_conn == 0) {
  }


  class connection {

    private:
        connected_socket _fd;
        input_stream<char> _read_buf;
        output_stream<char> _write_buf;
        http_response_parser _parser;
        http_client* _http_client;
        uint64_t _nr_done{0};
    public:
        connection(connected_socket&& fd, http_client* client)
            : _fd(std::move(fd))
            , _read_buf(_fd.input())
            , _write_buf(_fd.output())
            , _http_client(client){
        }

        uint64_t nr_done() {
            return _nr_done;
        }

        future<> do_req() {
            return _write_buf.write("howdy").then([this] {

                cout << "writing? 1" << endl;

                return _write_buf.flush();
            }).then([this] {

              cout << "reading? 0" << endl;

                _parser.init();
                return _read_buf.consume(_parser).then([this] {
                    // Read HTTP response header first

                    cout << "reading? 1" << endl;

                    if (_parser.eof()) {
                        return make_ready_future<>();
                    }


                    cout << "reading? 2" << endl;

                    auto _rsp = _parser.get_parsed_response();
                    auto it = _rsp->_headers.find("Content-Length");
                    if (it == _rsp->_headers.end()) {
                        print("Error: HTTP response does not contain: Content-Length\n");
                        return make_ready_future<>();
                    }


                    cout << "reading? 3" << endl;

                    auto content_len = std::stoi(it->second);
                    http_debug("Content-Length = %d\n", content_len);
                    // Read HTTP response body

                    cout << "reading? 4" << endl;
                    return _read_buf.read_exactly(content_len).then([this] (temporary_buffer<char> buf) {
                        _nr_done++;
                        http_debug("%s\n", buf.get());
                        if (_http_client->done(_nr_done)) {
                            return make_ready_future();
                        } else {
                            return do_req();
                        }
                    });
                });
            });
        }
  };


  future<uint64_t> total_reqs() {
      print("Requests on cpu %2d: %ld\n", engine().cpu_id(), _total_reqs);
      return make_ready_future<uint64_t>(_total_reqs);
  }

  bool done(uint64_t nr_done) {
      if (_timer_based) {
          return _timer_done;
      } else {
          return nr_done >= _reqs_per_conn;
      }
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

  future<> run() {
      // All connected, start HTTP request
      http_debug("Established all %6d tcp connections on cpu %3d\n", _conn_per_core, engine().cpu_id());
      if (_timer_based) {
          _run_timer.arm(std::chrono::seconds(_duration));
      }
      for (auto&& fd : _sockets) {
          auto conn = new connection(move(fd), this);
          conn->do_req().then_wrapped([this, conn] (auto&& f) {
              // http_debug("Finished connection %6d on cpu %3d\n", _conn_finished.current(), engine().cpu_id());
              _total_reqs += conn->nr_done();
              _conn_finished.signal();
              delete conn;
              try {
                  f.get();
              } catch (std::exception& ex) {
                  print("http request error: %s\n", ex.what());
              }
          });
      }
      // All finished
      return _conn_finished.wait(_conn_per_core);
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
        auto started = steady_clock_type::now();

        return http_clients->start(move(duration), move(total_conn), move(reqs_per_conn)).then([http_clients, server] {
          cout << "http_clients->connect" << endl;
          return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server}); // this connects to the server
          // return http_clients->invoke_on_all(&http_client::connect, ipv4_addr{server});
        }).then([http_clients] {
          cout << "http_clients->run" << endl;
          return http_clients->invoke_on_all(&http_client::run);
        }).then([http_clients] {
          cout << "http_clients->map_reduce" << endl;
          return http_clients->map_reduce(adder<uint64_t>(), &http_client::total_reqs);
        }).then([http_clients, started] (auto total_reqs){

          auto finished = steady_clock_type::now();
          auto elapsed = finished - started;
          auto secs = static_cast<double>(elapsed.count() / 1000000000.0);
          print("Total cpus: %u\n", smp::count);
          print("Total requests: %u\n", total_reqs);
          print("Total time: %f\n", secs);
          print("Requests/sec: %f\n", static_cast<double>(total_reqs) / secs);
          print("==========     done     ============\n");

          return http_clients->stop().then([http_clients] {
              delete http_clients;
              return make_ready_future<int>(0);
          });
        });

          cout << "END" << endl;
          return make_ready_future<int>(0); // this terminates the future
      });
}
