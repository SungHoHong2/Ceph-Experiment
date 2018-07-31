#include "ssfs_common.h"
#include "ssfs_posix.h"

int main( int argc, char **argv )
{

    printf("CHECKING ARGUEMENTS\n");

    if (argc==1){
        printf("no arguments\n");
        return 0;
    }

    if(argc>1){
        int i;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"CACHE_MISS")==0){
                cache_miss = 1;
            }
            if(strcmp(argv[i],"nvme0n1p1")==0){
                strcpy(raw_device, "/dev/nvme0n1p1");
            }
            if(strcmp(argv[i],"DEBUG")==0){
                chara_debug = 1;
            }
        }
    }

    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&tx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("FUSE-POSIX-SERVER CONFIGURATION BEGIN\n");
    if(cache_miss == 1) printf("\tCACHE_MISS\n");
    if(strlen(raw_device)>0) printf("\t%s\n", raw_device);
    printf("FUSE-POSIX-SERVER CONFIGURATION END\n");

    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, tcp_recv_launch, NULL);
        rc = pthread_create(&threads[1], NULL, tcp_send_launch, NULL);

    while(1){};
    return 0;
}