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

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <sys/queue.h>
#include "ssfs_common.h"
#include "ssfs_dpdk.h"
#include "ssfs_fuse.h"



static int
l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
{
    unsigned lcore_id = rte_lcore_id();
    if(lcore_id==4){
        printf("lcore[%u] works for the tx_loop\n", lcore_id);
        l2fwd_tx_loop();
    }else{
        printf("lcore[%u] works for the rx_loop\n", lcore_id);
        l2fwd_rx_loop();
    }
    return 0;
}


void *dpdk_launch(){
    rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
}


int main( int argc, char **argv )
{


    printf("CHECKING ARGUEMENTS\n");
    if(argc>0){
        int i =0;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"CACHE_HIT")==0){
                CACHE_HIT = 1;
            }
        }
    }


    TAILQ_INIT(&dpdk_queue);
    TAILQ_INIT(&avg_queue);
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

    gethostname(hostname, 1023);

    printf("FUSE-DPDK-CLIENT BEGIN\n");
    pthread_t threads[2];
    dpdk_msg_init();
    int rc = pthread_create(&threads[0], NULL, dpdk_launch, NULL);
    // rc = pthread_create(&threads[2], NULL, without_fuse_launch, NULL); while(1);

    printf("FUSE-DPDK-CLIENT BEGIN");
    if(CACHE_HIT == 1) printf(" WITH CACHE_HIT");
    printf("\n");

    umask(0);
    int fuse_argc = 3;
    char** fuse_argv;
    fuse_argv = malloc(fuse_argc * sizeof(char*));
    for (size_t i = 0; i < fuse_argc; i += 1)
        fuse_argv[i] = malloc(255 * sizeof(char));

    fuse_argv[0]="./ssfs";
    fuse_argv[1]="-f";
    fuse_argv[2]="/data1/sungho/trash";
    fuse_main(fuse_argc, fuse_argv, &xmp_oper, NULL);
    printf("FUSE-CLIENT END\n");
    return 0;
}