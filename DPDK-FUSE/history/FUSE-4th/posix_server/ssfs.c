#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <pthread.h>

#include "ssfs_common.h"
#include "ssfs_posix.h"


int main( int argc, char **argv )
{

    NOFILESYSTEM = 0;
    printf("CHECKING ARGUEMENTS\n");
    if(argc>1){
        int i;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"NOFILESYSTEM")==0){
                NOFILESYSTEM = 1;
            }
        }
    }

    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&tx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    printf("FUSE-POSIX-SERVER BEGIN");
    if(NOFILESYSTEM == 1) printf(" WITH NOFILESYSTEM");
    printf("\n");


    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, tcp_recv_launch, NULL);
        rc = pthread_create(&threads[1], NULL, tcp_send_launch, NULL);

    while(1){};
    return 0;
}