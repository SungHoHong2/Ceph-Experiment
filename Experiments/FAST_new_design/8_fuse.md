### list
- [x] first test pure DPDK and POSIX and confirm that the performance is better than POSIX
  - [x] performance using DPDK with one logical core (echo)
    - it seems communication does not work well with pthread and logical cores
  - [x] performance using DPDK + two pthreads are worse than POSIX
    - `fun fact I found was that when POSIX is used with mutex then the threads works collaboratively`
    - `in case of DPDK teh threads work black and white`

- [x] use rx_ring
    - `I have found how to use double threads for different logical cores` this is the example of `simple_ring`

- [x] **combine pass-through example which is a low level interface of FUSE with DPDK and POSIX**
    - [x] use pass through
    - [x] read request and then wait until it gets the data
    - [x] collect the latency

- [ ] **server side**
    - [x] find a special sudo command that may run FUSE in root
    - [x] find some process that does not have the sudo permission that is run by FUSE
    - [x] solve the situation where dpdk can work with FUSE
    - [x] Mellanox Connect X-3 is not working
    - [x] dpdk is not sending the data
    - [ ] get the cache hit results
    - [ ] get the non-remote storage result




### the correct data
00000000: E4 1D 2D D9 CB 82 E4 1D 2D D9 BF B1 46 72 6F 6D | ..-.....-...From
00000000: E4 1D 2D D9 CB 82 E4 1D 2D D9 BF B1 00 00 48 65 | ..-.....-.....He

516.719971
67.566580

### installing Mellanox driver
x86_64
libibumad3 ucx libibmad5 libopensm5 librxe-1 librxe-dev librxe-1-dbg
https://community.mellanox.com/docs/DOC-2197

run MLNX_OFED_SRC-4.2-1.2.0.0
- this will not install properly it gets stuck in openmpi or something but it will finish the install
- and then the os will ask for installing ibstat
- after this run `ibstat` and check if the mellanox driver is running




ofed_info -s
https://www.dell.com/support/home/us/en/04/drivers/driversdetails?driverid=4mvxh


### installing libfuse
```
# meson install
sudocmd apt-get install python3 ninja-build
sudocmd apt install python3-pip
export LC_ALL=C

get-pip.py download
python3.5 get-pip.py

pip3 install --user meson
export PATH=$PATH:/home/sungho/.local/bin/
export PATH=$PATH:/home/users/sungho/.local/bin/

# install libfuse
git clone https://github.com/libfuse/libfuse.git
mkdir build; cd build
meson ..

ninja
wget https://github.com/ninja-build/ninja/releases/download/v1.5.3/ninja-linux.zip

sudo ln -s /home/sungho/libfuse/build/lib/libfuse3.so.3.2.4 /lib/x86_64-linux-gnu/libfuse3.so.3
gcc -Wall passthrough.c -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread -o passthrough
./passthrough -f /mnt/ssd_cache
```

gcc passthrough.c `pkg-config fuse3 --cflags --libs`  -o passthrough

### producing good comparison results between DPDK and POSIX
- if FUSE is super slow and dpdk and posix is responsible for very portion of the latency, then there is no need to use DPDK.
- the filesystem that I can use is nvme0n1p4 on both c3n24 and c3n25
- the library that I have to use is the paththrough provided by FUSE library
