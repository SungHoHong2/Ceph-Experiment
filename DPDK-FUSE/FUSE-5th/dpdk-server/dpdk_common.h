#define PKT_SIZE 4089
int NOFILESYSTEM;

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


