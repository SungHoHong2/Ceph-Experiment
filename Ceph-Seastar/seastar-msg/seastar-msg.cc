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


void server_task(){}


int main(int ac, char** av) {
    std::cout << "OPTIMIZED SEASTAR BEGIN" << std::endl;

    char hostname[150];
    gethostname(hostname, 150);

    std::string arg_host;

    if(strcmp("wenji-w1",hostname)==0){
        arg_host = "10.218.104.170:1234";


    }else if(strcmp("w2",hostname)==0){
        arg_host = "10.218.111.252:1234";


    }else if(strcmp("c3n24",hostname)==0){
        arg_host = "10.107.30.41:1234";


    }else if(strcmp("c3n25",hostname)==0){
        arg_host = "10.107.30.40:1234";


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



}

const std::map<std::string, client::test_fn> client::tests = {
        {"ping", &client::ping_test},
};
