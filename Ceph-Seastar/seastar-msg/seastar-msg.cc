#include "core/reactor.hh"
#include "core/app-template.hh"
#include "core/temporary_buffer.hh"
#include "core/future-util.hh"
#include "core/distributed.hh"
#include "core/sleep.hh"
#include <vector>
#include <iostream>
#include <thread>

#include "common.hh"
#include "server.hh"
#include "client.hh"

namespace bpo = boost::program_options;

int main(int ac, char** av) {
    std::cout << "MAIN BEGIN" << std::endl;


    char hostname[150];
    gethostname(hostname, 150);

    std::string arg_host;

    if(strcmp("wenji-w1",hostname)==0){
        arg_host = "10.218.104.170:1234";


    }else if(strcmp("w2",hostname)==0){
        arg_host = "10.218.111.252:1234";


    } else {
        std::cout << "wrong host" << std::endl;
        return 0;
    }


    app_template app;
    app.add_options()
            ("server", bpo::value<std::string>()->default_value(arg_host), "Server address")
            ("port", bpo::value<uint16_t>()->default_value(1234), "TCP server port")
            ("test", bpo::value<std::string>()->default_value("ping"), "test type(ping | rxrx | txtx)")
            ("conn", bpo::value<unsigned>()->default_value(1), "nr connections per cpu")
            ("proto", bpo::value<std::string>()->default_value("tcp"), "transport protocol tcp|sctp")
            ("smp", bpo::value<unsigned>()->default_value(1), "smp");


    std::thread t1(task1, "Hello");


    return app.run_deprecated(ac, av, [&app] {
        auto&& config = app.configuration();
        auto conn_server = config["server"].as<std::string>();
        auto test = config["test"].as<std::string>();
        auto ncon = config["conn"].as<unsigned>();
        auto proto = config["proto"].as<std::string>();
        uint16_t port = config["port"].as<uint16_t>();

        protocol = transport::TCP;
        enable_tcp = 1;

        if (!client::tests.count(test)) {
            return engine().exit(1);
        }


        auto server = new distributed<tcp_server>; // run distributed object
        server->start().then([server = std::move(server), port] () mutable {
            engine().at_exit([server] {
                return server->stop();
            });
            server->invoke_on_all(&tcp_server::listen, ipv4_addr{port});
            // Invoke a method on all Service instances in parallel.
        }).then([port] {
            std::cout << "Seastar TCP server listening on port " << port << " ...\n";
        });


        using namespace std::chrono_literals;
        sleep(10s).then([conn_server, test, ncon] {
            clients.start().then([conn_server, test, ncon] () {
                clients.invoke_on_all(&client::start, ipv4_addr{conn_server}, test, ncon);



            });
        });

        std::cout << "MAIN END" << std::endl;
    });

}

const std::map<std::string, client::test_fn> client::tests = {
        {"ping", &client::ping_test},
};