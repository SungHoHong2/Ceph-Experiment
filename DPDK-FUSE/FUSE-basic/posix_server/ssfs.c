#define FUSE_USE_VERSION 30

#include <fuse.h>
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

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/fcntl.h>

#include <sys/queue.h>
#include "ssfs_common.h"
//#include "ssfs_posix.h"
//#include "ssfs_fuse.h"


int main( int argc, char **argv )
{

    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

//    printf("FUSE-DPDK BEGIN\n");
//    pthread_t threads[3];
//    int rc = pthread_create(&threads[0], NULL, tcp_msg_launch, NULL);
//        rc = pthread_create(&threads[2], NULL, fuse_rx_launch, NULL);

    printf("FUSE BEGIN\n");
    fuse_main( argc, argv, &operations, NULL );
    printf("FUSE END\n");
    return 0;
}