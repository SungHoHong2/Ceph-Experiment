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



```
default_cpp_dialect='gnu++1yi -fPIC
make[2]: Entering directory '/home/sungho/seastar/build/release/c-ares


CMakeFiles/c-ares.dir/ares_gethostbyname.c.o: /home/sungho/seastar/c-ares/ares_gethostbyname.c


/home/sungho/seastar/build/release/c-ares
CMakeFiles/c-ares.dir/all:
        $(MAKE) -f CMakeFiles/c-ares.dir/build.make CMakeFiles/c-ares.dir/depend
        $(MAKE) -f CMakeFiles/c-ares.dir/build.make CMakeFiles/c-ares.dir/build
        @$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --progress-dir=/home/sungho/seastar/build
/release/c-ares/CMakeFiles --progress-num=16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64 "Built target c-ares"


make -f CMakeFiles/Makefile2 all
make -f CMakeFiles/c-ares.dir/build.make CMakeFiles/c-ares.dir/depend
cd /home/sungho/seastar/build/release/c-ares && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/seastar/c-ares /home/sungho/seastar/c-ares /home/sungho/seastar/build/release/c-ares /home/sungho/seastar/build/release/c-ares /home/sungho/seastar/build/release/c-ares/CMakeFiles/c-ares.dir/DependInfo.cmake --color=

make -f CMakeFiles/c-ares.dir/build.make CMakeFiles/c-ares.dir/build
/usr/bin/cmake -E cmake_echo_color --switch= --progress-dir=/home/sungho/seastar/build/release/c-ares/CMa
keFiles --progress-num=16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,4
3,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64 "Built target c-ares"






@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sungho/seasta
r/build/release/c-ares/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/c-ares
.dir/ares_gethostbyaddr.c.o"
/usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/c-ares.dir/ares_gethostbyaddr.c.o  -c /home/sungho/seastar/c-ares/ares_gethostbyaddr.c

```



### debug the configure.py

```
```
