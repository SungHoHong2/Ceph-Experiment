### Comparing the latency between seastar and FIO
- the workstation will be tested by me seastar(Intel), Filesytem (ext4)
- the server will be tested by wenji seastar(Mellanox) Filesystem (xtf)
- the goal is to find out whether seastar is faster than the speed of the filesystem
- need to compare write and read

- the storage will communicate locally
  - 1 thread, 1 depth
- the cache will communicate to remote and local
  - 1 thread

### need to be sure how FIO generates the read and write


### Seastar Test
- pingpnog
- RX (server) - currently w2 will be the server
- TX (server) - currently w2 will be the server


### What is this IO_tester?
- this part is using filesystem
- https://github.com/scylladb/seastar/blob/master/apps/io_tester/io_tester.cc


### 2nd version
- IO latency is the latency for the single IO
- IO latency should be collected in
```
 lat (usec): min=384, max=92280, avg=2968.69, stdev=13764.82
```
- and seastar also needs to caculate the time based on this. (workstation 1G)
- additionally after this we can test it also in the server server 10G


### Fuse-related to cache
- https://github.com/ibizaman/pcachefs
- https://sourceforge.net/projects/fuse-cache/
- https://github.com/Doloops/mcachefs




### Test Configuration
sungho@w1:~$ lsblk
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT
sda      8:0    0   477G  0 disk
|-sda1   8:1    0   457G  0 part /
|-sda2   8:2    0     1K  0 part
`-sda5   8:5    0  19.9G  0 part [SWAP]
sdb      8:16   0 931.5G  0 disk
|-sdb1   8:17   0   9.3G  0 part /mnt/osd-device-0-data
|-sdb2   8:18   0   1.9G  0 part
|-sdb3   8:19   0   9.3G  0 part
|-sdb4   8:20   0 165.8G  0 part
|-sdb5   8:21   0   9.3G  0 part /mnt/osd-device-1-data
|-sdb6   8:22   0   1.9G  0 part
|-sdb7   8:23   0   9.3G  0 part
`-sdb8   8:24   0 724.8G  0 part
sdc      8:32   0  58.7G  0 disk
