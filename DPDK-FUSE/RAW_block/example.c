#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <string.h>



int main(){

    mtd_info_t mtd_info;           // the MTD structure
    erase_info_t ei;               // the erase block structure
    int i;


    char data[4096];
    strcpy(data,"GENERATE WRITING DATA\n");

    memset(data, '*', 4096*sizeof(char));

    unsigned char read_buf[4096] = {0x00};


    int fd = open("/dev/nvme0n1p1", O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU);
    // int fd = open("/dev/nvme0n1p1", O_RDWR); // get the device info
    // int fd = open("/dev/nvme0n1p1", O_RDWR | O_DIRECT); // get the device info

//    ei.length = mtd_info.erasesize;   //set the erase block size
//    for(ei.start = 0; ei.start < mtd_info.size; ei.start += ei.length)
//    {
//        ioctl(fd, MEMUNLOCK, &ei);
//        ioctl(fd, MEMERASE, &ei);
//    }

    lseek(fd, 0, SEEK_SET);
    read(fd, read_buf, sizeof(read_buf));
    printf("read data: %ld\n", strlen(read_buf));


    // sanity check
    //    for(i = 0; i<20; i++)
    //        printf("buf[%d] = 0x%02x\n", i, (unsigned int)read_buf[i]);


//    lseek(fd, 0, SEEK_SET);        // go back to first block's start
//    int val = write(fd, data, sizeof(data)); // write our message
//    printf("write data: %ld :: %d \n", strlen(data), val);

//    lseek(fd, 0, SEEK_SET);              // go back to first block's start
//    read(fd, read_buf, sizeof(read_buf));// read the data
//    printf("read data: %ld\n", strlen(read_buf));

    // sanity check, now you see the message we wrote!
    //    for(i = 0; i<20; i++)
    //        printf("buf[%d] = 0x%02x\n", i, (unsigned int)read_buf[i]);


    close(fd);
    return 0;
}