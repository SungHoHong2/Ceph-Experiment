### producing good comparison results between DPDK and POSIX
- if FUSE is super slow and dpdk and posix is responsible for very portion of the latency, then there is no need to use DPDK.
- the filesystem that I can use is nvme0n1p4 on both c3n24 and c3n25
- the library that I have to use is the paththrough provided by FUSE library
