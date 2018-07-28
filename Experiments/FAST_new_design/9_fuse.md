### First Basic Results of Cache-Miss and Cache-Hit
- [ ] Run the device on NVM-e
- [ ] User direct-io *having trouble with configuring ssd*
- [ ] use HDD in client
- [ ] `send data 4kb per message`
    - it seems that l2fwd works when I change the packet size in the rte_mempool
    - 

**Jumbo Frames**
https://software.intel.com/en-us/articles/jumbo-frames-in-open-vswitch-with-dpdk
sudocmd ip link set p5p2 mtu 9000
ip link show | grep mtu
perhaps we need to restart the computer




### Second Modify DPDK favorable to GDCache
- [ ] Fuse working with low-level pass through
- [ ] Test the performance using FIO.
