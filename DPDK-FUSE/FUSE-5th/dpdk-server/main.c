#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
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

	NOFILESYSTEM = 0;
	printf("CHECKING ARGUEMENTS\n");
	if(argc>1){
		int i;
		for(i=0;i<argc; i++){
			// printf("%s\n",argv[i]);

			if(strcmp(argv[i],"NOFILESYSTEM")==0){
				NOFILESYSTEM = 1;
			}

			if(strcmp(argv[i],"/dev/nvme0n1p1")==0){
				strcpy(raw_device, "/dev/nvme0n1p1");
			}
		}
	}

	if(strlen(raw_device)==0){
		printf("no ssd location\n");
		return 0;
	}


	gethostname(hostname, 1023);
	dpdk_init();

	printf("FUSE-DPDK-SERVER CONFIGURATION BEGIN\n");
	if(NOFILESYSTEM == 1) printf("\tNOFILESYSTEM");
	if(strlen(raw_device)>0) printf("\t%s");
	printf("FUSE-DPDK-SERVER CONFIGURATION END\n");


	/* launch per-lcore init on every lcore */
	rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
	printf("Closing port %d...", 1);
	rte_eth_dev_stop(1);
	rte_eth_dev_close(1);
	printf(" Done\n");
	printf("Bye...\n");

}
