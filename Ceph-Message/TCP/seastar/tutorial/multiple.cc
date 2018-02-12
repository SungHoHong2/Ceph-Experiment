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

future<int> incr(int i) {
    return sleep(10ms).then([i] { return i + 1; });
}

future<> f() {
    return incr(3).then([] (int val) {
        cout << "Got " << val << "\n";
    });
}
