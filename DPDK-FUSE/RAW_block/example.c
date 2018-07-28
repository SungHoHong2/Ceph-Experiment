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


    char data[20];
    strcpy(data,"howdyhowdy\n");


    unsigned char read_buf[20] = {0x00};



    int fd = open("/mnt/ssd_cache", O_RDWR | O_DIRECT); // get the device info
    printf("MTD Type: %x\nMTD total size: %x bytes\nMTD erase size: %x bytes\n",
           mtd_info.type, mtd_info.size, mtd_info.erasesize);


    ei.length = mtd_info.erasesize;   //set the erase block size

    for(ei.start = 0; ei.start < mtd_info.size; ei.start += ei.length)
    {
        ioctl(fd, MEMUNLOCK, &ei);
        // printf("Eraseing Block %#x\n", ei.start); // show the blocks erasing
        // warning, this prints a lot!
        ioctl(fd, MEMERASE, &ei);
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, read_buf, sizeof(read_buf));

    // sanity check, should be all 0xFF if erase worked
    for(i = 0; i<20; i++)
        printf("buf[%d] = 0x%02x\n", i, (unsigned int)read_buf[i]);


    printf("write data\n");
    lseek(fd, 0, SEEK_SET);        // go back to first block's start
    write(fd, data, sizeof(data)); // write our message


    printf("read data\n");
    lseek(fd, 0, SEEK_SET);              // go back to first block's start
    read(fd, read_buf, sizeof(read_buf));// read the data


    // sanity check, now you see the message we wrote!
    for(i = 0; i<20; i++)
        printf("buf[%d] = 0x%02x\n", i, (unsigned int)read_buf[i]);


    close(fd);
    return 0;
}