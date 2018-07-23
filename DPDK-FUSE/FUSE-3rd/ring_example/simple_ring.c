#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/queue.h>
#include <rte_common.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_atomic.h>
#include <rte_branch_prediction.h>
#include <rte_ring.h>
#include <rte_log.h>
#include <rte_mempool.h>

#define RTE_LOGTYPE_APP RTE_LOGTYPE_USER1

static const char *_MSG_POOL = "MSG_POOL";
static const char *_SEC_2_PRI = "SEC_2_PRI";
static const char *_PRI_2_SEC = "PRI_2_SEC";
const unsigned string_size = 64;

struct rte_ring *send_ring, *recv_ring;
struct rte_mempool *message_pool;
volatile int quit = 0;

static int
lcore_recv(__attribute__((unused)) void *arg)
{
    unsigned lcore_id = rte_lcore_id();

    if(lcore_id==0){
        printf("CHARA: [ using double threads are proved ] this is the master core\n");
        printf("Starting core %u\n", lcore_id);
        while (!quit){
            // void *msg;
            char *pdata;
            char data[string_size];
            pdata= &data;
            strcpy(data,"howdy");

            sleep(1);
            printf("lcore[%u]:%s\n", lcore_id, pdata);

            if (rte_ring_enqueue(send_ring, pdata) < 0) {
                printf("Failed to send message - message discarded\n");
                rte_mempool_put(message_pool, msg);
            }





        }

    }

    if(lcore_id==1){
        printf("CHARA: [ using double threads are proved ] this is the second core\n");
        printf("Starting core %u\n", lcore_id);
        while (!quit){
            void *msg;
            if (rte_ring_dequeue(recv_ring, &msg) < 0){
                usleep(5);
                continue;
            }
            printf("CHARA: core %u: Received '%s'\n", lcore_id, (char *)msg);
            rte_mempool_put(message_pool, msg);
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    const unsigned flags = 0;
    const unsigned ring_size = 64;
    const unsigned pool_size = 1024;
    const unsigned pool_cache = 32;
    const unsigned priv_data_sz = 0;

    int ret;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Cannot init EAL\n");

    if (rte_eal_process_type() == RTE_PROC_PRIMARY){

        printf("CHARA: rte_ring_create\n");
        send_ring = rte_ring_create(_PRI_2_SEC, ring_size, rte_socket_id(), flags);
        recv_ring = rte_ring_create(_SEC_2_PRI, ring_size, rte_socket_id(), flags);
        message_pool = rte_mempool_create(_MSG_POOL, pool_size,
                                          string_size, pool_cache, priv_data_sz,
                                          NULL, NULL, NULL, NULL,
                                          rte_socket_id(), flags);
    }

    rte_eal_mp_remote_launch(lcore_recv, NULL, CALL_MASTER);
    return 0;
}
