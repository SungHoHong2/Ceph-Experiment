using namespace seastar;
using namespace net;
using namespace std::chrono_literals;
static int total_ping_identifier = 0;

class client;
distributed<client> clients;

transport protocol = transport::TCP;

class client {
private:
    static constexpr unsigned _pings_per_connection = 10000;
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

        future<> ping(int times) {

            std::string str = "";
            int v, tenth;
            int total = 4;

            std::string s = std::to_string(total_ping_identifier);
            tenth = s.length();
            for(v=0; v<(total-tenth); v++){
                str.append("0");
            }
            str.append(s);

            total_ping_identifier++;
            if(str.length()>4){
                str = "ping";
            }

            return _write_buf.write(str).then([this] {
                return _write_buf.flush();
            }).then([this, times, str] {

                std::cout << "WRITE::"<< str << std::endl;
                if (times > 0) {
                    return ping(times - 1);
                } else {
                    return make_ready_future();
                }

            });
        }
    };


    future<> start(ipv4_addr server_addr, std::string test, unsigned ncon) {
        _server_addr = server_addr;
        _concurrent_connections = ncon * smp::count;
        _total_pings = _pings_per_connection * _concurrent_connections;
        _test = test;

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
