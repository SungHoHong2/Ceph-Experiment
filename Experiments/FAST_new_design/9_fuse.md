### First Basic Results of Cache-Miss and Cache-Hit
- [x] Run the device on NVM-e
- [x] User direct-io *having trouble with configuring ssd*
- [x] use HDD in client
- [x] `send data 4kb per message`
    - [x] the mtu size seems not be working in the server
    - [x] `just send multiple same sized packets just send them four times`.
    - [x] find out why the file size is keep increasing for some reason..

- [x] modify the cache-miss
- [x] change the capitals of the variables to non-capitals
- [x] develop the POSIX replica
- [ ] refine version for the progress chart

**Jumbo Frames**
https://software.intel.com/en-us/articles/jumbo-frames-in-open-vswitch-with-dpdk
sudocmd ip link set p5p2 mtu 9000
ip link show | grep mtu


### Second Modify DPDK favorable to GDCache
- [ ] Fuse working with low-level pass through
- [ ] Test the performance using FIO.


Hey Wenji,
I am having trouble using the direct-IO in order to test the performance of the SSD in the ASU cluster.

I have tested the read and write on the
/dev/nvme0n1p1 in ASU cluster c3n24

However, I have found out that using direct-IO fails to read and write for my implementation.

Here is the basic stuff that I have tested below
https://github.com/SungHoHong2/Ceph-Experiment/blob/master/DPDK-FUSE/RAW_block/example.c

One of the reason I suspect is from this phrase from the stack overflow.

Direct DMA transfer typically requires the buffer to be aligned
The O_DIRECT flag may impose alignment restrictions on the length and address of userspace buffers and the file offset of I/Os. ... Under Linux 2.6, alignment to 512-byte boundaries suffices.

https://stackoverflow.com/questions/10996539/read-from-a-hdd-with-o-direct-fails-with-22-einval-invalid-argument

Do you think this problem can be fixed easily? or do you think I should just find a alternative route such as generating the strings randomly in order to avoid configuring the block device?
