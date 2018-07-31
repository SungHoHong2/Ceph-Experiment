#define _GNU_SOURCE
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
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>


#define PORT "1234"  // the port users will be connecting to
#define PKT_SIZE 1024 // max number of bytes we can get at once
#define BACKLOG 10     // how many pending connections queue will hold
#define CON_SIZE 255
#define DATA_SIZE 4089
#define MERGE_PACKETS 4
#define SECTOR 512

int cache_miss;
int chara_debug;

char raw_device[CON_SIZE];

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


