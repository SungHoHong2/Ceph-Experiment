#include "core/reactor.hh"
#include "core/app-template.hh"
#include "core/temporary_buffer.hh"
#include "core/distributed.hh"
#include <vector>
#include <iostream>
#include "server.hh"

namespace bpo = boost::program_options;

int main(int ac, char** av) {
    app_template app;
    app.add_options()
            ("port", bpo::value<uint16_t>()->default_value(1234), "TCP server port") // add port
            ("tcp", bpo::value<std::string>()->default_value("yes"), "tcp listen") // add tcp
            ("sctp", bpo::value<std::string>()->default_value("no"), "sctp listen"); // enable sctp


    return app.run_deprecated(ac, av, [&] {
        auto&& config = app.configuration();
        uint16_t port = config["port"].as<uint16_t>(); // assign the port value from the app_template
        enable_tcp = config["tcp"].as<std::string>() == "yes"; // assign the tcp key from the app_template
        enable_sctp = config["sctp"].as<std::string>() == "yes"; // assign the sctp key from the app_template
        if (!enable_tcp && !enable_sctp) {
            fprint(std::cerr, "Error: no protocols enabled. Use \"--tcp yes\" and/or \"--sctp yes\" to enable\n");
            return engine().exit(1);
        } // if there is no specific value then return false


        auto server = new distributed<tcp_server>; // run distributed object
        // The distributed template manages a sharded service,
        // by creating a copy of the service on each shard, providing mechanisms
        // to communicate with each shard's copy, and a way to stop the service.


        //Starts Service by constructing an instance on every logical core with a copy of args passed to the constructor.
        server->start().then([server = std::move(server), port] () mutable {
            engine().at_exit([server] {
                return server->stop();
            });
            server->invoke_on_all(&tcp_server::listen, ipv4_addr{port});
            // Invoke a method on all Service instances in parallel.
        }).then([port] {
            std::cout << "Seastar TCP server listening on port " << port << " ...\n";
        });
    });
}