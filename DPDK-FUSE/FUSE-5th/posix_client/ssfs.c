

#include "ssfs_common.h"
#include "ssfs_posix.h"
#include "ssfs_fuse.h"



int main( int argc, char **argv )
{

    printf("CHECKING ARGUEMENTS\n");
    if(argc>1){
        int i =0;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"CACHE_HIT")==0){
                CACHE_HIT = 1;
            }
            if(strcmp(argv[i],"TEN")==0){
                MAX_LOOP = 10;
            }
            if(strcmp(argv[i],"HUNDRED")==0){
                MAX_LOOP = 100;
            }
            if(strcmp(argv[i],"THOUSAND")==0){
                MAX_LOOP = 1000;
            }

        }
    }

    TAILQ_INIT(&avg_queue);
    TAILQ_INIT(&posix_queue);
    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&tx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("FUSE-POSIX BEGIN\n");
    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, tcp_send_launch, NULL);
        rc = pthread_create(&threads[1], NULL, tcp_recv_launch, NULL);


    printf("FUSE-POSIX-CLIENT BEGIN");
    if(CACHE_HIT == 1) printf(" WITH CACHE_HIT");
    if(MAX_LOOP == 10) printf(" 10 REQ");
    if(MAX_LOOP == 100) printf(" 100 REQ");

    umask(0);
    int fuse_argc = 3;
    char** fuse_argv;
    fuse_argv = malloc(fuse_argc * sizeof(char*));
    for (size_t i = 0; i < fuse_argc; i += 1)
        fuse_argv[i] = malloc(255 * sizeof(char));

    fuse_argv[0]="./ssfs";
    fuse_argv[1]="-f";
    fuse_argv[2]= argv[2];
    fuse_main(fuse_argc, fuse_argv, &xmp_oper, NULL);
    printf("FUSE-CLIENT END\n");


    return 0;
}