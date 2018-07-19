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


int main( int argc, char **argv )
{

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

    printf("FUSE-DPDK-CLIENT BEGIN\n");
    pthread_t threads[3];
    struct thread_data td[3];
    td[0].c = argc;
    td[0].v = argv;
    dpdk_msg_init((void *)&td[0]);

    int rc = pthread_create(&threads[0], NULL, l2fwd_tx_loop, NULL);
        rc = pthread_create(&threads[2], NULL, l2fwd_rx_loop, NULL);
        rc = pthread_create(&threads[2], NULL, fuse_rx_launch, NULL);


    printf("FUS-CLIENT BEGIN\n");
    fuse_main( argc, argv, &operations, NULL );
    printf("FUSE-CLIENT END\n");
    return 0;
}