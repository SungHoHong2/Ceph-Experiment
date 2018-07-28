#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

int main()
{

    int fd, nr, nr2, nw, nw2;
    char fl_nm[]={"/dev/nvme0n1p1"};
    char buf_wr[]={"FIRST TEXT INPUT\n"};
    char buf_wr2[]={"SECOND TEXT INPUT\n"};
    char buf_rd[strlen(buf_wr)];
    char buf_rd2[strlen(buf_wr2)];
    char buf_rd3[strlen(buf_wr)+strlen(buf_wr2)];

    char *test;

    char* str = NULL;
    void* ad = NULL;
    if (posix_memalign(&ad, 1024, 512)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }

    str = (char *)(ad);

    strcpy(str, "howdy howdy\n");
    printf("CHARA: %s\n", str);

    



    // str = new(ad) char[512];





    //open file
    fd = open(fl_nm, O_RDWR|O_CREAT, 0777);
    nw = pwrite(fd, &buf_wr, strlen(buf_wr), 0);

    //error checking
    if(fd == -1){
        perror("[error in open]\n");
    }
    else if(nw == -1){
        perror("[error in write]\n");
    }
    else{
        /*if open and write process are okey, read first write data
        * from file*/
        nr = pread(fd, &buf_rd, sizeof(buf_rd), 0);
        if(nr == -1){
            perror("[error in read]\n");
        } else{
            printf("%s\n", buf_rd);
        }

    }

    //second write process.
    nw2= pwrite(fd, &buf_wr2, strlen(buf_wr2), strlen(buf_wr));

    //write error checking
    if(nw2 == -1){
        perror("[error in write 2]\n");
    }else{
        /*if write process is correct
        * second read process*/
        nr2 = pread(fd, &buf_rd3, sizeof(buf_rd)+sizeof(buf_rd2), 0);
        printf("%s\n", buf_rd3);
    }


    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }

    return 0;
}