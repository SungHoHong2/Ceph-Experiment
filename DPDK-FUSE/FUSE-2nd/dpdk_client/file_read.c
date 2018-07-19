#include <fuse.h>
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

    file = fopen("/mnt/ssd_cache/test/server", "r");
    if (file) {
        pthread_mutex_lock(&tx_lock);
        txe = malloc(sizeof(struct fuse_message));
        rtn = fread(txe->data, sizeof(char), 1024, file);
        // printf("send msg in FUSE: %s\n", txe->data);
        TAILQ_INSERT_TAIL(&fuse_tx_queue, txe, nodes);
        pthread_mutex_unlock(&tx_lock);
        fclose(file);
    }


    return  0;
}