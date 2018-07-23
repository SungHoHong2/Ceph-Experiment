### list
- [x] first test pure DPDK and POSIX and confirm that the performance is better than POSIX
  - [x] performance using DPDK with one logical core (echo)
    - it seems communication does not work well with pthread and logical cores
  - [x] performance using DPDK + two pthreads are worse than POSIX
    - `fun fact I found was that when POSIX is used with mutex then the threads works collaboratively`
    - `in case of DPDK teh threads work black and white`

- [] use rx_ring
    - `I have found how to use double threads for different logical cores` this is the example of `simple_ring` 
    -

- [] combine pass-through example which is a low level interface of FUSE with DPDK and POSIX
- [] just compile FUSE with DPDK and FUSE with POSIX (without synchronization) and compare the performance
- [] identify whether the performance degrades from synchronization between DPDK and FUSE or the network latency.
-

https://doc.dpdk.org/guides-16.04/sample_app_ug/performance_thread.html
