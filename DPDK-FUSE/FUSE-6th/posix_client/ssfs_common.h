#define _GNU_SOURCE
#define FUSE_USE_VERSION 31

#include <fuse_lowlevel.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <inttypes.h>

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
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define CON_SIZE 255
#define DATA_SIZE 4096
#define MERGE_PACKETS 4
#define PKT_SIZE 1024
#define SECTOR 512

int max_loop;
int cache_hit;
int cache_miss;
int cache_compact;
int raw_device;
int chara_debug;

int total_requests = 0;
int posix_requests = 0;


TAILQ_HEAD(avg_head, avg_node) avg_queue;
TAILQ_HEAD(dpdk_head, avg_node) posix_queue;

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
    char data[DATA_SIZE];
};


struct fuse_message
{
    char data[DATA_SIZE];
    TAILQ_ENTRY(fuse_message) nodes;
};

TAILQ_HEAD(tx_head, fuse_message) fuse_tx_queue;
TAILQ_HEAD(rx_head, fuse_message) fuse_rx_queue;
pthread_mutex_t rx_lock;
pthread_mutex_t tx_lock;

struct avg_node *av = NULL;
struct avg_node *posix_av = NULL;

