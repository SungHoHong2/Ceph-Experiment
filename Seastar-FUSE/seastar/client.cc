#include <math.h>
#include <vector>
#include "core/app-template.hh"
#include "core/future-util.hh"
#include "core/distributed.hh"

using namespace seastar;
using namespace net;
using namespace std::chrono_literals;

static int rx_msg_size = 4 * 1024;
static int tx_msg_total_size = 1000 * 1024 * 1024;
static int tx_msg_size = 4 * 1024;
static int tx_msg_nr = tx_msg_total_size / tx_msg_size;
static std::string str_txbuf(tx_msg_size, 'X');
static int pingpong_size = 4 * 1024;
static std::string str_ping(pingpong_size, 'X');
static std::string str_pong(pingpong_size, 'X');
static std::string str_txtx(pingpong_size, 'T');
static std::string str_rxrx(pingpong_size, 'R');
static std::vector<double> samples;


class client;
distributed<client> clients;
transport protocol = transport::TCP;

class client {
private:
    unsigned _total_pings;
    unsigned _concurrent_connections;
    ipv4_addr _server_addr;
    std::string _test;
    lowres_clock::time_point _earliest_started;
    lowres_clock::time_point _latest_finished;
    size_t _processed_bytes;
    unsigned _num_reported;
public:
    class connection {
        connected_socket _fd;
        input_stream<char> _read_buf;
        output_stream<char> _write_buf;
        size_t _bytes_read = 0;
        size_t _bytes_write = 0;
    public:
        connection(connected_socket&& fd)
                : _fd(std::move(fd))
                , _read_buf(_fd.input())
                , _write_buf(_fd.output()) {}

        future<> do_read() {
            auto read_started = lowres_clock::now();
            return _read_buf.read_exactly(rx_msg_size).then([this, read_started](temporary_buffer<char> buf) {
                _bytes_read += buf.size();

                if (buf.size() == 0) {
                    return make_ready_future();
                } else {
                    auto read_finished = lowres_clock::now();
                    auto elapsed = read_finished - read_started;
                    double msecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                    // std::cout << "READ: " << buf.size() << "::" << msecs << std::endl;
                    samples.push_back(msecs);

                    return do_read();
                }
            });
        }

        future<> do_write(int end) {
            if (end == 0) {
                return make_ready_future();
            }

            auto write_started = lowres_clock::now();
            return _write_buf.write(str_txbuf).then([this] {
                _bytes_write += tx_msg_size;
                return _write_buf.flush();
            }).then([this, end, write_started] {
                auto write_finished = lowres_clock::now();
                auto elapsed = write_finished - write_started;
                double msecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                // std::cout << "WRITE: " << "::" << msecs << std::endl;
                samples.push_back(msecs);

                return do_write(end - 1);
            });
        }


        future<> ping(int times) {

            auto ping_started = lowres_clock::now();

            return _write_buf.write(str_ping).then([this] {
                return _write_buf.flush();
            }).then([this, times, ping_started] {
                return _read_buf.read_exactly(pingpong_size).then([this, times, ping_started] (temporary_buffer<char> buf) {

                    if (buf.size() != pingpong_size) {
                        fprint(std::cerr, "illegal packet received: %d\n", buf.size());
                        return make_ready_future();
                    }
                    auto str = std::string(buf.get(), buf.size());
                    if (str != str_pong) {
                        fprint(std::cerr, "illegal packet received: %d\n", buf.size());
                        return make_ready_future();
                    }

                    if (times > 0) {
                        auto ping_finished = lowres_clock::now();
                        auto elapsed = ping_finished - ping_started;
                        double msecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                        // std::cout << "PING: " << buf.size() << "::" << msecs << std::endl;
                        samples.push_back(msecs);

                        return ping(times - 1);
                    } else {
                        return make_ready_future();
                    }
                });
            });
        }


        future<size_t> rxrx() {
            return _write_buf.write(str_rxrx).then([this] {
                return _write_buf.flush();
            }).then([this] {
                return do_write(tx_msg_nr).then([this] {
                    return _write_buf.close();
                }).then([this] {
                    return make_ready_future<size_t>(_bytes_write);
                });
            });
        }

        future<size_t> txtx() {
            return _write_buf.write(str_txtx).then([this] {
                return _write_buf.flush();
            }).then([this] {
                return do_read().then([this] {
                    return make_ready_future<size_t>(_bytes_read);
                });
            });
        }


    };

    future<> ping_test(connection *conn) {
        auto started = lowres_clock::now();
        return conn->ping(_total_pings).then([started] {
            auto finished = lowres_clock::now();
            clients.invoke_on(0, &client::ping_report, started, finished);
        });
    }

    future<> rxrx_test(connection *conn) {
        auto started = lowres_clock::now();
        return conn->rxrx().then([started] (size_t bytes) {
            auto finished = lowres_clock::now();
            clients.invoke_on(0, &client::rxtx_report, started, finished, bytes);
        });
    }

    future<> txtx_test(connection *conn) {
        auto started = lowres_clock::now();
        return conn->txtx().then([started] (size_t bytes) {
            auto finished = lowres_clock::now();
            clients.invoke_on(0, &client::rxtx_report, started, finished, bytes);
        });
    }


    void ping_report(lowres_clock::time_point started, lowres_clock::time_point finished) {
        if (_earliest_started > started)
            _earliest_started = started;
        if (_latest_finished < finished)
            _latest_finished = finished;
        if (++_num_reported == _concurrent_connections) {

            int size = samples.size();
            double avg =0;
            for (int i = 0; i < size; i++)
            {
                avg += samples[i];
            }

            avg = avg/size;
            double variance = 0;
            double t = samples[0];
            for (int i = 1; i < size; i++)
            {
                t += samples[i];
                double diff = ((i + 1) * samples[i]) - t;
                variance += (diff * diff) / ((i + 1.0) *i);
            }

            double std_var = variance / (size - 1);
            double std_dev = sqrt(std_var);


            auto elapsed = finished - started;
            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            // auto secs = static_cast<double>(usecs) / static_cast<double>(1000 * 1000);
            fprint(std::cout, "========== ping ============\n");
            fprint(std::cout, "Server: %s\n", _server_addr);
            fprint(std::cout,"Connections: %u\n", _concurrent_connections);
            fprint(std::cout, "Total PingPongs: %u\n", _total_pings);
            fprint(std::cout, "Total Time(micro): %f\n", static_cast<double>(usecs));
//            fprint(std::cout, "Total Time(Secs): %f\n", secs);
//            fprint(std::cout, "Requests/Sec: %f\n", static_cast<double>(_total_pings) / secs);
            fprint(std::cout, "AVG per request(micro): %f\n", avg);
            fprint(std::cout, "STDEV per request(micro): %f\n", std_dev);

            clients.stop().then([] {
                engine().exit(0);
            });
        }
    }


    void rxtx_report(lowres_clock::time_point started, lowres_clock::time_point finished, size_t bytes) {
        if (_earliest_started > started)
            _earliest_started = started;
        if (_latest_finished < finished)
            _latest_finished = finished;
        _processed_bytes += bytes;
        if (++_num_reported == _concurrent_connections) {


            int size = samples.size();
            double avg =0;
            for (int i = 0; i < size; i++)
            {
                avg += samples[i];
            }

            avg = avg/size;
            double variance = 0;
            double t = samples[0];
            for (int i = 1; i < size; i++)
            {
                t += samples[i];
                double diff = ((i + 1) * samples[i]) - t;
                variance += (diff * diff) / ((i + 1.0) *i);
            }

            double std_var = variance / (size - 1);
            double std_dev = sqrt(std_var);


            auto elapsed = finished - started;
            auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            // auto secs = static_cast<double>(usecs) / static_cast<double>(1000 * 1000);
            fprint(std::cout, "========== %s ============\n", _test);
            fprint(std::cout, "Server: %s\n", _server_addr);
            fprint(std::cout, "Connections: %u\n", _concurrent_connections);
            fprint(std::cout, "Bytes Received(MiB): %u\n", _processed_bytes/1024/1024);
            fprint(std::cout, "Total Time(micro): %f\n", static_cast<double>(usecs));
            fprint(std::cout, "AVG per request(micro): %f\n", avg);
            fprint(std::cout, "STDEV per request(micro): %f\n", std_dev);

            // fprint(std::cout, "Total Time(Secs): %f\n", secs);
            // fprint(std::cout, "Bandwidth(Gbits/Sec): %f\n",
            //       static_cast<double>((_processed_bytes * 8)) / (1000 * 1000 * 1000) / secs);
            clients.stop().then([] {
                engine().exit(0);
            });
        }
    }

    future<> start(ipv4_addr server_addr, std::string test, unsigned ncon) {
        _server_addr = server_addr;
        _concurrent_connections = ncon * smp::count;
        _total_pings = tx_msg_nr * _concurrent_connections;
        _test = test;


        // CHARA: CORRECT_TIME
//        auto temp_start = std::chrono::high_resolution_clock::now();
//        static_start=chrono::duration_cast<chrono::microseconds>(temp_start).count();


        for (unsigned i = 0; i < ncon; i++) {
            socket_address local = socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}});
            engine().net().connect(make_ipv4_address(server_addr), local, protocol).then([this, test] (connected_socket fd) {
                auto conn = new connection(std::move(fd));
                (this->*tests.at(test))(conn).then_wrapped([conn] (auto&& f) {
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
    }
    future<> stop() {
        return make_ready_future();
    }

    typedef future<> (client::*test_fn)(connection *conn);
    static const std::map<std::string, test_fn> tests;
};

namespace bpo = boost::program_options;

int main(int ac, char ** av) {
    app_template app;
    app.add_options()
            ("server", bpo::value<std::string>()->default_value("192.168.56.101:1234"), "Server address")
            ("test", bpo::value<std::string>()->default_value("ping"), "test type(ping | rxrx | txtx)")
            ("conn", bpo::value<unsigned>()->default_value(1), "nr connections per cpu")
            ("proto", bpo::value<std::string>()->default_value("tcp"), "transport protocol tcp|sctp");

    return app.run_deprecated(ac, av, [&app] {
        auto&& config = app.configuration();
        auto server = config["server"].as<std::string>();
        auto test = config["test"].as<std::string>();
        auto ncon = config["conn"].as<unsigned>();
        auto proto = config["proto"].as<std::string>();

        if (proto == "tcp") {
            protocol = transport::TCP;
        } else if (proto == "sctp") {
            protocol = transport::SCTP;
        } else {
            fprint(std::cerr, "Error: --proto=tcp|sctp\n");
            return engine().exit(1);
        }

        if (!client::tests.count(test)) {
            fprint(std::cerr, "Error: -test=ping | rxrx | txtx\n");
            return engine().exit(1);
        }

        clients.start().then([server, test, ncon] () {
            clients.invoke_on_all(&client::start, ipv4_addr{server}, test, ncon);
        });
    });
}

const std::map<std::string, client::test_fn> client::tests = {
        {"ping", &client::ping_test},
        {"rxrx", &client::rxrx_test},
        {"txtx", &client::txtx_test},
};
