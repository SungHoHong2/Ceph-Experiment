### list
- [] first test pure DPDK and POSIX and confirm that the performance is better than POSIX
- [] combine pass-through example which is a low level interface of FUSE with DPDK and POSIX
- [] just compile FUSE with DPDK and FUSE with POSIX (without synchronization) and compare the performance
- [] identify whether the performance degrades from synchronization between DPDK and FUSE or the network latency.
-

### results
```
[0] recv msg in FUSE: 26 :: 43924 :: 11
[1] recv msg in FUSE: 26 :: 43920 :: 11
[2] recv msg in FUSE: 26 :: 43921 :: 11
[3] recv msg in FUSE: 26 :: 43920 :: 11
[4] recv msg in FUSE: 26 :: 43920 :: 11
[5] recv msg in FUSE: 26 :: 43918 :: 11
[6] recv msg in FUSE: 26 :: 43904 :: 11
[7] recv msg in FUSE: 26 :: 43903 :: 11
[8] recv msg in FUSE: 26 :: 43901 :: 11
[9] recv msg in FUSE: 26 :: 43898 :: 11
[10] recv msg in FUSE: 26 :: 43897 :: 11
```
