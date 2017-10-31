### Prerequisite

- **when the dpdk does not find librados related file**
    - change the parameter into n
```
$ vim common_base
./dpdk/config/common_base:198:CONFIG_RTE_LIBRTE_MLX4_PMD=n
./dpdk/config/common_base:199:CONFIG_RTE_LIBRTE_MLX4_DEBUG=n
./dpdk/config/common_base:200:CONFIG_RTE_LIBRTE_MLX4_SGE_WR_N=4
./dpdk/config/common_base:201:CONFIG_RTE_LIBRTE_MLX4_MAX_INLINE=0
./dpdk/config/common_base:202:CONFIG_RTE_LIBRTE_MLX4_TX_MP_CACHE=8
./dpdk/config/common_base:203:CONFIG_RTE_LIBRTE_MLX4_SOFT_COUNTERS=1
./dpdk/config/common_base:206:# Compile burst-oriented Mellanox ConnectX-4 (MLX5) PMD
./dpdk/config/common_base:208:CONFIG_RTE_LIBRTE_MLX5_PMD=n
./dpdk/config/common_base:209:CONFIG_RTE_LIBRTE_MLX5_DEBUG=n
./dpdk/config/common_base:210:CONFIG_RTE_LIBRTE_MLX5_TX_MP_CACHE=8
```
<br>

### Install DPDK

- **install location**
```
$ cd ceph-12.2.0/src/dpdk-17
$ sudo make install T=x86_64-native-linuxapp-gcc DESTDIR=/usr/local
```

- **install**
```
# prerequisites
  $ install vim
  $ install gcc-multilib, libnuma-dev

# install dpdk
  $ make config T=x86_64-native-linuxapp-gcc
  $ make
	$ make install T=x86_64-native-linuxapp-gcc

# install kernel modules
	$ sudo modprobe uio
	$ sudo insmod build/kmod/igb_uio.ko

# configure hugepages for dpdk
	$ echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
  $ sudo mkdir /mnt/huge
  $ sudo mount -t hugetlbfs nodev /mnt/huge

# Bind network adaptor
	$ sudo ifconfig ens3 down
	$ sudo ./usertools/dpdk-devbind.py --bind=igb_uio ens3

# Setup the environment
  $ vi etc/profile
	$ export RTE_SDK=/home/sungho/ceph-12.2.0/src/dpdk-17
	$ export DESTDIR=/usr/local
	$ export RTE_TARGET=x86_64-native-linuxapp-gcc
	$ ln -s $RTE_SDK/build $RTE_SDK/x86_64-default-linuxapp-gcc

# if it does not work
  $ make install T=$RTE_TARGET
```
