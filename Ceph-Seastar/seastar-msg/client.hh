using namespace seastar;
using namespace net;
using namespace std::chrono_literals;
static int total_ping_identifier = 0;

class client;
distributed<client> clients;
transport protocol = transport::TCP;


void task1(char msg[])
{
    while(1) {
        while(send_size!=0) sleep(1);
        memcpy(send_packet, msg, 6);
        send_size=6;
    }
}


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

        future<> ping() {
            sleep(1);

            memset(_send_packet, 0, PACKET_SIZE);
            if(send_size!=0){
                memcpy(_send_packet, send_packet, send_size);
                send_size=0;
            }else {
                memcpy(_send_packet, "^", 1);
            }

            std::cout << "WRITE BEFORE::" << _send_packet << std::endl;

            return _write_buf.write(_send_packet).then([this] {
                return _write_buf.flush();
            }).then([this] {

                if(strlen(_send_packet)>2) {
                    std::cout << "WRITE AFTER::" << _send_packet << std::endl;
                }

                return ping();

            });
        }
    };

    future<> ping_test(connection *conn) {
        auto started = lowres_clock::now();
        return conn->ping().then([started] {
            auto finished = lowres_clock::now();
            clients.invoke_on(0, &client::ping_report, started, finished);
        });
    }

    void ping_report(lowres_clock::time_point started, lowres_clock::time_point finished) {
        if (_earliest_started > started)
            _earliest_started = started;
        if (_latest_finished < finished)
            _latest_finished = finished;
        if (++_num_reported == _concurrent_connections) {
            clients.stop().then([] {
                // engine().exit(0);
            });
        }
    }


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
