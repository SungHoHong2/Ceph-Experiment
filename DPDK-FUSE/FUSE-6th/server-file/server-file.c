#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <sys/queue.h>
#include <sys/time.h>


#define SECTOR 512
#define PKT_SIZE 1024
#define MERGE_PACKETS 4
#define LOOPS 1000
double intervals[99999];

void calculateSD(double data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;
    int i;
    for(i=0; i<LOOPS; ++i)
    {
        sum += data[i];
    }

    mean = sum/LOOPS;
    printf("mean: %f\n",mean);


    for(i=0; i<LOOPS; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    printf("std: %f\n",sqrt(standardDeviation/LOOPS));
}

struct avg_node
{
    uint64_t num;
    uint64_t start_time;
    uint64_t end_time;
    uint64_t interval;
    TAILQ_ENTRY(avg_node) nodes;
};

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}




int main(){

    int fd, nr, test_i=0;
    char* aligned_buf_r = NULL;
    void* ad = NULL;
    struct avg_node *av = NULL;
    char* aggregated = malloc(MERGE_PACKETS* PKT_SIZE * sizeof(char));

    TAILQ_HEAD(avg_head, avg_node) avg_queue;
    TAILQ_INIT(&avg_queue);


    if (posix_memalign(&ad, SECTOR, PKT_SIZE * MERGE_PACKETS)) {
        perror("posix_memalign failed");
        exit(EXIT_FAILURE);
    }

    aligned_buf_r = (char *) (ad);

    for(int i =0; i<LOOPS; i++) {
        sleep(1);
        av = malloc(sizeof(struct avg_node));
        av->start_time = getTimeStamp();
        TAILQ_INSERT_TAIL(&avg_queue, av, nodes);

        fd = open("/dev/nvme0n1p1", O_RDWR | O_DIRECT);
        nr = pread(fd, aligned_buf_r, PKT_SIZE * MERGE_PACKETS, 0);
        close(fd);

        strcpy(aggregated, aligned_buf_r);
        av = TAILQ_FIRST(&avg_queue);
        av->end_time = getTimeStamp();
        av->num = test_i;
        av->interval = av->end_time - av->start_time;
        intervals[test_i] = (double) av->interval;
        TAILQ_REMOVE(&avg_queue, av, nodes);
        test_i++;
        printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(aggregated), av->interval);
    }

    calculateSD(intervals);

    return 0;

}