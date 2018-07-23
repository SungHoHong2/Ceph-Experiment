#include <sys/time.h>
#include <math.h>

int total_requests = 0;

TAILQ_HEAD(avg_head, avg_node) avg_queue;
struct avg_node
{
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

void avg_results(){

    double avg =0;
    struct avg_node *av;
    double variance = 0;
    double t = 0;

    int i =0;
    while (!TAILQ_EMPTY(&avg_queue))
    {
        av = TAILQ_FIRST(&avg_queue);

        printf("output: %ld\n",av->interval);

//        if(av->interval<=0){
//            total_requests--;
//        }

        avg += av->interval;
        TAILQ_REMOVE(&avg_queue, av, nodes);
        free(av);
        av = NULL;


//        if(i==0) {
//            t = av->interval;
//            free(av);
//            av = NULL;
//            i++;
//        } else {
//            t += av->interval;
//            double diff = ((i + 1) * av->interval) - t;
//            variance += (diff * diff) / ((i + 1.0) * i);
//
//            free(av);
//            av = NULL;
//            i++;
//        }
    }

    avg = avg/total_requests;
    printf("latency average: %f\n",avg);

    double std_var = variance / (total_requests - 1);
    double std_dev = sqrt(std_var);
    printf("latency std_dev: %f\n",std_dev);


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


