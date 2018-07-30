#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>

#define SECTOR 512
#define BS 4096

int max_loop = 1000;

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

    for(i=0; i<max_loop; ++i)
    {
        sum += data[i];
    }

    mean = sum/max_loop;
    printf("mean: %f\n",mean);


    for(i=0; i<max_loop; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    printf("std: %f\n",sqrt(standardDeviation/max_loop));
}


int main( int argc, char **argv ){

    uint64_t start_time, end_time;
    int file;
    int i;
    void *buf;
    posix_memalign(&buf, SECTOR, BS);

    unsigned long long offset;
    int ret;
    for(i=0; i<max_loop; i++) {
        start_time = getTimeStamp();
        file = open("/dev/nvme0n1p1", O_RDWR | O_DIRECT);
        if (file < 0) {
            printf("Open error\n");
            return -1;
        }
        offset = ((rand() % (1024 *1024 *1024)) / SECTOR) * SECTOR;
        // printf("offset = %llu", offset);
        if (file) {
            ret = pread(file, buf, BS, offset);
            if (ret < 0 || ret == 0) {
                printf("Read error %d\n", ret);
                return 0;
            }
            printf("recv msg in offset: %llu\n", offset);
            close(file);
        }
        end_time = getTimeStamp();
        intervals[i] = (double)end_time-start_time;
        //printf("[%d] interval : %f\n", i, intervals[i]);
    }

    calculateSD(intervals);
    return  0;
}

