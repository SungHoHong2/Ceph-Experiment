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

int MAX_LOOP;
int POSIX;
int DPDK;

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

double intervals[99999];

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


int main( int argc, char **argv ){

    uint64_t start_time, end_time;
    FILE *file;
    char data[4089];
    int i;

    printf("CHECKING ARGUEMENTS\n");

    if(argc==0) return 0;

    if(argc>1){
        int i =0;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"TEN")==0){
                MAX_LOOP = 10;
            }
            if(strcmp(argv[i],"HUNDRED")==0){
                MAX_LOOP = 100;
            }
            if(strcmp(argv[i],"THOUSAND")==0){
                MAX_LOOP = 1000;
            }

            if(strcmp(argv[i],"POSIX")==0){
                POSIX = 1;
                printf("POSIX ENABLED\n");
            }
            if(strcmp(argv[i],"DPDK")==0){
                DPDK = 1;
                printf("DPDK ENABLED\n");
            }
        }
    }


    if(DPDK) file = fopen("/mnt/ssd_cache/data1/sungho/client.txt", "r");
    if(POSIX) file = fopen("/mnt/hdd/data1/sungho/client.txt", "r");

    for(i=0; i<MAX_LOOP; i++) {
        // usleep(1);
        start_time = getTimeStamp();
        if (file) {
            fread(data, sizeof(char), 4089, file);
        }
        end_time = getTimeStamp();
        intervals[i] = (double)end_time-start_time;
        printf("[%d] interval : %f\n", i, intervals[i]);
        printf("recv msg in FUSE: %s\n", data);
    }

    calculateSD(intervals);
    fclose(file);

    return  0;
}