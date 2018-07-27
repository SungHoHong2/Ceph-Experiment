#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#define MAX_LOOP 10

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

double intervals[MAX_LOOP];

void calculateSD(double data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i=0; i<MAX_LOOP; ++i)
    {
        sum += data[i];
    }

    mean = sum/MAX_LOOP;
    printf("mean: %f\n",mean);


    for(i=0; i<MAX_LOOP; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    printf("std: %f\n",sqrt(standardDeviation/MAX_LOOP));
}


// POSIX run the benchmark without the sudo
// DPDK run the benchmark with the sudo


int main(){

    uint64_t start_time, end_time;
    FILE *file;
    char data[1024];
    int i;

    for(i=0; i<MAX_LOOP; i++) {
        sleep(1);
        start_time = getTimeStamp();
        file = fopen("/data1/sungho/trash/data1/sungho/client.txt", "r");
        // file = fopen("/mnt/ssd_cache/home/sungho/client.txt", "r");
        if (file) {
            fread(data, sizeof(char), 23, file);
            printf("recv msg in FUSE: %s\n", data);
            fclose(file);
        }
        end_time = getTimeStamp();
        intervals[i] = (double)end_time-start_time;
        printf("[%d] interval : %f\n", i, intervals[i]);
    }

    calculateSD(intervals);
    return  0;
}