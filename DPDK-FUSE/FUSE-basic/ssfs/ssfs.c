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



void *fuse_tx_launch() {
    printf("FUSE-TX BEGIN\n");
    while(1) {
        pthread_mutex_lock(&tx_lock);
        if(!TAILQ_EMPTY(&fuse_tx_queue)) {
            e = TAILQ_FIRST(&fuse_tx_queue);
            printf("send msg in FUSE: %s\n", e->data);
            TAILQ_REMOVE(&fuse_tx_queue, e, nodes);
            free(e);
            e = NULL;
        }
        pthread_mutex_unlock(&tx_lock);
    }
}


void *fuse_rx_launch() {

    printf("FUSE-RX BEGIN\n");
    struct fuse_message * e = NULL;
    while(1) {
        pthread_mutex_lock(&rx_lock);
        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            printf("recv msg in FUSE: %s\n", e->data);
            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
            free(e);
            e = NULL;
        }
        pthread_mutex_unlock(&rx_lock);
    }
}




int main( int argc, char **argv )
{

    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("FUSE-DPDK BEGIN\n");
    pthread_t threads[3];
    struct thread_data td[3];
    td[0].c = argc;
    td[0].v = argv;
    int rc = pthread_create(&threads[0], NULL, dpdk_msg_launch, (void *)&td[0]);
        rc = pthread_create(&threads[1], NULL, fuse_tx_launch, NULL);
        rc = pthread_create(&threads[2], NULL, fuse_rx_launch, NULL);

    printf("FUSE BEGIN\n");
    fuse_main( argc, argv, &operations, NULL );
    printf("FUSE END\n");
    return 0;
}