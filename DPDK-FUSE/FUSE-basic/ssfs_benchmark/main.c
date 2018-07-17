#include <stdio.h>
#include <unistd.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_REQUEST 100

int main (int arc, char * argv [])
{

    FILE *file;
    for(c=0; c<NUMBER_OF_REQUEST; c++){
        file = fopen("/mnt/ssd_cache/test/client", "r");
        if (file) {
            txe = malloc(sizeof(struct fuse_message));
            rtn = fread(txe->data, sizeof(char), 1024, file);
            printf("send msg from client: %s\n", txe->data);
            fclose(file);
        }

    }

    return EXIT_SUCCESS;
}