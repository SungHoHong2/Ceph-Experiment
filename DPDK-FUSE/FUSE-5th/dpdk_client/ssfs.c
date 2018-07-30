#include "ssfs_common.h"
#include "ssfs_dpdk.h"
#include "ssfs_fuse.h"



static int
l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
{
    unsigned lcore_id = rte_lcore_id();
    if(lcore_id==4){
        printf("lcore[%u] works for the tx_loop\n", lcore_id);
        l2fwd_tx_loop();
    }else{
        printf("lcore[%u] works for the rx_loop\n", lcore_id);
        l2fwd_rx_loop();
    }
    return 0;
}


void *dpdk_launch(){
    rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
}


int main( int argc, char **argv )
{

    printf("CHECKING ARGUEMENTS\n");
    if(argc>1){
        int i =0;
        for(i=0;i<argc; i++){

            if(strcmp(argv[i],"DEBUG")==0){
                CHARA_DEBUG = 1;
            }

            if(strcmp(argv[i],"CACHE_HIT")==0){
                CACHE_HIT = 1;
            }

            if(strcmp(argv[i],"CACHE_MISS")==0){
                CACHE_MISS = 1;
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


    TAILQ_INIT(&dpdk_queue);
    TAILQ_INIT(&avg_queue);
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

    gethostname(hostname, 1023);
    pthread_t threads[2];
    dpdk_msg_init();
    int rc = pthread_create(&threads[0], NULL, dpdk_launch, NULL);
    // rc = pthread_create(&threads[2], NULL, without_fuse_launch, NULL); while(1);


    printf("FUSE-DPDK-CLIENT BEGIN\n");
    if(CACHE_HIT == 1) printf("\tWITH CACHE_HIT");
    if(CACHE_MISS == 1) printf("\tWITH CACHE_MISS");
    if(MAX_LOOP == 10) printf("\t10 REQ");
    if(MAX_LOOP == 100) printf("\t100 REQ");
    printf("FUSE-DPDK-CLIENT END\n");

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