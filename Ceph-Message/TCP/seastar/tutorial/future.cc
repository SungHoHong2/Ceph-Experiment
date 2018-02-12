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

future<> f() {
    cout << "Sleeping... " << std::flush;
    sleep(200ms).then([] { cout << "200ms " << endl; });
    sleep(100ms).then([] { cout << "100ms " << endl; });
    return sleep(1s).then([] {
      cout << "Done.\n";
    });
}
