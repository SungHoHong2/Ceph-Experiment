#include <iostream>
#include <math.h>
#include <vector>


int main()
{
    std::vector<double> samples;
    samples.push_back(1.0);
    samples.push_back(2.0);
    samples.push_back(3.0);
    samples.push_back(4.0);
    samples.push_back(5.0);
    samples.push_back(6.0);
    samples.push_back(7.0);
    samples.push_back(8.0);
    samples.push_back(9.0);
    samples.push_back(10.0);

    int size = samples.size();
    double avg =0;
    for (int i = 0; i < size; i++)
    {
        avg += samples[i];
    }

    avg = avg/size;
    std::cout <<"avg: " << avg << std::endl;


    double variance = 0;
    double t = samples[0];
    for (int i = 1; i < size; i++)
    {
        t += samples[i];
        double diff = ((i + 1) * samples[i]) - t;
        variance += (diff * diff) / ((i + 1.0) *i);
    }

    double std_var = variance / (size - 1);
    double std_dev = sqrt(std_var);
    std::cout <<"std_dev: " << std_dev << std::endl;
    return 0;
}
