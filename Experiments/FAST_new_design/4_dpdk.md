### Symmetric Multi-process Example
- https://doc.dpdk.org/guides-16.04/sample_app_ug/multi_process.html


**simple_smp**
using `two queues` and a `single memory pool` in shared memory
three objects are created at startup by the primary process,
secondary process cannot create objects in memory as it cannot reserve memory zones
the secondary process then uses lookup functions to attach to these objects as it starts


**mulit-process_support**
each process reads packets from each of the network ports in use
RSS is used to distribute incoming packets on each port to different hardware RX queues

Each process reads a different RX queue on each port
so does not contend with any other process for that queue access
Similarly, each process writes outgoing packets to a different TX queue on each port.


/home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/symmetric_mp/build/symmetric_mp -c 2 -n 4 --proc-type=auto -- -p 3 --num-procs=4 --proc-id=0

/home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/symmetric_mp/build/symmetric_mp -c 2 -n 4 --proc-type=auto -- -p 3 --num-procs=4 --proc-id=1


**Client-Server Multi-process Example**
a single server process performs the packet reception from the ports being used
distributes these packets using round-robin ordering among a set of client processes
perform level-2 forwarding of packets by sending each packet out on a different network port.


### building from l2fwd
  - it seems that crafting the packets or counting the packets within the tx and rx queue section does not work
  - when we remove the sleep then it is definitely sending something
  - `proved that we receive the data by using rte_pktmbuf_dump`
  - `if you add sleep in one client the won't be transferred`
  - https://stackoverflow.com/questions/38726332/dpdk-rte-mbuf-payload-data-initialization-or-updation-modification


  - lets test the packet in the userspace and then go aack
  -
