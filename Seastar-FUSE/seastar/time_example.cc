#include <iostream>
#include <chrono>
#include <ratio>
#include <thread>

auto t1, t2;


void f()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
    t1 = std::chrono::high_resolution_clock::now();
    f();
    t2 = std::chrono::high_resolution_clock::now();

    // floating-point duration: no duration_cast needed
    std::chrono::duration<double, std::milli> fp_ms = t2 - t1;

    // integral duration: requires duration_cast
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // converting integral duration to integral duration of shorter divisible time unit:
    // no duration_cast needed
    std::chrono::duration<long, std::micro> int_usec = int_ms;

    std::cout << "f() took " << fp_ms.count() << " ms, "
              << "or " << int_ms.count() << " whole milliseconds "
              << "(which is " << int_usec.count() << " whole microseconds)" << std::endl;
}