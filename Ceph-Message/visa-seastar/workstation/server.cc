#include "core/reactor.hh"
#include "core/app-template.hh"
#include "core/temporary_buffer.hh"
#include "core/distributed.hh"
#include <vector>
#include <iostream>

using namespace seastar;

static std::string str_ping{"ping"};
static std::string str_pong{"pong"};

class tcp_server {
    std::vector<server_socket> _tcp_listeners;

public:
    future<> listen(ipv4_addr addr) { // listens to ip address
        listen_options lo;
        lo.proto = transport::TCP;
        lo.reuse_address = true;
        _tcp_listeners.push_back(engine().listen(make_ipv4_address(addr), lo)); // add the client address to the global list
        do_accepts(_tcp_listeners); // accept the client
        return make_ready_future<>(); // activate the function
    }

    future<> stop() {
        return make_ready_future<>();
    }

    void do_accepts(std::vector<server_socket>& listeners) {
        int which = listeners.size() - 1; // identify the *th client
        listeners[which].accept().then([this, &listeners] (connected_socket fd, socket_address addr) mutable {
            auto conn = new connection(*this, std::move(fd), addr); // create a connection for the client

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
        output_stream<char> _write_buf;
    public:
        connection(tcp_server& server, connected_socket&& fd, socket_address addr)
            : _fd(std::move(fd))
            , _read_buf(_fd.input())  //get the input buffer from the socket
            , _write_buf(_fd.output()) {} // get the output buffer from the socket
        future<> process() {
             return read(); // process function directs to the read function
        }
        future<> read() {
            if (_read_buf.eof()) { // if there is nothing to read in the  buffer end the read function
                return make_ready_future(); // end function
            }

            size_t n = 4; // Expect 4 bytes cmd from client
            return _read_buf.read_exactly(n).then([this] (temporary_buffer<char> buf) {
                if (buf.size() == 0) { // if there is nothing to read in the  buffer end the read function
                    return make_ready_future(); // end function
                }
                auto cmd = std::string(buf.get(), buf.size()); // get the string ping
                // pingpong test
                if (cmd == str_ping) { // validate the received data string "ping"
                    // this is where you get the string ping!
                    // std::cout << str_ping << std::endl;
                    return _write_buf.write(str_pong).then([this] { // write the "pong" to the output buffer
                        return _write_buf.flush(); // send it back to the socket
                    }).then([this] {
                        return this->read(); // rerun the read function
                    });
                }
            });
        }
    };
};

namespace bpo = boost::program_options;

int main(int ac, char** av) {
    app_template app; // run the seastar engine
    return app.run_deprecated(ac, av, [&] {
        uint16_t port = 1234; // assign the port value from the app_template
        auto server = new distributed<tcp_server>; // run distributed object
        // The distributed template manages a sharded service,
        // by creating a copy of the service on each shard, providing mechanisms
        // to communicate with each shard's copy, and a way to stop the service.

        // Starts Service by constructing an instance on every logical core with a copy of args passed to the constructor.
        // The server will run on a loop until the engine receives the exit signal
        server->start().then([server = std::move(server), port] () mutable {
            engine().at_exit([server] { // if the server got the termination signal
                return server->stop(); // finish the server when the server is terminated
                // until the stop function the server will run infinitely
            });
            server->invoke_on_all(&tcp_server::listen, ipv4_addr{port}); // run the listen function
            // Invoke a method on all Service instances in parallel.
        }).then([port] {
            std::cout << "Seastar TCP server listening on port " << port << " ...\n"; // prompt user after running the listen function
        });
    });
}
