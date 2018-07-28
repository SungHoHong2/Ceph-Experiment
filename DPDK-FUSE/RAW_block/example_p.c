#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{

    int fd, nr, nr2, nw, nw2;
    char fl_nm[]={"/dev/nvme0n1p1"};
    char buf_wr[]={"hello everyone this is first text\n"};
    char buf_wr2[]={"Did you miss me? Don't afraid\n"};
    char buf_rd[120];
    char buf_rd2[120];

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

        printf("length of the buf_wr %d\n", strlen(buf_wr));
        /*if open and write process are okey, read first write data
        * from file*/
        nr = pread(fd, &buf_rd, sizeof(buf_rd), 0);

        //display succeeded message about first write and open process
        printf("[file is opened]\n");
        printf("[succeeded write(1) process]\n");

        //read process error control
        if(nr == -1){
            perror("[error in read]\n");
        } else{
            printf("[reading(1) data] from %s\n", fl_nm);
            printf("[%s]\n", buf_rd);
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
        nr2 = pread(fd, &buf_rd2, sizeof(buf_rd), 0);

        printf("-----------------------------------\n");
        printf("[succeeded write(2) process]\n");
        printf("[reading(2) data] from %s\n", fl_nm);
        printf("[%s]\n", buf_rd2);
    }


    //error checking for close process
    if(close(fd) == -1){
        perror("[error in close]\n");
    }else{
        printf("[succeeded in close]\n");
    }

    return 0;
}