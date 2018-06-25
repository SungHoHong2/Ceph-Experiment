
#include "core/reactor.hh"
#include "core/app-template.hh"
#include "core/temporary_buffer.hh"
#include "core/future-util.hh"
#include "core/distributed.hh"
#include <vector>
#include <iostream>
#include "server.hh"
#include "client.hh"

namespace bpo = boost::program_options;

int main(int ac, char** av) {
    std::cout << "MAIN BEGIN" << std::endl;

    app_template app;

    app.add_options()
            ("server", bpo::value<std::string>()->default_value("10.218.104.170:1234"), "Server address")
            ("test", bpo::value<std::string>()->default_value("ping"), "test type(ping | rxrx | txtx)")
            ("conn", bpo::value<unsigned>()->default_value(1), "nr connections per cpu")
            ("proto", bpo::value<std::string>()->default_value("tcp"), "transport protocol tcp|sctp")
            ("smp", bpo::value<unsigned>()->default_value(1), "smp");


    return app.run_deprecated(ac, av, [&app] {

        auto&& config = app.configuration();
        auto server = config["server"].as<std::string>();
        auto test = config["test"].as<std::string>();
        auto ncon = config["conn"].as<unsigned>();
        auto proto = config["proto"].as<std::string>();


        protocol = transport::TCP;
        clients.start().then([server, test, ncon] () {
            clients.invoke_on_all(&client::start, ipv4_addr{server}, test, ncon);
        });


        std::cout << "MAIN END" << std::endl;
    });



}