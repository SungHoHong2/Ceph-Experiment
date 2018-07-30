#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#define BUF_SIZE 1024
#define SECTOR 512
#define BS 4096



int main( int argc, char **argv ){

    int file;
    int i;
    void *buf;
    posix_memalign(&buf, SECTOR, BS);

    unsigned long long offset;
    int ret;

    for(int i=0; i< 10; i ++) {
        file = open("/data1/sungho/trash/one_gig_example", O_RDWR | O_DIRECT);
        if (file < 0) {
            printf("Open error\n");
            return -1;
        }
        offset = ((rand() % (1024 * 1024 * 1024)) / SECTOR) * SECTOR;
        if (file) {
            ret = pread(file, buf, BS, offset);
            if (ret < 0 || ret == 0) {
                printf("Read error %d\n", ret);
                return 0;
            }
            printf("recv msg in offset: %llu\n", offset);
            close(file);
        }
    }

    return  0;
}

