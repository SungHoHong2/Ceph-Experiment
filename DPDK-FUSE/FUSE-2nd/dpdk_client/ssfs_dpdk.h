
static volatile bool force_quit;

/* MAC updating enabled by default */
static int mac_updating = 1;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define NB_MBUF   8192

// #define MAX_PKT_BURST 32
#define MAX_PKT_BURST 1

#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */
#define MEMPOOL_CACHE_SIZE 256
static volatile int timerz = 100;

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

struct rte_mempool *test_pktmbuf_pool = NULL;


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

    /* 02:00:00:00:00:xx */
    tmp = &eth->d_addr.addr_bytes[0];
    *((uint64_t *)tmp) = 0xbcab839f36a0 + ((uint64_t)dest_portid << 40);

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

    char* data;
    struct message obj;
    struct fuse_message * e = NULL;
    struct message *msg;

     pthread_mutex_lock(&tx_lock);
     if(!TAILQ_EMPTY(&fuse_tx_queue)) {

            e = TAILQ_FIRST(&fuse_tx_queue);
            printf("send msg in DPDK: %s\n",e->data);
            strncpy(obj.data, e->data, 100);

            msg = &obj;
            data = rte_pktmbuf_append(m, sizeof(struct message));

            if (data != NULL)
                rte_memcpy(data, msg, sizeof(struct message));

            TAILQ_REMOVE(&fuse_tx_queue, e, nodes);
     }
     pthread_mutex_unlock(&tx_lock);


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

    pthread_mutex_lock(&rx_lock);
    e = malloc(sizeof(struct fuse_message));
    fprintf(f, "recv msg in DPDK: %s\n", msg->data);
    strcpy(e->data, msg->data);
    TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
    fflush(f);

    pthread_mutex_unlock(&rx_lock);
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

            pthread_mutex_lock(&tx_lock);
            if(!TAILQ_EMPTY(&fuse_tx_queue)) {
                e = TAILQ_FIRST(&fuse_tx_queue);
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
                TAILQ_REMOVE(&fuse_tx_queue, e, nodes);
            }
            pthread_mutex_unlock(&tx_lock);
    }
}



void
*l2fwd_rx_loop() {
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
        /*
         * Read packet from RX queues
         */
        for (i = 0; i < qconf->n_rx_port; i++) {
            portid = qconf->rx_port_list[0];
            nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
                                     pkts_burst, MAX_PKT_BURST);

            for (j = 0; j < nb_rx; j++) {
                //CHARA BEGIN
                m = pkts_burst[j];

                int rte_mbuf_packet_length = rte_pktmbuf_pkt_len(m);
                int header_length = rte_mbuf_packet_length - 1024;

                if (rte_mbuf_packet_length == 1024) {
                    dpdk_pktmbuf_dump(stdout, m, 1024, 0);
                }
                //CHARA END
                rte_prefetch0(rte_pktmbuf_mtod(m, void * ));
                rte_pktmbuf_free(m);
            }
        }
    }

}


//static int
//l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
//{
//    l2fwd_main_loop();
//    return 0;
//}


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


/* Parse the argument given in the command line of the application */
static int
l2fwd_parse_args(int argc, char **argv)
{
    int opt, ret;
    char **argvopt;
    int option_index;
    char *prgname = argv[0];
    argvopt = argv;
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


struct thread_data
{
    int c;
    char **v;
};


void *dpdk_msg_init(void *threadarg) {

    printf("DPDK BEGIN\n");

    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    int argc = my_data->c;
    char **argv = my_data->v;

    sleep(5);

    struct lcore_queue_conf *qconf;
    struct rte_eth_dev_info dev_info;
    int ret;
    uint8_t nb_ports;
    uint8_t nb_ports_available;
    uint8_t portid, last_port;
    unsigned lcore_id, rx_lcore_id;
    unsigned nb_ports_in_mask = 0;


    int dpdk_argc = 12;
    char** dpdk_argv;
    dpdk_argv = malloc(dpdk_argc * sizeof(char*));
    for (size_t i = 0; i < dpdk_argc; i += 1)
        dpdk_argv[i] = malloc(255 * sizeof(char));

    dpdk_argv[0]="./sfss";
    dpdk_argv[1]="-c";
    dpdk_argv[2]="0x2";
    dpdk_argv[3]="-n";
    dpdk_argv[4]="4";
    dpdk_argv[5]="--";
    dpdk_argv[6]="-q";
    dpdk_argv[7]="8";
    dpdk_argv[8]="-p";
    dpdk_argv[9]="0x1";
    dpdk_argv[10]="-T";
    dpdk_argv[11]="1";

    /* init EAL */
    ret = rte_eal_init(dpdk_argc, dpdk_argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
    argc -= ret;
    argv += ret;

    force_quit = false;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* parse application arguments (after the EAL ones) */
    ret = l2fwd_parse_args(argc, argv);
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
        if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
            continue;

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


        /* Start device */
        ret = rte_eth_dev_start(portid);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n",
                     ret, (unsigned) portid);

        printf("done: \n");

        rte_eth_promiscuous_disable(portid);

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

    check_all_ports_link_status(nb_ports, l2fwd_enabled_port_mask);

//    ret = 0;
//    /* launch per-lcore init on every lcore */
//    rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
//    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
//        if (rte_eal_wait_lcore(lcore_id) < 0) {
//            ret = -1;
//            break;
//        }
//    }

    printf("DPDK END\n");
}

