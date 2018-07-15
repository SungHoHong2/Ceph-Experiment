#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
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


struct message {
    char data[1024];
};

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
    struct message *msg = (struct message *) data;
    fprintf(f,"recv msg: %s\n", msg->data);
    fflush(f);
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
static void
l2fwd_main_loop(void)
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    struct rte_mbuf *m;

    unsigned lcore_id;
    unsigned i, j, portid, nb_rx;
    struct lcore_queue_conf *qconf;

    lcore_id = rte_lcore_id();
    qconf = &lcore_queue_conf[lcore_id];

    if (qconf->n_rx_port == 0) {
        RTE_LOG(INFO, L2FWD, "lcore %u has nothing to do\n", lcore_id);
        return;
    }

    RTE_LOG(INFO, L2FWD, "entering main loop on lcore %u\n", lcore_id);

    for (i = 0; i < qconf->n_rx_port; i++) {

        portid = qconf->rx_port_list[i];
        RTE_LOG(INFO, L2FWD, " -- lcoreid=%u portid=%u\n", lcore_id,
                portid);

    }

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
                int header_length =  rte_mbuf_packet_length - 1024;

                if(header_length>0){
                    // printf("rte_mbuf_packet_length: %d\n", rte_mbuf_packet_length);  // lenght of the offset: 456
                    // printf("header_length: %d\n", header_length);  // lenght of the offset: 456
                    dpdk_pktmbuf_dump(stdout, m, 1024, header_length);
                }
                //CHARA END
                rte_prefetch0(rte_pktmbuf_mtod(m, void *));
                l2fwd_simple_forward(m, portid);
            }
        }
    }
}

static int
l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
{
    l2fwd_main_loop();
    return 0;
}

/* display usage */
static void
l2fwd_usage(const char *prgname)
{
    printf("%s [EAL options] -- -p PORTMASK [-q NQ]\n"
           "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
           "  -q NQ: number of queue (=ports) per lcore (default is 1)\n"
           "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 10 default, 86400 maximum)\n"
           "  --[no-]mac-updating: Enable or disable MAC addresses updating (enabled by default)\n"
           "      When enabled:\n"
           "       - The source MAC address is replaced by the TX port MAC address\n"
           "       - The destination MAC address is replaced by 02:00:00:00:00:TX_PORT_ID\n",
           prgname);
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
l2fwd_parse_args(int argc, char **argv)
{
    int opt, ret, timer_secs;
    char **argvopt;
    int option_index;
    char *prgname = argv[0];

    argvopt = argv;

    while ((opt = getopt_long(argc, argvopt, short_options,
                              lgopts, &option_index)) != EOF) {

        switch (opt) {
            /* portmask */
            case 'p':
                l2fwd_enabled_port_mask = l2fwd_parse_portmask(optarg);
                if (l2fwd_enabled_port_mask == 0) {
                    printf("invalid portmask\n");
                    l2fwd_usage(prgname);
                    return -1;
                }
                break;

                /* nqueue */
            case 'q':
                l2fwd_rx_queue_per_lcore = l2fwd_parse_nqueue(optarg);
                if (l2fwd_rx_queue_per_lcore == 0) {
                    printf("invalid queue number\n");
                    l2fwd_usage(prgname);
                    return -1;
                }
                break;

                /* timer period */
            case 'T':
                timer_secs = l2fwd_parse_timer_period(optarg);
                if (timer_secs < 0) {
                    printf("invalid timer period\n");
                    l2fwd_usage(prgname);
                    return -1;
                }
                timer_period = timer_secs;
                break;

                /* long options */
            case 0:
                break;

            default:
                l2fwd_usage(prgname);
                return -1;
        }
    }

    if (optind >= 0)
        argv[optind-1] = prgname;

    ret = optind-1;
    optind = 1; /* reset getopt lib */
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


static int do_getattr( const char *path, struct stat *st )
{
    printf( "[getattr] Called\n" );
    printf( "\tAttributes of %s requested\n", path );

    // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
    // 		st_uid: 	The user ID of the file’s owner.
    //		st_gid: 	The group ID of the file.
    //		st_atime: 	This is the last access time for the file.
    //		st_mtime: 	This is the time of the last modification to the contents of the file.
    //		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
    //		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
    //						as no process still holds it open. Symbolic links are not counted in the total.
    //		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

    if ( strcmp( path, "/" ) == 0 )
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    }
    else
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
    }

    return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    printf( "--> Getting The List of Files of %s\n", path );

    filler( buffer, ".", NULL, 0 ); // Current Directory
    filler( buffer, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
        filler( buffer, "client", NULL, 0 );
        filler( buffer, "server", NULL, 0 );
    }

    return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    printf( "--> Trying to read %s, %u, %u\n", path, offset, size );

    char client[] = "Hello World From CLIENT!\n";
    char server[] = "Hello World From SERVER!\n";
    char *selectedText = NULL;

    // ... //

    if ( strcmp( path, "/client" ) == 0 )
        selectedText = client;
    else if ( strcmp( path, "/server" ) == 0 )
        selectedText = server;
    else
        return -1;

    // ... //

    memcpy( buffer, selectedText + offset, size );

    return strlen( selectedText ) - offset;
}

static struct fuse_operations operations = {
        .getattr	= do_getattr,
        .readdir	= do_readdir,
        .read		= do_read,
};


struct thread_data
{
    int  thread_id;
    int c;
    char **v;
    char *message;
};

void *PrintHello(void *threadarg) {

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


    char *dpdk_argv[] = {
            (char *) "-c",
            (char *) "0x20",
            (char *) "-n",
            (char *) "4",
            (char *) "--",
            (char *) "-q",
            (char *) "8",
            (char *) "-p",
            (char *) "0x1",
            (char *) "-T",
            (char *) "1",
            NULL
    };

    int dpdk_argc = 12;
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

    /* convert to number of cycles */
    timer_period *= rte_get_timer_hz();

    /* create the mbuf pool */
    l2fwd_pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NB_MBUF,
                                                 MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                                 rte_socket_id());
    if (l2fwd_pktmbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

    /* reset l2fwd_dst_ports */
    for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++)
        l2fwd_dst_ports[portid] = 0;
    last_port = 0;





    printf("DPDK END\n");
}

int main( int argc, char **argv )
{
    printf("FUSE-DPDK BEGIN\n");

    pthread_t threads[2];
    struct thread_data td[2];
    td[0].c = argc;
    td[0].v = argv;
    // int rc = pthread_create(&threads[0], NULL, PrintHello, (void *)&td[0]);

    printf("FUSE BEGIN\n");
    fuse_main( argc, argv, &operations, NULL );
    printf("FUSE END\n");
    return 0;

}