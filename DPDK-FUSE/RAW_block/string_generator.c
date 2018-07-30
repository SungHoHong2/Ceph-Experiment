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




void divideString(char *str, int n)
{
    int str_size = strlen(str);
    int i;
    int part_size;

// Check if string can be divided in
// n equal parts
    if (str_size % n != 0)
    {
        printf("Invalid Input: String size");
        printf(" is not divisible by n");
        return;
    }

// Calculate the size of parts to
// find the division points
    part_size = str_size / n;
    for (i = 0; i< str_size; i++)
    {
        if (i % part_size == 0)
            printf("\n");
        printf("%c", str[i]);
    }
}




int main()
{

    int fd, nr;
    char fl_nm[]={"/dev/nvme0n1p1"};

    char* aligned_buf_w = NULL;
    char* aligned_buf_r = NULL;

    void* ad = NULL;
    if (posix_memalign(&ad, SECTOR, BUF_SIZE * 4)) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }

    aligned_buf_w = (char *)(ad);


    ad = NULL;
    if (posix_memalign(&ad, SECTOR, 1024 * BUF_SIZE * 4 )) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);

    memset(aligned_buf_w, '*', BUF_SIZE * 4 * sizeof(char));

    printf("BEFORE READ BEGIN\n");
    printf("\t aligned_buf_w::%ld\n",strlen(aligned_buf_w));
    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));
    printf("BEFORE READ END\n");


    fd = open(fl_nm, O_RDWR | O_DIRECT);
//    pwrite(fd, aligned_buf_w, BUF_SIZE * 4, 0);


    pread(fd, aligned_buf_r, BUF_SIZE * 4, 0);
    printf("AFTER READ BEGIN\n");
    printf("\taligned_buf_r::%ld\n",strlen(aligned_buf_r));
    printf("AFTER READ END\n");


    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }


    printf("splitting string into 1024 bytes\n");

    int i;
    int end_size = 1024;

    char** pp;
    pp = malloc(4 * sizeof(char*));      // allocate the array to hold the pointer

    for(i=0; i<4;i++){
        pp[i] = malloc( sizeof(char) * 1024);
        memcpy(pp[i], aligned_buf_r, end_size);
        printf("%ld\n", strlen(pp[i]));
        aligned_buf_r+=1024;

    }

    for(i=0; i<4;i++) {
        free(pp[i]);
        aligned_buf_r-=1024;

    }

    aligned_buf_r = NULL;
    ad = NULL;
    if (posix_memalign(&ad, SECTOR, BUF_SIZE * 4 )) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);

    printf("BEFORE READ BEGIN\n");
    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));
    printf("BEFORE READ END\n");

    fd = open(fl_nm, O_RDWR | O_DIRECT);
    pread(fd, aligned_buf_r, BUF_SIZE * 4, 0);
    close(fd);

    printf("AFTER READ BEGIN\n");
    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));
    printf("AFTER READ END\n");


    for(i=0; i<4;i++){
        pp[i] = malloc( sizeof(char) * 1024);
        memcpy(pp[i], aligned_buf_r, end_size);
        printf("%ld\n", strlen(pp[i]));
        aligned_buf_r+=1024;

    }

    for(i=0; i<4;i++) {
        free(pp[i]);
        aligned_buf_r-=1024;

    }

    return 0;
}