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


int main(){

    int c;
    FILE *file;
    char data[24];
    file = fopen("/mnt/ssd_cache/server", "r");
    if (file) {
        fread(data, sizeof(char), 24, file);
        printf("send msg in FUSE: %s\n", data);
        fclose(file);
    }

    return  0;
}