### list
- [x] first test pure DPDK and POSIX and confirm that the performance is better than POSIX
  - [] performance using DPDK with one logical core (echo)
  - [x] performance using DPDK + two pthreads are worse than POSIX

- [] combine pass-through example which is a low level interface of FUSE with DPDK and POSIX
- [] just compile FUSE with DPDK and FUSE with POSIX (without synchronization) and compare the performance
- [] identify whether the performance degrades from synchronization between DPDK and FUSE or the network latency.
-

### results
```
23997
23998
23999
24000
24000
24016
24017
24017
24018
24020
24020

107925
107921
107922
107920
107920
107918
107904
107903
107902
107901
107896
```
