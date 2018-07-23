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

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}






int main(){

    uint64_t start_time, end_time;
    FILE *file;
    char data[1024];
    int i;

    for(i=0; i<10; i++) {
        start_time = getTimeStamp();
        file = fopen("/mnt/ssd_cache/home/sungho/client.txt", "r");
        if (file) {
            fread(data, sizeof(char), 22, file);
            printf("recv msg in FUSE: %s\n", data);
            fclose(file);
        }
        end_time = getTimeStamp();
        printf("interval : %ld\n", end_time-start_time);
    }

    return  0;
}