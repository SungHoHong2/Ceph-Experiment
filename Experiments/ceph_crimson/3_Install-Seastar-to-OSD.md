### makefile log
- I think the third part is the one atht will allow you `integrate Seastar`
- this is the result from `make ceph-osd -j`
- if the design is indeed centralized and the OSDs are only 1 : 1 relationship,
- then simply implementing Sestar with client or server is suffice
- use `sudocmd ceph_seastar_msg custom_makefile_test`

### integrating Seastar with Ceph
- use the common header file

### found issues with the BOOST version
- boost_1_67_0 is used by ceph
- boost 1.58.0 is used by seastar
- need to find a match

### bug found on the boost library in Ceph
- include/boost/lockfree/spsc_queue.hpp
- Add #include <boost/next_prior.hpp>; no longer in utility.hpp

```

```




### Need to recompile Seastar to work with shared libraries
- modify `build.ninja`
- recompile Seastar with -fPIC
- recompile Seastar-DPDK with -fPIC
  - [update] mk/internal/rte.build-pre.mk
  - [update] /home/sungho/seastar/dpdk/drivers/net/vmxnet3/Makefile
  - [update] /home/sungho/seastar/dpdk/drivers/net/i40e/Makefile
  - [compile] /home/sungho/seastar/build/dpdk/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/ixgbe/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/e1000/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/ring/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/bnxt/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/cxgbe/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/ena/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/enic/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/af_packet/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/ark/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/avp/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/bnx2x/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/bonding/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/dpaa/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/dpaa2/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/failsafe/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/fm10k/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/kni/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/liquidio/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/mlx4/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/mlx5/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/nfp/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/null/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/pcap/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/qede/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/sfc/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/szedata2/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/thunderx/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/tap/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/vhost/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/virtio/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/vmxnet3/Makefile
  - /home/sungho/seastar/dpdk/drivers/net/xenvirt/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_hash/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_kvargs/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_acl/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_bitratestats/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_cfgfile/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_cmdline/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_cryptodev/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_distributor/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_efd/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_ether/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_eventdev/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_ip_frag/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_jobstats/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_kni/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_latencystats/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_lpm/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_mbuf/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_mempool/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_meter/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_metrics/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_pdump/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_pipeline/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_port/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_power/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_reorder/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_ring/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_sched/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_table/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_timer/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_vhost/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_eal/linuxapp/eal/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_eal/linuxapp/igb_uio/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_eal/linuxapp/kni/Makefile
  - /home/sungho/seastar/dpdk/lib/librte_mempool/Makefile
  - /home/sungho/seastar/dpdk/drivers/mempool/ring/Makefile


  ### Problem with shared-libraries combined with Ceph.
  - /home/sungho/seastar/build/release/libcares-seastar.a  needs be compiled with -fPIC
  - [compile] build.ninja -j 4
  - build/release/c-ares/Makefile : carescmake_release c-ares
  - CMakeFiles/c-ares.dir/build.make CMakeFiles/c-ares.dir/ares_gethostbyaddr.c.o
  - build/release/c-ares/CMakeFiles
  - need to compile seastar library with `-fPIC`

  - /home/sungho/seastar/build/release/c-ares/CMakeFiles/c-ares.dir/build.make

  [ceph compile] cd /home/sungho/ceph/build/src
  cmake -E cmake_link_script CMakeFiles/ceph-osd.dir/link.txt --verbose=

  [seastar compile]
  vi /home/sungho/seastar/build/release/c-ares/CMakeFiles/c-ares.dir/build.make



### debug the configure.py
- https://github.com/scylladb/seastar/issues/467
- https://github.com/scylladb/seastar/commit/f6d0de0ad4634cbf28e54cf0d5862ef9e8eab5ba
- add set (CMAKE_POSITION_INDEPENDENT_CODE TRUE) at the start of c-ares/CMakeLists.txt
- ./configure.py --enable-dpdk --compiler g++-5 --mode release --disable-hwloc --cflags="-DSEASTAR_DEFAULT_ALLOCATOR -fPIC"


### duplicated usage of DPDK in Seastar and Ceph
- Directed Ceph to use Seastar DPDK

```
sudo ./configure.py --enable-dpdk --compiler g++-5
sudo ./configure.py --enable-dpdk --compiler g++-5 --mode release --cflags="-fPIC"
sudo ./configure.py --so --enable-dpdk --compiler g++-5 --mode release --cflags="-fPIC"
./configure.py --enable-dpdk --compiler g++-5 --mode release --disable-hwloc --cflags="-DSEASTAR_DEFAULT_ALLOCATOR -fPIC"
./configure.py --compiler g++-5 --mode release --disable-hwloc --cflags="-DSEASTAR_DEFAULT_ALLOCATOR -fPIC"


problem is we don't support building seastar as a shared library. That option is deprecated and should be removed.
deprecated version of seastar only supports shared-library version
moving on tot he commit
```

### removing the files from SPDK
- `/home/sungho/ceph/src/spdk`
- and `make` the file `after removing the unwanted dpdk functions`
- if the compilation with just screwing up the spdk works well, then it is definitely `Seastar problem`
- However, if the same error shows up then it is not the Seastar problem it is the `implementation problem.`
- it turned out `Seastar is fucking up` the Ceph.
- Exactly which part... I will find out

**the problem starts from the link**

[0] get the backup verision of the link.txt
[1] confirm whether the boost library is causing the runtime error
[2] I need to make sure whether `different version` of boost library is making the error
    - try out using the same version of boostv **the final straw**
    - but I don't agree with this part because isn't the boost library just using as import?
    - `yes in seastar it is fine, but we don't know whether it is okay with Ceph.`


[X] If this part does not work we definitely need to try out the *Crimson...*
    - because that part for sure they know how to compile it
    - the crimson didnot actually intgerated Ceph with Seastar
    - It is testing the Seastar by bringing some of the Ceph message stack


**experiment**
- after changing this part it is the cause of the error!
- link.txt is the source of the error

```
/home/sungho/ceph/build/src/CMakeFiles/ceph-osd.dir/link.txt
/home/sungho/seastar/configure.py
/home/sungho/ceph/src/os/bluestore/NVMEDevice.cc

this part is the error
rc/tcmalloc.cc:278] Attempt to free invalid pointer 0x60000005efc0
```


### pointer error after compilation
- the main cause happens with introudcing seastar library.
- lets specifically find out which part actually causes the fatal blow
- https://stackoverflow.com/questions/20297524/c-free-invalid-pointer
- one reason for this is that Seastar and Ceph are invoking `different DPDK`.
- let us try without the `DPDK in Seastar` and combine it to the link.txt and the makefile
  - DPDK is not the issue.
  - The only issue should be the boost library
  - Right now the two are sharing different boost library
  - Issue is `evident` because different versions does not contain the right ones.

  ./configure.py --compiler g++-5 --mode release  --cflags="-DSEASTAR_DEFAULT_ALLOCATOR -fPIC"

```
-lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread
../boost/lib/libboost_thread.a ../boost/lib/libboost_chrono.a ../boost/lib/libboost_atomic.a ../boost/lib/libboost_system.a ../boost/lib/libboost_random.a ../boost/lib/libboost_program_options.a

```


### updating the do_cmake.sh and cmakelist.txt
- there is an interference with the memory profiling
- I am trying it out by removing the memory profiling

- it will be good to avoid `spdk` `google-perftools`
  - find_package(Boost 1.67 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
  - remove spdk within configure.py
  - option(WITH_DPDK "Enable DPDK messaging" OFF)
  - set(TCMALLOC_srcs perfglue/disabled_heap_profiler.cc)

```
export LC_ALL=C
./do_cmake.sh -DWITH_MGR_DASHBOARD_FRONTEND=OFF -DWITH_DPDK=OFF -DWITH_SPDK=OFF
./do_cmake.sh -DWITH_MGR_DASHBOARD_FRONTEND=OFF -DWITH_DPDK=OFF -DWITH_SPDK=OFF -DWITH_SEASTAR=ON

modified the CMakelist.txt
set(ALLOCATOR "libc")
build_boost(1.67 -> build_boost(1.58



```
