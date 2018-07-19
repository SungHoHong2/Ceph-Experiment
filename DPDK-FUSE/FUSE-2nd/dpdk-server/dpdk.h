static volatile bool force_quit;

/* MAC updating enabled by default */
static int mac_updating = 1;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define NB_MBUF   8192

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */
#define MEMPOOL_CACHE_SIZE 256

/*
 * Configurable number of RX/TX ring descriptors
 */
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512
static uint16_t nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
static uint16_t nb_txd = RTE_TEST_TX_DESC_DEFAULT;

/* ethernet addresses of ports */
static struct ether_addr l2fwd_ports_eth_addr[RTE_MAX_ETHPORTS];

/* mask of enabled ports */
static uint32_t l2fwd_enabled_port_mask = 0;

/* list of enabled ports */
static uint32_t l2fwd_dst_ports[RTE_MAX_ETHPORTS];

static unsigned int l2fwd_rx_queue_per_lcore = 1;

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16
struct lcore_queue_conf {
    unsigned n_rx_port;
    unsigned rx_port_list[MAX_RX_QUEUE_PER_LCORE];
} __rte_cache_aligned;
struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

static struct rte_eth_dev_tx_buffer *tx_buffer[RTE_MAX_ETHPORTS];

static const struct rte_eth_conf port_conf = {
        .rxmode = {
                .split_hdr_size = 0,
                .header_split   = 0, /**< Header Split disabled */
                .hw_ip_checksum = 0, /**< IP checksum offload disabled */
                .hw_vlan_filter = 0, /**< VLAN filtering disabled */
                .jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
                .hw_strip_crc   = 1, /**< CRC stripped by hardware */
        },
        .txmode = {
                .mq_mode = ETH_MQ_TX_NONE,
        },
};

struct rte_mempool * l2fwd_pktmbuf_pool = NULL;

/* Per-port statistics struct */
struct l2fwd_port_statistics {
    uint64_t tx;
    uint64_t rx;
    uint64_t dropped;
} __rte_cache_aligned;
struct l2fwd_port_statistics port_statistics[RTE_MAX_ETHPORTS];

#define MAX_TIMER_PERIOD 86400 /* 1 day max */
/* A tsc-based timer responsible for triggering statistics printout */
static uint64_t timer_period = 10; /* default period is 10 seconds */

static void
l2fwd_mac_updating(struct rte_mbuf *m, unsigned dest_portid)
{
    struct ether_hdr *eth;
    void *tmp;

    eth = rte_pktmbuf_mtod(m, struct ether_hdr *);

    /* 02:00:00:00:00:xx */
    tmp = &eth->d_addr.addr_bytes[0];
    *((uint64_t *)tmp) = 0xd4d4a6211b00 + ((uint64_t)dest_portid << 40);

    /* src addr */
    ether_addr_copy(&l2fwd_ports_eth_addr[dest_portid], &eth->s_addr);
}

static void
l2fwd_simple_forward(struct rte_mbuf *m, unsigned portid)
{
    unsigned dst_port;
    int sent;
    struct rte_eth_dev_tx_buffer *buffer;

    dst_port = l2fwd_dst_ports[portid];

    if (mac_updating)
        l2fwd_mac_updating(m, dst_port);
    buffer = tx_buffer[dst_port];
    sent = rte_eth_tx_buffer(dst_port, 0, buffer, m);
    if (sent)
        port_statistics[dst_port].tx += sent;
}


void
dpdk_packet_hexdump(FILE *f, const char * title, const void * buf, unsigned int len, int start)
{
    unsigned int ofs;
    const unsigned char *data = buf;
    ofs = start;
    data+=ofs;
    struct fuse_message *e = NULL;
    struct message *msg = (struct message *) data;

    if(strlen(msg->data)>=24 && strcmp(msg->data, "Hello World From CLIENT!\n")==0) {

        pthread_mutex_lock(&rx_lock);
        e = malloc(sizeof(struct fuse_message));
        fprintf(f, "recv msg in DPDK: %s\n",msg->data);
        strcpy(e->data, msg->data);
        TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
        fflush(f);
        pthread_mutex_unlock(&rx_lock);
    }
}


void dpdk_pktmbuf_dump(FILE *f, const struct rte_mbuf *m, unsigned dump_len, int start)
{
    unsigned int len;
    unsigned nb_segs;
    __rte_mbuf_sanity_check(m, 1);
    nb_segs = m->nb_segs;

    while (m && nb_segs != 0) {
        __rte_mbuf_sanity_check(m, 0);
        len = dump_len;

        if (len > m->data_len)
            len = m->data_len;
        if (len != 0) {
            dpdk_packet_hexdump(f, NULL, rte_pktmbuf_mtod(m, void * ), len, start);
        }
        dump_len -= len;
        m = m->next;
        nb_segs --;
    }
}


void
* l2fwd_rx_loop()
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    struct rte_mbuf *m;

    unsigned lcore_id;
    unsigned i, j, portid, nb_rx;
    struct lcore_queue_conf *qconf;

    lcore_id = 1;
    qconf = &lcore_queue_conf[lcore_id];


    while (!force_quit) {
        /*
         * Read packet from RX queues
         */
        for (i = 0; i < qconf->n_rx_port; i++) {

            portid = qconf->rx_port_list[i];
            nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
                                     pkts_burst, MAX_PKT_BURST);


            for (j = 0; j < nb_rx; j++) {

                //CHARA BEGIN
                m = pkts_burst[j];

                int rte_mbuf_packet_length = rte_pktmbuf_pkt_len(m);
                if(rte_mbuf_packet_length==1024){
                    // printf("header_length: %d\n", header_length);  // lenght of the offset: 456
                    dpdk_pktmbuf_dump(stdout, m, 1024, 0);
                }
                //CHARA END
                // rte_prefetch0(rte_pktmbuf_mtod(m, void *));
                // l2fwd_simple_forward(m, portid);


//                fprintf(f, "recv msg: %s\n", msg->data);
//                int c;
//                FILE *file;
//                char data[24];
//                file = fopen("/mnt/ssd_cache/server", "r");
//                if (file) {
//                    c = fread(data, sizeof(char), 24, file);
//                    printf("send msg in FILESYSTEM: %s\n", data);
//                    fclose(file);
//                }
//                rte_memcpy(msg->data, data, sizeof(char)*24);

            }
        }
    }
}


/* main processing loop */
void
*l2fwd_tx_loop()
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    struct rte_mbuf *m;

    unsigned lcore_id;
    unsigned i, j, portid, nb_rx;
    struct lcore_queue_conf *qconf;
    struct rte_eth_dev_tx_buffer *buffer;

    lcore_id = 1;
    qconf = &lcore_queue_conf[lcore_id];

    struct rte_mbuf *rm[1];

    while (!force_quit) {
        portid = qconf->rx_port_list[0];
        char* data;
        struct message obj;
        struct fuse_message * e = NULL;
        struct message *msg;
        struct rte_mbuf *rm[1];

        pthread_mutex_lock(&rx_lock);
        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            printf("send msg in DPDK: %s\n",e->data);
            msg = &obj;
            strncpy(obj.data, e->data, 100);
            rm[0] = rte_pktmbuf_alloc(test_pktmbuf_pool);
            l2fwd_mac_updating(rm[0], portid);

            data = rte_pktmbuf_append(rm[0], sizeof(struct message));

            if (data != NULL)
                rte_memcpy(data, msg, sizeof(struct message));

            rte_prefetch0(rte_pktmbuf_mtod(rm[0], void *));
            rte_eth_tx_burst(portid, 0, rm, 1);
            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
        }
        pthread_mutex_unlock(&rx_lock);
    }
}




static int
l2fwd_parse_portmask(const char *portmask)
{
    char *end = NULL;
    unsigned long pm;

    /* parse hexadecimal string */
    pm = strtoul(portmask, &end, 16);
    if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
        return -1;

    if (pm == 0)
        return -1;

    return pm;
}

static unsigned int
l2fwd_parse_nqueue(const char *q_arg)
{
    char *end = NULL;
    unsigned long n;

    /* parse hexadecimal string */
    n = strtoul(q_arg, &end, 10);
    if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
        return 0;
    if (n == 0)
        return 0;
    if (n >= MAX_RX_QUEUE_PER_LCORE)
        return 0;

    return n;
}

static int
l2fwd_parse_timer_period(const char *q_arg)
{
    char *end = NULL;
    int n;

    /* parse number string */
    n = strtol(q_arg, &end, 10);
    if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
        return -1;
    if (n >= MAX_TIMER_PERIOD)
        return -1;

    return n;
}

static const char short_options[] =
        "p:"  /* portmask */
        "q:"  /* number of queues */
        "T:"  /* timer period */
;

#define CMD_LINE_OPT_MAC_UPDATING "mac-updating"
#define CMD_LINE_OPT_NO_MAC_UPDATING "no-mac-updating"

enum {
    /* long options mapped to a short option */

    /* first long only option value must be >= 256, so that we won't
     * conflict with short options */
            CMD_LINE_OPT_MIN_NUM = 256,
};

static const struct option lgopts[] = {
        { CMD_LINE_OPT_MAC_UPDATING, no_argument, &mac_updating, 1},
        { CMD_LINE_OPT_NO_MAC_UPDATING, no_argument, &mac_updating, 0},
        {NULL, 0, 0, 0}
};

/* Parse the argument given in the command line of the application */
static int
l2fwd_parse_args()
{
    int ret;
    char *prgname = "dpdk-server";
    l2fwd_enabled_port_mask = l2fwd_parse_portmask("0x1");
    l2fwd_rx_queue_per_lcore = l2fwd_parse_nqueue("8");
    ret = 6;
    return ret;
}


/* Check the link status of all ports in up to 9s, and print them finally */
static void
check_all_ports_link_status(uint8_t port_num, uint32_t port_mask)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
    uint8_t portid, count, all_ports_up, print_flag = 0;
    struct rte_eth_link link;

    printf("\nChecking link status");
    fflush(stdout);
    for (count = 0; count <= MAX_CHECK_TIME; count++) {
        if (force_quit)
            return;
        all_ports_up = 1;
        for (portid = 0; portid < port_num; portid++) {
            if (force_quit)
                return;
            if ((port_mask & (1 << portid)) == 0)
                continue;
            memset(&link, 0, sizeof(link));
            rte_eth_link_get_nowait(portid, &link);
            /* print link status if flag set */
            if (print_flag == 1) {
                if (link.link_status)
                    printf("Port %d Link Up - speed %u "
                           "Mbps - %s\n", (uint8_t)portid,
                           (unsigned)link.link_speed,
                           (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
                           ("full-duplex") : ("half-duplex\n"));
                else
                    printf("Port %d Link Down\n",
                           (uint8_t)portid);
                continue;
            }
            /* clear all_ports_up flag if any link down */
            if (link.link_status == ETH_LINK_DOWN) {
                all_ports_up = 0;
                break;
            }
        }
        /* after finally printing all link status, get out */
        if (print_flag == 1)
            break;

        if (all_ports_up == 0) {
            printf(".");
            fflush(stdout);
            rte_delay_ms(CHECK_INTERVAL);
        }

        /* set the print_flag if all ports up or timeout */
        if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
            print_flag = 1;
            printf("done\n");
        }
    }
}



static void
signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        printf("\n\nSignal %d received, preparing to exit...\n",
               signum);
        force_quit = true;
    }
}