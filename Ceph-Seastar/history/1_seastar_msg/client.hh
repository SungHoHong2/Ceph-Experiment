using namespace seastar;
using namespace net;
using namespace std::chrono_literals;

class client;
distributed<client> clients;
transport protocol = transport::TCP;


class client {
private:
    static constexpr unsigned _pings_per_connection = 10000;
    unsigned _total_pings;
    unsigned _concurrent_connections;
    ipv4_addr _server_addr;
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

        future<> process() {
             return when_all(read(), write()).discard_result();
        }

        future<> read() {
            usleep(0);
            if (_read_buf.eof()) {
                return make_ready_future();
            }

            return _read_buf.read_exactly(5).then([this] (temporary_buffer<char> buf) {
                memset(recv_packet,0, PACKET_SIZE);
                memcpy(recv_packet, buf.get(),  buf.size());

                if(strcmp("^^^^^", recv_packet) != 0)
                std::cout << "LISTEN::" << recv_packet << "  " << buf.size() <<  std::endl;

                return this->read();
            });
        }

        future<> write() {
            usleep(0);
            if(send_size!=0){
                memcpy(_send_packet, send_packet, send_size);
                send_size=0;
            }else {
                // return write();
                memset(_send_packet,"^", 5);
            }

            return _write_buf.write(_send_packet).then([this] {
                return _write_buf.flush();
            }).then([this] {
                // std::cout << "WRITE AFTER::" << _send_packet << std::endl;
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
