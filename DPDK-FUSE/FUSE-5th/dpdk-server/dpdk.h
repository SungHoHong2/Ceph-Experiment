#include <unistd.h>
char hostname[1024];
static volatile bool force_quit;

/* MAC updating enabled by default */
static int mac_updating = 1;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define NB_MBUF   8192

// #define MAX_PKT_BURST 32
#define MAX_PKT_BURST 1


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
struct rte_mempool *test_pktmbuf_pool = NULL;

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


static void
l2fwd_mac_updating(struct rte_mbuf *m, unsigned dest_portid)
{
    struct ether_hdr *eth;
    void *tmp;

    eth = rte_pktmbuf_mtod(m, struct ether_hdr *);
    tmp = &eth->d_addr.addr_bytes[0];
    if(strcmp(hostname,"w1")==0){
        *((uint64_t *)tmp) = 0xd5d4a6211b00 + ((uint64_t)dest_portid << 40);
    }else if (strcmp(hostname,"c3n24")==0){
        // ASU c3n24 -> c3n25 E4:1D:2D:D9:BF:B1
        *((uint64_t *)tmp) = 0xb1bfd92d1de4 + ((uint64_t)dest_portid << 40);
    }

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
    struct message obj;
    struct lcore_queue_conf *qconf;
    qconf = &lcore_queue_conf[1];
    unsigned portid = qconf->rx_port_list[0];

    e = malloc(sizeof(struct fuse_message));
        printf("recv msg in DPDK: %s\n",msg->data);
        strcpy(e->data, msg->data);

        struct rte_mbuf *rm[1];
        int c;
        char *zdata;

        if( NOFILESYSTEM == 1 ) {
            msg = &obj;
            strncpy(obj.data, "Hello World From SERVER!\n", 26);
        } else {
//            FILE *file;
//            char sdata[PKT_SIZE];
//            file = fopen("/mnt/ssd_cache/server", "r");
//            if (file) {
//                c = fread(sdata, sizeof(char), 26, file);
//                // printf("send msg in FILESYSTEM: %s\n", sdata);
//                fclose(file);
//            }
//            msg = &obj;
//            strncpy(obj.data, sdata, 26);
        }


        rm[0] = rte_pktmbuf_alloc(test_pktmbuf_pool);
        rte_prefetch0(rte_pktmbuf_mtod(rm[0], void *));

        zdata = rte_pktmbuf_append(rm[0], sizeof(struct message));
        zdata+=sizeof(struct ether_hdr)-2;

        rte_memcpy(zdata, msg, sizeof(struct message));
        l2fwd_mac_updating(rm[0], 0);

        // rte_pktmbuf_dump(stdout, rm[0], 60);
        printf("send msg in DPDK: %s\n", msg->data);
        rte_eth_tx_burst(1, 0, rm, 1);
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
l2fwd_rx_loop()
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
            portid = qconf->rx_port_list[0];
            nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
                                     pkts_burst, MAX_PKT_BURST);

            for (j = 0; j < nb_rx; j++) {
                m = pkts_burst[j];
                int rte_mbuf_packet_length = rte_pktmbuf_pkt_len(m);

//                 rte_pktmbuf_dump(stdout, m, 60);

                 if(rte_mbuf_packet_length==PKT_SIZE) {

                     if(strcmp(hostname,"w1")==0) {
                         dpdk_pktmbuf_dump(stdout, m, PKT_SIZE, 0);
                     }

                     if(strcmp(hostname,"c3n24")==0) {
                         dpdk_pktmbuf_dump(stdout, m, PKT_SIZE, sizeof(struct ether_hdr) - 2);
                     }
                 }

            }
    }
}


/* main processing loop */
void
*l2fwd_tx_loop()
{

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
    char *prgname = "dpdk-server_backup";
    l2fwd_enabled_port_mask = l2fwd_parse_portmask("0x2");
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


int dpdk_init(){

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

    struct lcore_queue_conf *qconf;
    struct rte_eth_dev_info dev_info;
    int ret;
    uint8_t nb_ports;
    uint8_t nb_ports_available;
    uint8_t portid, last_port;
    unsigned lcore_id, rx_lcore_id;
    unsigned nb_ports_in_mask = 0;

    int count = 12;
    char** dpdk_argv;
    dpdk_argv = malloc(count * sizeof(char*));      // allocate the array to hold the pointer
    for (size_t i = 0; i < count; i += 1)
        dpdk_argv[i] = malloc(255 * sizeof(char));  // allocate each array to hold the strings

    dpdk_argv[0]="./build/dpdk_server";
    dpdk_argv[1]="-c";
    dpdk_argv[2]="0x2";
    dpdk_argv[3]="-n";
    dpdk_argv[4]="4";
    dpdk_argv[5]="--";
    dpdk_argv[6]="-q";
    dpdk_argv[7]="8";
    dpdk_argv[8]="-p";
    dpdk_argv[9]="0x2";
    dpdk_argv[10]="-T";
    dpdk_argv[11]="1";


    /* init EAL */
    ret = rte_eal_init(count, dpdk_argv);
    force_quit = false;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* parse application arguments (after the EAL ones) */
    ret = l2fwd_parse_args();
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Invalid L2FWD arguments\n");

    printf("MAC updating %s\n", mac_updating ? "enabled" : "disabled");

    /* create the mbuf pool */
    l2fwd_pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NB_MBUF,
                                                 MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                                 rte_socket_id());

    /* create memory pool for send data */
    if (test_pktmbuf_pool == NULL) {
        test_pktmbuf_pool = rte_pktmbuf_pool_create("test_pktmbuf_pool",
                                                    NB_MBUF, MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    }


    if (l2fwd_pktmbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

    printf("CHARA: available ports %u\n",nb_ports);


    /* reset l2fwd_dst_ports */
    for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++)
        l2fwd_dst_ports[portid] = 0;
    last_port = 0;


    /*
     * Each logical core is assigned a dedicated TX queue on each port.
     */
    for (portid = 0; portid < nb_ports; portid++) {
        /* skip ports that are not enabled */
        if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
            continue;

        if (nb_ports_in_mask % 2) {
            l2fwd_dst_ports[portid] = last_port;
            l2fwd_dst_ports[last_port] = portid;
        }
        else
            last_port = portid;

        nb_ports_in_mask++;

        rte_eth_dev_info_get(portid, &dev_info);
    }
    if (nb_ports_in_mask % 2) {
        printf("Notice: odd number of ports in portmask.\n");
        l2fwd_dst_ports[last_port] = last_port;
    }

    rx_lcore_id = 0;
    qconf = NULL;

    /* Initialize the port/queue configuration of each logical core */
    for (portid = 0; portid < nb_ports; portid++) {
        /* skip ports that are not enabled */
        if ((l2fwd_enabled_port_mask & (1 << portid)) == 0) {
            printf("CHARA: port %u is skipped\n", portid);
            continue;
        }

        /* get the lcore_id for this port */
        while (rte_lcore_is_enabled(rx_lcore_id) == 0 ||
               lcore_queue_conf[rx_lcore_id].n_rx_port ==
               l2fwd_rx_queue_per_lcore) {
            rx_lcore_id++;
            if (rx_lcore_id >= RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "Not enough cores\n");
        }

        if (qconf != &lcore_queue_conf[rx_lcore_id])
            /* Assigned a new logical core in the loop above. */
            qconf = &lcore_queue_conf[rx_lcore_id];

        qconf->rx_port_list[qconf->n_rx_port] = portid;
        qconf->n_rx_port++;
        printf("Lcore %u: RX port %u\n", rx_lcore_id, (unsigned) portid);
    }

    nb_ports_available = nb_ports;

    /* Initialise each port */
    for (portid = 0; portid < nb_ports; portid++) {
        /* skip ports that are not enabled */
        if ((l2fwd_enabled_port_mask & (1 << portid)) == 0) {
            printf("Skipping disabled port %u\n", (unsigned) portid);
            nb_ports_available--;
            continue;
        }
        /* init port */
        printf("Initializing port %u... ", (unsigned) portid);
        fflush(stdout);
        ret = rte_eth_dev_configure(portid, 1, 1, &port_conf);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n",
                     ret, (unsigned) portid);

        rte_eth_macaddr_get(portid,&l2fwd_ports_eth_addr[portid]);

        /* init one RX queue */
        fflush(stdout);
        ret = rte_eth_rx_queue_setup(portid, 0, nb_rxd,
                                     rte_eth_dev_socket_id(portid),
                                     NULL,
                                     l2fwd_pktmbuf_pool);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup:err=%d, port=%u\n",
                     ret, (unsigned) portid);

        /* init one TX queue on each port */
        fflush(stdout);
        ret = rte_eth_tx_queue_setup(portid, 0, nb_txd,
                                     rte_eth_dev_socket_id(portid),
                                     NULL);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n",
                     ret, (unsigned) portid);

        /* Initialize TX buffers */
        tx_buffer[portid] = rte_zmalloc_socket("tx_buffer",
                                               RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST), 0,
                                               rte_eth_dev_socket_id(portid));
        if (tx_buffer[portid] == NULL)
            rte_exit(EXIT_FAILURE, "Cannot allocate buffer for tx on port %u\n",
                     (unsigned) portid);

        rte_eth_tx_buffer_init(tx_buffer[portid], MAX_PKT_BURST);

        if (ret < 0)
            rte_exit(EXIT_FAILURE, "Cannot set error callback for "
                                   "tx buffer on port %u\n", (unsigned) portid);

        /* Start device */
        ret = rte_eth_dev_start(portid);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n",
                     ret, (unsigned) portid);

        printf("done: \n");

        rte_eth_promiscuous_enable(portid);

        printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
               (unsigned) portid,
               l2fwd_ports_eth_addr[portid].addr_bytes[0],
               l2fwd_ports_eth_addr[portid].addr_bytes[1],
               l2fwd_ports_eth_addr[portid].addr_bytes[2],
               l2fwd_ports_eth_addr[portid].addr_bytes[3],
               l2fwd_ports_eth_addr[portid].addr_bytes[4],
               l2fwd_ports_eth_addr[portid].addr_bytes[5]);

    }

    if (!nb_ports_available) {
        rte_exit(EXIT_FAILURE,
                 "All available ports are disabled. Please set portmask.\n");
    }
}