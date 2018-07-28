#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(){

    struct stat fstat;
    stat(filepath, &fstat);
    int blksize = (int)fstat.st_blksize;
    int align = blksize-1;

    const char *buff = (char *) malloc((int)blksize+align);
    buff = (char *)(((uintptr_t)buff+align)&~((uintptr_t)align));




    if(write(fd,buff,(size_t)blksize)<1) {
        //Error handling
        free((char *)buff);
        return -1;
    }



    return 0;
}