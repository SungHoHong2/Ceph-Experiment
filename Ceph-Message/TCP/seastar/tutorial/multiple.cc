#include "core/app-template.hh"
#include "core/reactor.hh"
#include "core/sleep.hh"
#include <iostream>
using namespace seastar;
using namespace std;

extern future<> f();


int main(int argc, char** argv) {
    app_template app;
    try {
        app.run(argc, argv, f);
    } catch(std::runtime_error &e) {
        cerr << "Couldn't start application: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

future<int> fast() {
    return make_ready_future<int>(3);
}


future<int> slow() {
    return sleep(100ms).then([] { return 3; });
}

future<> f() {
    slow().then([] (int val) {
        std::cout << "Slow: " << val << "\n";
    });

    return fast().then([] (int val) {
        cout << "Fast: " << val << "\n";
    });
}
