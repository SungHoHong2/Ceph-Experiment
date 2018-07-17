#include <sys/time.h>

TAILQ_HEAD(avg_head, avg_node) avg_queue;
struct avg_node
{
    uint64_t start_time;
    uint64_t end_time;
    uint64_t interval;
    TAILQ_ENTRY(fuse_message) nodes;
};


uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}



struct message {
    char data[1024];
};


struct fuse_message
{
    char data[1024];
    TAILQ_ENTRY(fuse_message) nodes;
};

TAILQ_HEAD(tx_head, fuse_message) fuse_tx_queue;
TAILQ_HEAD(rx_head, fuse_message) fuse_rx_queue;
pthread_mutex_t rx_lock;
pthread_mutex_t tx_lock;


