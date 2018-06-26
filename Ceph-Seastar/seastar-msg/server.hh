using namespace seastar;

static std::string str_ping{"ping"};
static std::string str_pong{"pong"};
static int tx_msg_size = 4 * 1024;
static std::string str_txbuf(tx_msg_size, 'X');
static bool enable_tcp = false;
static bool enable_sctp = false;

class tcp_server {
    std::vector<server_socket> _tcp_listeners;
    std::vector<server_socket> _sctp_listeners;
public:
    future<> listen(ipv4_addr addr) {
        if (enable_tcp) {
            listen_options lo;
            lo.proto = transport::TCP;
            lo.reuse_address = true;
            _tcp_listeners.push_back(engine().listen(make_ipv4_address(addr), lo));
            do_accepts(_tcp_listeners);
        }

        if (enable_sctp) {
            listen_options lo;
            lo.proto = transport::SCTP;
            lo.reuse_address = true;
            _sctp_listeners.push_back(engine().listen(make_ipv4_address(addr), lo));
            do_accepts(_sctp_listeners);
        }
        return make_ready_future<>();
    }

    // FIXME: We should properly tear down the service here.
    future<> stop() {
        return make_ready_future<>();
    }

    void do_accepts(std::vector<server_socket>& listeners) {
        int which = listeners.size() - 1;
        listeners[which].accept().then([this, &listeners] (connected_socket fd, socket_address addr) mutable {
            auto conn = new connection(*this, std::move(fd), addr);
            conn->process().then_wrapped([conn] (auto&& f) {
                delete conn;
                try {
                    f.get();
                } catch (std::exception& ex) {
                    std::cout << "request error " << ex.what() << "\n";
                }
            });
            do_accepts(listeners);
        }).then_wrapped([] (auto&& f) {
            try {
                f.get();
            } catch (std::exception& ex) {
                std::cout << "accept failed: " << ex.what() << "\n";
            }
        });
    }
    class connection {
        connected_socket _fd;
        input_stream<char> _read_buf;
    public:
        connection(tcp_server& server, connected_socket&& fd, socket_address addr)
                : _fd(std::move(fd))
                , _read_buf(_fd.input()){}
        future<> process() {
            return read();
        }
        future<> read() {
            sleep(1);
            if (_read_buf.eof()) {
                return make_ready_future();
            }

            return _read_buf.read_exactly(5).then([this] (temporary_buffer<char> buf) {

                if(buf.size()>2) {
                    memset(recv_packet,0, PACKET_SIZE);
                    memcpy(recv_packet, buf.get(),  buf.size());
                    std::cout << "LISTEN::" << recv_packet << "  " << buf.size() <<  std::endl;
                }
                return this->read();
            });
        }

    };
};
