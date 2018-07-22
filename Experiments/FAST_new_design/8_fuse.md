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
17833
17911
17993
18078
18162
18245
18328
18412
18496
18580
18663

87928
87925
87925
87927
87927
87926
87927
87912
87911
87924
87921



```
