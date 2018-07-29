#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#define BUF_SIZE 4096
struct message {
    char data[PKT_SIZE];
};
int main()
{

    int fd, nr;
    char fl_nm[]={"/dev/nvme0n1p1"};

    char* aligned_buf_w = NULL;
    char* aligned_buf_r = NULL;

    void* ad = NULL;
    if (posix_memalign(&ad, 32, BUF_SIZE)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }

    aligned_buf_w = (char *)(ad);


    ad = NULL;
    if (posix_memalign(&ad, 32, BUF_SIZE)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);

    memset(aligned_buf_w, '*', BUF_SIZE * sizeof(char));

//    printf("BEFORE READ BEGIN\n");
//    printf("\t aligned_buf_w::%s\n",aligned_buf_w);
//    printf("\t aligned_buf_r::%s\n",aligned_buf_r);
//    printf("BEFORE READ END\n");


    fd = open(fl_nm, O_RDWR|O_CREAT);
    // fd = open(fl_nm, O_RDWR|O_CREAT|O_DIRECT);
    pwrite(fd, aligned_buf_w, BUF_SIZE, 0);


    pread(fd, aligned_buf_r, BUF_SIZE, 0);
//    printf("AFTER READ BEGIN\n");
    printf("\taligned_buf_r::%ld\n",strlen(aligned_buf_r));
//    printf("AFTER READ END\n");


    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }


    printf("splitting string into 1024 bytes\n");


    char array_test[3][1024];

    int i;
    for(i=0; i<3; i++){
        memcpy(array_test[i], aligned_buf_r, 1024);
        printf("%s\n", array_test[i]);
        aligned_buf_r+=1024;
    }

    



    return 0;
}