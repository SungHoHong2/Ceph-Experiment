#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>
using namespace seastar;
using namespace std;

int main(int argc, char** argv) {
    app_template app;

    try{
    app.run(argc, argv, [] {
            cout << "Hello world  " << smp::count <<  endl;  // prints out the total number of running threads
            return make_ready_future<>();
    });
  } catch(runtime_error &e){
        cerr << "Couldn't start application: " << e.what() << "\n";
        return 1;
  }

}
