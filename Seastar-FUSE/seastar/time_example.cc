#include <iostream>
#include <chrono>
using namespace std;

int main()
{
    cout << chrono::high_resolution_clock::period::den << endl;

    auto start_time = chrono::high_resolution_clock::now();
    int temp;
    for (int i = 0; i< 242000000; i++)
        temp+=temp;
    auto end_time = chrono::high_resolution_clock::now();
    cout << "sec::" << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << endl;
    cout << "microsec::" << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << endl;
    return 0;
}
