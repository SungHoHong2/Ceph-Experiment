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

#define SECTOR 512
#define PKT_SIZE 1024
#define MERGE_PACKETS 4
#define LOOPS 1000



int main(){


    int fd, nr;
    char* aligned_buf_r = NULL;
    void* ad = NULL;

    if (posix_memalign(&ad, SECTOR, PKT_SIZE * MERGE_PACKETS )) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }

    aligned_buf_r = (char *)(ad);
    fd = open("/dev/nvme0n1p1", O_RDWR | O_DIRECT);
    nr = pread(fd, aligned_buf_r, PKT_SIZE * MERGE_PACKETS, 0);
    close(fd);

    printf("%s\n",aligned_buf_r);


    return 0;

}