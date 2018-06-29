using namespace seastar;
using namespace net;
using namespace std::chrono_literals;

class client;
distributed<client> clients;
transport protocol = transport::TCP;


void task1(char msg[])
{
    while(1) {
        while(send_size!=0) sleep(1);
        memcpy(send_packet, msg, 5);
        send_size=5;
    }
}


class client {
private:
    static constexpr unsigned _pings_per_connection = 10000;
    unsigned _total_pings;
    unsigned _concurrent_connections;
    ipv4_addr _server_addr;
    lowres_clock::time_point _earliest_started;
    lowres_clock::time_point _latest_finished;
    size_t _processed_bytes;
    unsigned _num_reported;
public:
    class connection {
        connected_socket _fd;
        output_stream<char> _write_buf;
        size_t _bytes_read = 0;
        size_t _bytes_write = 0;
    public:
        connection(connected_socket&& fd)
                : _fd(std::move(fd))
                , _write_buf(_fd.output()) {}

        future<> process() {
            // return when_all(read(), write()).discard_result();
            return write();
        }

        future<> write() {
            usleep(0);
            if(send_size!=0){
                memcpy(_send_packet, send_packet, send_size);
                send_size=0;
            }else {
                return write();
            }

            return _write_buf.write(_send_packet).then([this] {
                return _write_buf.flush();
            }).then([this] {
                std::cout << "WRITE AFTER::" << _send_packet << std::endl;
                return write();

            });
        }
    };


    future<> start(ipv4_addr server_addr, unsigned ncon) {
        _server_addr = server_addr;
        _concurrent_connections = ncon * smp::count;
        _total_pings = _pings_per_connection * _concurrent_connections;

        for (unsigned i = 0; i < ncon; i++) {
            socket_address local = socket_address(::sockaddr_in{AF_INET, INADDR_ANY, {0}});
            engine().net().connect(make_ipv4_address(server_addr), local, protocol).then([this] (connected_socket fd) {
                auto conn = new connection(std::move(fd));

                conn->process().then_wrapped([conn] (auto&& f) {
                    delete conn;
                    try {
                        f.get();
                    } catch (std::exception& ex) {
                        std::cout << "request error " << ex.what() << "\n";
                    }
                });




            });
        }
        return make_ready_future();
    }
    future<> stop() {
        return make_ready_future();
    }

};
