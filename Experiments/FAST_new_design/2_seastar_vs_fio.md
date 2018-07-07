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



### What is this IO_tester?
- this part is using filesystem
- https://github.com/scylladb/seastar/blob/master/apps/io_tester/io_tester.cc
-
