#include <stdio.h>
#include <unistd.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_REQUEST 10

int main (int arc, char * argv [])
{

    int c;
    char read_file[1024];
    FILE *file;
    for(c=0; c<NUMBER_OF_REQUEST; c++){
        file = fopen("/mnt/ssd_cache/test/client", "r");
        if (file) {
            rtn = fread(read_file, sizeof(char), 1024, file);
            printf("send msg from client: %s\n", read_file);
            fclose(file);
        }

    }

    return EXIT_SUCCESS;
}