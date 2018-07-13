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

void *pkts[PACKET_READ_SIZE];
struct rte_eth_dev_tx_buffer *tx_buffer[0];

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


static void
handle_packet(struct rte_mbuf *buf)
{
    int sent;
    struct rte_eth_dev_tx_buffer *buffer = tx_buffer[0];
    // sent = rte_eth_tx_buffer_flush(0, 0, buffer);
    sent = rte_eth_tx_buffer(0, 0, buffer, buf);
    if (sent)
        printf("CHARA::rte_eth_tx_buffer::%d\n,",sent);

}

/*
 * send a burst of traffic to a client, assuming there are packets
 * available to be sent to this client
 */
static void
flush_rx_queue(uint16_t client)
{
    uint16_t j, sent;
    struct client *cl;
    uint16_t i, rx_pkts;


    if (cl_rx_buf[client].count == 0)
        return;

    cl = &clients[client];

    if (rte_ring_enqueue_bulk(cl->rx_q, (void **)cl_rx_buf[client].buffer,
                              cl_rx_buf[client].count, NULL) == 0){
        for (j = 0; j < cl_rx_buf[client].count; j++)
            rte_pktmbuf_free(cl_rx_buf[client].buffer[j]);
        cl->stats.rx_drop += cl_rx_buf[client].count;
    }
    else {
        cl->stats.rx += cl_rx_buf[client].count;
        rx_pkts = rte_ring_dequeue_burst(cl->rx_q, pkts, PACKET_READ_SIZE, NULL);
    }

    if (unlikely(rx_pkts == 0)) {
            sent = rte_eth_tx_buffer_flush(ports->id[0], 0, tx_buffer[0]);
    }

    // RTE_LOG(INFO, APP, "CHARA: rte_ring_enqueue_bulk::stats.rx: %d\n", cl->stats.rx);
    // RTE_LOG(INFO, APP, "CHARA: rte_ring_enqueue_bulk::stats.rx_drop: %d\n", cl->stats.rx_drop);
    // RTE_LOG(INFO, APP, "CHARA: rte_ring_dequeue_burst::rx_pkts: %d\n", rx_pkts);
    cl_rx_buf[client].count = 0;

    for (i = 0; i < rx_pkts; i++)
        handle_packet(pkts[i]);
}



/*
 * marks a packet down to be sent to a particular client process
 */
static inline void
enqueue_rx_packet(uint8_t client, struct rte_mbuf *buf)
{
    // RTE_LOG(INFO, APP, "CHARA: enqueue_rx_packet\n");
    cl_rx_buf[client].buffer[cl_rx_buf[client].count++] = buf;
}


static void
process_packets(uint32_t port_num __rte_unused,
                struct rte_mbuf *pkts[], uint16_t rx_count)
{
    uint16_t i;
    uint8_t client = 0;
    // RTE_LOG(INFO, APP, "CHARA: process_packets\n");

    for (i = 0; i < rx_count; i++) {
        enqueue_rx_packet(client, pkts[i]);

        if (++client == num_clients)
            client = 0;
    }

    for (i = 0; i < num_clients; i++)
        flush_rx_queue(i);
}



/*
 * Function called by the master lcore of the DPDK process.
 */
static void
do_packet_forwarding(void)
{
    unsigned port_num = 0; /* indexes the port[] array */

    RTE_LOG(INFO, APP, "CHARA: loop start\n");

    for (;;) {
        struct rte_mbuf *buf[PACKET_READ_SIZE];
        uint16_t rx_count;
        sleep(1);


        RTE_LOG(INFO, APP, "CHARA: reading port num::%d\n", port_num);
        /* read a port */
        rx_count = rte_eth_rx_burst(ports->id[port_num], 0, \
				buf, PACKET_READ_SIZE);
        ports->rx_stats.rx[port_num] += rx_count;
        RTE_LOG(INFO, APP, "CHARA: rte_eth_rx_burst::%d\n", rx_count);


        /* Now process the NIC packets read */
        if (likely(rx_count > 0))
            process_packets(port_num, buf, rx_count);

//        /* move to next port */
//        if (++port_num == ports->num_ports)
//            port_num = 0;
    }
}




int main(int argc, char *argv[]){

    /* initialise the system */
    if (init(argc, argv) < 0 )
        return -1;

    RTE_LOG(INFO, APP, "CHARA: Finished Process Init.\n");

    /* allocate Local buffers */
    cl_rx_buf = calloc(num_clients, sizeof(cl_rx_buf[0]));

    /* put all other cores to sleep bar master */
    // I think this guy is just used by other cores
    // rte_eal_mp_remote_launch(sleep_lcore, NULL, SKIP_MASTER);

    //    rte_eal_mp_remote_launch(do_packet_forwarding, NULL, CALL_MASTER);
     do_packet_forwarding();

    return 0;
}