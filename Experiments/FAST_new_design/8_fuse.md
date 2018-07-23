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

[0] recv msg in DPDK ::  424
[1] recv msg in DPDK ::  448
[2] recv msg in DPDK ::  469
[3] recv msg in DPDK ::  501
[4] recv msg in DPDK ::  886
[5] recv msg in DPDK ::  962
[6] recv msg in DPDK ::  988
[7] recv msg in DPDK ::  1012
[8] recv msg in DPDK ::  1041
[9] recv msg in DPDK ::  1125
[10] recv msg in DPDK :: 1838


[0] recv msg in FUSE: 26 :: 483
[1] recv msg in FUSE: 26 :: 499
[2] recv msg in FUSE: 26 :: 499
[3] recv msg in FUSE: 26 :: 572
[4] recv msg in FUSE: 26 :: 920
[5] recv msg in FUSE: 26 :: 989
[6] recv msg in FUSE: 26 :: 1045
[7] recv msg in FUSE: 26 :: 1057
[8] recv msg in FUSE: 26 :: 1057
[9] recv msg in FUSE: 26 :: 1169
[10] recv msg in FUSE: 26 :: 1859
