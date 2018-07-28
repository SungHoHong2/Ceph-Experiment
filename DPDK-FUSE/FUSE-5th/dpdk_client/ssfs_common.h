#include <sys/time.h>
#include <math.h>
#define PKT_SIZE 4089

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


struct fuse_message
{
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

