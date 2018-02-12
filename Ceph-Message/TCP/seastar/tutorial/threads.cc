#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>
using namespace seastar;
using namespace std;

int main(int argc, char** argv) {
    app_template app;
    app.run(argc, argv, [] {
            cout << "Hello world  " << smp::count <<  endl;  // printout
            return make_ready_future<>();
    });
}
