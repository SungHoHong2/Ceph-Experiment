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


### work order
1. Create client and server for DPDK
2. Use FUSE for simple file storage
3. COMBINE fuse that stores data from client to server and returns the result back
4  compare with the result that does not use FUSE as a file storage


PORTS
-----
Port 0: 'a0:36:9f:83:ab:bc'     Port 1: 'a0:36:9f:83:ab:bd'

Port 0 - rx:      6589  tx:      5793
Port 1 - rx:      6643  tx:      5713

CLIENTS
-------
Client  0 - rx:     11633, rx_drop:      1599
            tx:     11506, tx_drop:         0
