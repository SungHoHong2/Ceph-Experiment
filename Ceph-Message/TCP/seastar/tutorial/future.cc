#include "core/app-template.hh"
#include "core/reactor.hh"
#include "core/sleep.hh"
#include <iostream>
using namespace seastar;
using namespace std;

int main(int argc, char** argv) {
    app_template app;

    app.run(argc, argv, [] {
      cout << "sleeping" << endl;
          return sleep(1s).then([]{
            cout << "Done" << endl;
          });
    });
}
