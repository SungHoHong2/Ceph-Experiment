#include <sys/time.h>

int total_requests = 0;

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

void avg_results(){

    double avg =0;
    struct avg_node *av;
    while (!TAILQ_EMPTY(&avg_queue))
    {
        av = TAILQ_FIRST(&avg_queue);
        TAILQ_REMOVE(&avg_queue, av, nodes);

        avg += av->interval;

        free(e);
        av = NULL;
    }

    avg = avg/total_requests;
    printf("latency average: %f\n",avg);

    


//    std::cout <<"avg: " << avg << std::endl;
//
//
//    double variance = 0;
//    double t = samples[0];
//    for (int i = 1; i < size; i++)
//    {
//        t += samples[i];
//        double diff = ((i + 1) * samples[i]) - t;
//        variance += (diff * diff) / ((i + 1.0) *i);
//    }
//
//    double std_var = variance / (size - 1);
//    double std_dev = sqrt(std_var);
//    std::cout <<"std_dev: " << std_dev << std::endl;

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


