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
    - [x] use pass through
    - [x] read request and then wait until it gets the data
    - [ ] collect the latency

- [ ] **first run the DPDK application pingpong to see how the port is actually used 





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

`pkg-config fuse3 --cflags --libs`

### producing good comparison results between DPDK and POSIX
- if FUSE is super slow and dpdk and posix is responsible for very portion of the latency, then there is no need to use DPDK.
- the filesystem that I can use is nvme0n1p4 on both c3n24 and c3n25
- the library that I have to use is the paththrough provided by FUSE library
