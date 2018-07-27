#define FUSE_USE_VERSION 31
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

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
#include "ssfs_fuse.h"



int main( int argc, char **argv )
{
    TAILQ_INIT(&avg_queue);
    TAILQ_INIT(&posix_queue);
    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&tx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("FUSE-POSIX BEGIN\n");
    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, tcp_send_launch, NULL);
        rc = pthread_create(&threads[1], NULL, tcp_recv_launch, NULL);

        
    printf("FUS-CLIENT BEGIN\n");
    umask(0);

    fuse_main(argc, argv, &xmp_oper, NULL);
    printf("FUSE-CLIENT END\n");


    return 0;
}