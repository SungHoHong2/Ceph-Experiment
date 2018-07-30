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
    if (posix_memalign(&ad, SECTOR, BUF_SIZE * 4 )) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);

    memset(aligned_buf_w, '*', BUF_SIZE * 4 * sizeof(char));

    printf("BEFORE READ BEGIN\n");
    printf("\t aligned_buf_w::%ld\n",strlen(aligned_buf_w));
    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));
    printf("BEFORE READ END\n");


    fd = open(fl_nm, O_RDWR | O_DIRECT);
    pwrite(fd, aligned_buf_w, BUF_SIZE * 4, 0);


    pread(fd, aligned_buf_r, BUF_SIZE * 4, 0);
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

    char array_test[4][1024];
    int i;
    int end_size = 1024;


    char *p1 = malloc( sizeof(char) * 1024);
    char *p2 = malloc( sizeof(char) * 1024);
    char *p3 = malloc( sizeof(char) * 1024);
    char *p4 = malloc( sizeof(char) * 1024);

    memcpy(p1, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p1));
    aligned_buf_r+=1024;

    memcpy(p2, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p2));
    aligned_buf_r+=1024;

    memcpy(p3, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p3));
    aligned_buf_r+=1024;

    memcpy(p4, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p4));
    aligned_buf_r+=1024;

    free(p1);
    free(p2);
    free(p3);
    free(p4);
    aligned_buf_r-=4096;


//    for(i=0; i<4; i++){
//        memcpy(array_test[i], aligned_buf_r, end_size);
//        printf("%ld\n", strlen(array_test[i]));
//        aligned_buf_r+=1024;
//    }



    aligned_buf_r = NULL;
    ad = NULL;
    if (posix_memalign(&ad, SECTOR, BUF_SIZE * 4 )) {
        perror("posix_memalign failed"); exit (EXIT_FAILURE);
    }
    aligned_buf_r = (char *)(ad);


    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));
    fd = open(fl_nm, O_RDWR | O_DIRECT);
    pread(fd, aligned_buf_r, BUF_SIZE * 4, 0);
    close(fd);
    printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));


//    for(i=0; i<4; i++){
//        memcpy(array_test[i], aligned_buf_r, end_size);
//        printf("%ld\n", strlen(array_test[i]));
//        aligned_buf_r+=1024;
//    }


    p1 = malloc( sizeof(char) * 1024);
    p2 = malloc( sizeof(char) * 1024);
    p3 = malloc( sizeof(char) * 1024);
    p4 = malloc( sizeof(char) * 1024);

    memcpy(p1, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p1));
    aligned_buf_r+=1024;

    memcpy(p2, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p2));
    aligned_buf_r+=1024;

    memcpy(p3, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p3));
    aligned_buf_r+=1024;

    memcpy(p4, aligned_buf_r, end_size);
    printf("%ld\n", strlen(p4));
    aligned_buf_r+=1024;



//
//    aligned_buf_r-=4096;
//    end_size-=4096;
//
//
//    for(i=0; i<4; i++){
//        memcpy(array_test[i], aligned_buf_r, end_size);
//        printf("%ld\n", strlen(array_test[i]));
//        aligned_buf_r+=1024;
//        end_size+=1024;
//
//    }


    return 0;
}