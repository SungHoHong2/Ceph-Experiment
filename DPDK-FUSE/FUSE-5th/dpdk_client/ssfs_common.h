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
#include <sys/time.h>
#include <math.h>

#define CON_SIZE 255
#define PKT_SIZE 1024
#define DATA_SIZE 4089
#define MERGE_PACKETS 4

int CHARA_DEBUG;
int MAX_LOOP;
int CACHE_HIT;

int total_requests = 0;
int dpdk_requests = 0;

TAILQ_HEAD(avg_head, avg_node) avg_queue;
TAILQ_HEAD(dpdk_head, avg_node) dpdk_queue;
TAILQ_HEAD(dpdkr_head, avg_node) dpdk_result;

struct avg_node
{
    uint64_t num;
    uint64_t start_time;
    uint64_t end_time;
    uint64_t interval;
    TAILQ_ENTRY(avg_node) nodes;
};

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}


struct message {
    char data[PKT_SIZE];
};


struct fuse_message {
    char data[PKT_SIZE];
    TAILQ_ENTRY(fuse_message) nodes;
};

TAILQ_HEAD(tx_head, fuse_message) fuse_tx_queue;
TAILQ_HEAD(rx_head, fuse_message) fuse_rx_queue;
pthread_mutex_t rx_lock;
pthread_mutex_t tx_lock;


struct avg_node *dpdk_av = NULL;
struct avg_node *av = NULL;
struct avg_node *avr = NULL;

