#include <stdio.h>
#include <unistd.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct thread_data
{
    int c;
    char **v;
};


struct fuse_message
{
    char data[1024];
    TAILQ_ENTRY(fuse_message) nodes;
};

TAILQ_HEAD(tx_head, fuse_message) fuse_tx_queue;
TAILQ_HEAD(rx_head, fuse_message) fuse_rx_queue;


void *fuse_tx_launch(void *threadarg) {

    while(1){
        sleep(1);
        printf("?????\n");
    }


//    while(1) {
//        sleep(1);
//        // printf("sending data man?\n");
//        struct fuse_message *e = NULL;
//        e = malloc(sizeof(struct fuse_message));
//        strcpy(e->data, "howdy");
//        TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
//
//        if(!TAILQ_EMPTY(&fuse_tx_queue)) {
//            e = TAILQ_FIRST(&fuse_tx_queue);
//            printf("send msg in FUSE: %s\n", e->data);
//            TAILQ_REMOVE(&fuse_tx_queue, e, nodes);
//            free(e);
//            e = NULL;
//        }
//    }


}


void *fuse_rx_launch(void *threadarg) {

    printf("FUSE-RX BEGIN\n");

    while(1){
        sleep(1);
        printf("xxxx\n");
    }

//    struct fuse_message * e = NULL;
//    while(1) {
//        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
//            e = TAILQ_FIRST(&fuse_rx_queue);
//            printf("recv msg in FUSE: %s\n", e->data);
//            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
//            free(e);
//            e = NULL;
//        }
//    }
}



int main(){

    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    pthread_t threads[3];
    struct thread_data td[3];

    printf("??\n");
    int rc = pthread_create(&threads[0], NULL, fuse_tx_launch, (void *)&td[10]);
    printf("??\n");
    rc = pthread_create(&threads[1], NULL, fuse_rx_launch, (void *)&td[1]);

    return 0;
}