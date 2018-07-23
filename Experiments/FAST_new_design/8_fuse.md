### list
- [x] first test pure DPDK and POSIX and confirm that the performance is better than POSIX
  - [x] performance using DPDK with one logical core (echo)
    - it seems communication does not work well with pthread and logical cores
  - [x] performance using DPDK + two pthreads are worse than POSIX
    - `fun fact I found was that when POSIX is used with mutex then the threads works collaboratively`
    - `in case of DPDK teh threads work black and white`

- [x] use rx_ring
    - `I have found how to use double threads for different logical cores` this is the example of `simple_ring`

- [ ] **combine pass-through example which is a low level interface of FUSE with DPDK and POSIX**
    - [ ] use pass through
    - [ ] read request and then wait until it gets the data
    - [ ] collect the latency


### installing libfuse
```
# meson install
sudo apt-get install python3 ninja-build
sudo apt install python3-pip
export LC_ALL=C
pip3 install --user meson
export PATH=$PATH:/home/sungho/.local/bin/

# install libfuse
mkdir build; cd build
meson ..
ninja
sudo ln -s /home/sungho/libfuse/build/lib/libfuse3.so.3.2.4 /lib/x86_64-linux-gnu/libfuse3.so.3


cd example
gcc -Wall passthrough.c `pkg-config fuse3 --cflags --libs` -o passthrough
gcc -Wall passthrough.c -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread -o passthrough

./passthrough -f /mnt/ssd_cache

```



### producing good comparison results between DPDK and POSIX
- if FUSE is super slow and dpdk and posix is responsible for very portion of the latency, then there is no need to use DPDK.
- the filesystem that I can use is nvme0n1p4 on both c3n24 and c3n25
- the library that I have to use is the paththrough provided by FUSE library
