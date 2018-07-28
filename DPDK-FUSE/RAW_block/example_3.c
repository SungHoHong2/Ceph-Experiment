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

    int fd, nr;
    char fl_nm[]={"/dev/nvme0n1p1"};

    char* aligned_buf_w = NULL;
    char* aligned_buf_r = NULL;

    void* ad = NULL;
    if (posix_memalign(&ad, 32, 4089)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }

    aligned_buf_w = (char *)(ad);


    ad = NULL;
    if (posix_memalign(&ad, 32, 4089)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);


    strcpy(aligned_buf_w, "BEFORE TEXT READ");
    strcpy(aligned_buf_r, "XXXXXXXXXXXXXXXX");


    printf("BEFORE READ BEGIN\n");
    printf("\t aligned_buf_w::%s\n",aligned_buf_w);
    printf("\t aligned_buf_r::%s\n",aligned_buf_r);
    printf("BEFORE READ END\n");


    fd = open(fl_nm, O_RDWR|O_CREAT);
    pwrite(fd, aligned_buf_w, 4089, 0);

    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }

//    fd = open(fl_nm, O_RDWR|O_CREAT, 0777);
    fd = open(fl_nm, O_RDONLY|O_DIRECT);
    nr = pread(fd, aligned_buf_r, 4089, 0);

    printf("AFTER READ BEGIN\n");
    printf("\taligned_buf_r::%s\n",aligned_buf_r);
    printf("AFTER READ END\n");

    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }

    return 0;
}