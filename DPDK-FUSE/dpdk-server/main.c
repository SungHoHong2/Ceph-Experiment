#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <inttypes.h>
#include <sys/queue.h>
#include <errno.h>
#include <netinet/ip.h>

#include <rte_common.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_byteorder.h>
#include <rte_launch.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_atomic.h>
#include <rte_ring.h>
#include <rte_log.h>
#include <rte_debug.h>
#include <rte_mempool.h>
#include <rte_memcpy.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_ethdev.h>
#include <rte_byteorder.h>
#include <rte_malloc.h>
#include <rte_string_fns.h>


#include "common.h"
#include "args.h"
#include "init.h"

/*
 * When doing reads from the NIC or the client queues,
 * use this batch size
 */
#define PACKET_READ_SIZE 32

/*
 * Local buffers to put packets in, used to send packets in bursts to the
 * clients
 */
struct client_rx_buf {
    struct rte_mbuf *buffer[PACKET_READ_SIZE];
    uint16_t count;
};

/* One buffer per client rx queue - dynamically allocate array */
static struct client_rx_buf *cl_rx_buf;





int main(int argc, char *argv[]){

    /* initialise the system */
    if (init(argc, argv) < 0 )
        return -1;

    RTE_LOG(INFO, APP, "Finished Process Init.\n");

    /* allocate Local buffers */
    cl_rx_buf = calloc(num_clients, sizeof(cl_rx_buf[0]));






    return 0;
}