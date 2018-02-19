#include "core/app-template.hh"
#include "core/future-util.hh"
#include "core/distributed.hh"

using namespace seastar;
using namespace net;
using namespace std::chrono_literals;

class client;
distributed<client> clients; // distributed: Invoke a method on a specific instance of Service.
transport protocol = transport::TCP; // protocol used to transport data

class client {
private:
    static constexpr unsigned _pings_per_connection = 10000;
    unsigned _total_pings;
    unsigned _concurrent_connections;
    ipv4_addr _server_addr;
    std::string _test;
    lowres_clock::time_point _earliest_started; // time used to count latency
    lowres_clock::time_point _latest_finished; // time sued to count latency
    unsigned _num_reported; // compare the total number of pingpongs and end the loop
public:
    class connection {
        connected_socket _fd;
        input_stream<char> _read_buf;
        output_stream<char> _write_buf;
    public:
        connection(connected_socket&& fd)
            : _fd(std::move(fd))
            , _read_buf(_fd.input())
            , _write_buf(_fd.output()) {}

        // ping function
        future<> ping(int times) {
            return _write_buf.write("ping").then([this] {
                return _write_buf.flush(); // send string data ping
            }).then([this, times] {
                return _read_buf.read_exactly(4).then([this, times] (temporary_buffer<char> buf) { // read only when the file size is 4
                    auto str = std::string(buf.get(), buf.size());  // get the string data from the server "pong"
                    // std::cout << str << std::endl;
                    if (times > 0) {
                        return ping(times - 1); // continue the ping for designated amount of time
                    } else {
                        return make_ready_future(); // finish ping
                    }
                });
            });
        }
    };

    future<> ping_test(connection *conn) {
        auto started = lowres_clock::now(); // start timing the clock
        return conn->ping(_pings_per_connection).then([started] {
            auto finished = lowres_clock::now(); // stop timing the clock
            clients.invoke_on(0, &client::ping_report, started, finished); // run pingpong report after transfer
        });
    }


    void ping_report(lowres_clock::time_point started, lowres_clock::time_point finished) {
        if (_earliest_started > started)
            _earliest_started = started;
        if (_latest_finished < finished)
            _latest_finished = finished;
        if (++_num_reported == _concurrent_connections) {
            auto elapsed = _latest_finished - _earliest_started;
            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            auto secs = static_cast<double>(usecs) / static_cast<double>(1000 * 1000);
            fprint(std::cout, "========== ping ============\n");
            fprint(std::cout, "Server: %s\n", _server_addr);
            fprint(std::cout,"Connections: %u\n", _concurrent_connections);
            fprint(std::cout, "Total PingPong: %u\n", _total_pings);
            fprint(std::cout, "Total Time(Secs): %f\n", secs);
            fprint(std::cout, "Requests/Sec: %f\n",
                static_cast<double>(_total_pings) / secs);
            clients.stop().then([] {
                engine().exit(0);
            });
        }
    }

    future<> start(ipv4_addr server_addr, std::string test, unsigned ncon) {
        _server_addr = server_addr; // assign the ip address
        _concurrent_connections = ncon * smp::count; // multiple the total available cores and the communication
        _total_pings = _pings_per_connection * _concurrent_connections; // multiple total number of pings and concurrent connection
        _test = test; // assign ping

        for (unsigned i = 0; i < ncon; i++) { // iterate all connections
            socket_address local = socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}}); // get socket address

            engine().net().connect(make_ipv4_address(server_addr), local, protocol) // connect with TCP protocol
            .then([this, test] (connected_socket fd) {
                auto conn = new connection(std::move(fd)); // create connection
               (this->client::ping_test)(conn).then_wrapped([conn] (auto&& f) {
                // (this->*tests.at(test))(conn).then_wrapped([conn] (auto&& f) {
                    delete conn;
                    try {
                        f.get();
                    } catch (std::exception& ex) {
                        fprint(std::cerr, "request error: %s\n", ex.what());
                    }
                });
            });
        }
        return make_ready_future();
        // Creates a future in an available, value state.
    }
    future<> stop() {
        return make_ready_future();
    }

    typedef future<> (client::*test_fn)(connection *conn);
    static const std::map<std::string, test_fn> tests;
};

namespace bpo = boost::program_options;

int main(int ac, char ** av) {
    app_template app; // starts the seastar engine
    return app.run_deprecated(ac, av, [&app] {  // run application
          auto server = "10.218.111.253:1234"; // assign ip address
          // TCP: 10.218.105.75:1234
          // DPDK: 10.218.111.252:1234
          std::string test = "ping"; // assing function ping_test
          unsigned ncon = 16; // assign number of connections per core
          protocol = transport::TCP;  // assign protocol
          clients.start().then([server, test, ncon] () {
              clients.invoke_on_all(&client::start, ipv4_addr{server}, test, ncon); // this is one function invoking
              // Invoke a callable on all instances of Service.
          });
    });
}
