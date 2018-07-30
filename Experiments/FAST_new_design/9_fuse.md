### First Basic Results of Cache-Miss and Cache-Hit
- [ ] Run the device on NVM-e
- [ ] User direct-io *having trouble with configuring ssd*
- [ ] use HDD in client
- [ ] `send data 4kb per message`
    - [x] the mtu size seems not be working in the server
    - [x] `just send multiple same sized packets just send them four times`.
    - [ ] find out why the file size is keep increasing for some reason..



**Jumbo Frames**
https://software.intel.com/en-us/articles/jumbo-frames-in-open-vswitch-with-dpdk
sudocmd ip link set p5p2 mtu 9000
ip link show | grep mtu



### Second Modify DPDK favorable to GDCache
- [ ] Fuse working with low-level pass through
- [ ] Test the performance using FIO.
