#include "dpdk_common.h"
#include "dpdk.h"


static int
l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
{
	l2fwd_rx_loop();
	return 0;
}


int
main(int argc, char **argv)
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
			if(strcmp(argv[i],"CACHE_HIT")==0){
				cache_hit = 1;
			}
			if(strcmp(argv[i],"CACHE_COMPACT")==0){
				cache_compact = 1;
			}

		}
	}


	if(strlen(raw_device)==0){
		printf("no ssd location\n");
		return 0;
	}

	gethostname(hostname, CON_SIZE);
	dpdk_init();

	printf("FUSE-DPDK-SERVER CONFIGURATION BEGIN\n");
	if(cache_miss == 1) printf("\tCACHE_MISS\n");
	if(cache_hit == 1) printf("\tCACHE_HIT\n");
	if(cache_compact == 1) printf("\tCACHE_COMPACT\n");
	if(strlen(raw_device)>0) printf("\t%s\n", raw_device);
	printf("FUSE-DPDK-SERVER CONFIGURATION END\n");


	/* launch per-lcore init on every lcore */
	rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
	printf("Closing port %d...", 1);
	rte_eth_dev_stop(1);
	rte_eth_dev_close(1);
	printf(" Done\n");
	printf("Bye...\n");

}
