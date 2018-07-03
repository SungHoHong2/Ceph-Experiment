HOSTS="$(cat /etc/hostname)"
HISTORY="3_osd_seastar"
HISTORY_SEASTAR_MSG="3_seastar_msg"


if [ "$1" = "host_to_git" ]
then
rm -rf history/$HISTORY
mkdir history/$HISTORY
# cp /Users/sunghohong/Documents/Projects/ceph/src/Makefile src/Makefile
cp /Users/sunghohong/Documents/Projects/ceph/src/ceph_osd.cc                    src/
cp /Users/sunghohong/Documents/Projects/ceph/src/osd/OSD.h                      src/
cp /Users/sunghohong/Documents/Projects/ceph/src/msg/simple/SimpleMessenger.cc  src/
cp /Users/sunghohong/Documents/Projects/ceph/src/os/bluestore/NVMEDevice.cc     src/

cp src/* history/$HISTORY
cd /Users/sunghohong/Documents/Projects/Ceph-Experiment
git add .
git commit -m '$HISTORY'
git push origin master


# sudocmd ceph_seastar_msg git_to_server
elif [ "$1" = "git_to_server" ]
then
git pull origin master
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/ceph_osd.cc               /home/sungho/ceph/src/ceph_osd.cc
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/OSD.h                     /home/sungho/ceph/src/osd/OSD.h
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/SimpleMessenger.cc        /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc


      if [ "$HOSTS" = "w2" ]
      then
      # export LC_ALL=C
      # you must! and must! only use sudo in the make install!!!
      # this part is used and not used sometimes
      cd /home/sungho/ceph/build
      make ceph-osd -j 8 -n && make install -j 8

      kill -9 `pidof ceph-osd`
      ceph-osd -i 0 -c /etc/ceph/ceph.conf

      elif [ "$HOSTS" = "wenji-w1" ]
      then
      kill -9 `pidof ceph-mon`
      kill -9 `pidof ceph-mgr`
      ceph-mon -i wenji -c /etc/ceph/ceph.conf
      ceph-mgr -i 0 -c /etc/ceph/ceph.conf
      fi


# sudocmd ceph_seastar_msg custom_makefile_test
elif [ "$1" = "custom_makefile_test" ]
then
SEASTAR_LIBRARY='-std=gnu++17 -fPIC -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DHAVE_DPDK -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -O2 -I/home/sungho/seastar -I/home/sungho/seastar/build/dpdk/include -I/home/sungho/seastar/fmt -I/home/sungho/seastar/build/release/gen -I/home/sungho/seastar/build/release/c-ares -L/home/sungho/seastar/build/release -L/home/sungho/seastar/build/dpdk/lib -Wl,--whole-archive,-lseastar,--no-whole-archive -std=gnu++17 -g -fvisibility=hidden -pthread -I/home/sungho/seastar -U_FORTIFY_SOURCE -DHAVE_DPDK -I/home/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -I/home/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -Wl,--no-as-needed -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind -lyaml-cpp -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000 -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar'
SEASTAR_LIBRARY='-fPIC -std=gnu++17 -g -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DSEASTAR_DEFAULT_ALLOCATOR -fPIC -DBOOST_TEST_DYN_LINK -fPIC -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -fPIC -O2 -fPIC -I/home/sungho/seastar -I/home/sungho/seastar/fmt -I/home/sungho/seastar/build/release/gen -I/home/sungho/seastar/build/release/c-ares -L/home/sungho/seastar/build/release -Wl,--whole-archive,-lseastar,--no-whole-archive -fPIC -std=gnu++17 -g -fvisibility=hidden -pthread -I/home/sungho/seastar -U_FORTIFY_SOURCE -DSEASTAR_DEFAULT_ALLOCATOR -fPIC -I/home/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -fPIC -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -Wl,--no-as-needed -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind -lyaml-cpp -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar
'
# SEASTAR_LIBRARY=''

echo "SEASTAR MAKEFILE BEGIN"
echo "run the install_depths.sh whenever your rerun your computer"
git pull origin master
cd /home/sungho/ceph/build/
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/ceph_osd.cc               /home/sungho/ceph/src/ceph_osd.cc
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/OSD.h                     /home/sungho/ceph/src/osd/OSD.h
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/SimpleMessenger.cc        /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/spsc_queue.hpp            /home/sungho/ceph/build/boost/include/boost/lockfree/spsc_queue.hpp
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/spsc_queue.hpp            /home/sungho/ceph/build/boost/src/Boost/boost/lockfree/spsc_queue.hpp
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/link.txt                  /home/sungho/ceph/build/src/CMakeFiles/ceph-osd.dir/link.txt
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/configure.py              /home/sungho/seastar/configure.py
# cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/NVMEDevice.cc             /home/sungho/ceph/src/os/bluestore/NVMEDevice.cc

# make -f CMakeFiles/Makefile2 ceph-osd -j 4
# make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/depend -j 4
# make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build  -j 4
# /usr/bin/cmake -E cmake_echo_color --switch= --green --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num=3 "Building CXX object src/CMakeFiles/ceph-osd.dir/ceph_osd.cc.o" -j 4
# cd /home/sungho/ceph/src/spdk
# make

# cd /home/sungho/ceph/build/src && /usr/bin/c++ -g -DCEPH_LIBDIR=\"/usr/local/lib\" -DCEPH_PKGLIBDIR=\"/usr/local/lib/ceph\" -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D__linux__ -isystem /home/sungho/ceph/build/boost/include -I/home/sungho/ceph/build/src/include -I/home/sungho/ceph/src -isystem /home/sungho/ceph/build/include -I/usr/include/nss -I/usr/include/nspr -isystem /home/sungho/ceph/src/xxHash -isystem /home/sungho/ceph/src/rapidjson/include -I/home/sungho/ceph/src/dmclock/src -I/home/sungho/ceph/src/dmclock/support/src -isystem /home/sungho/ceph/src/rocksdb/include -fno-strict-aliasing -fsigned-char -rdynamic  -ftemplate-depth-1024 -fno-new-ttp-matching -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fstack-protector-strong -std=c++17 -fdiagnostics-color=auto -I/usr/include -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -O2 -fPIE -DHAVE_CONFIG_H -D__CEPH__ -D_REENTRANT -D_THREAD_SAFE -D__STDC_FORMAT_MACROS $SEASTAR_LIBRARY -o CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -c /home/sungho/ceph/src/ceph_osd.cc
#
# # /usr/bin/cmake -E cmake_echo_color --switch= --green --bold --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num= "Linking CXX executable ../bin/ceph-osd"
# cd /home/sungho/ceph/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/ceph-osd.dir/link.txt --verbose=
# # /usr/bin/cmake -E cmake_echo_color --switch= --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num=3 "Built target ceph-osd"
# # /usr/bin/cmake -E cmake_progress_start /home/sungho/ceph/build/CMakeFiles 0


# cd /home/sungho/ceph/build/src && /usr/bin/c++ -nostartfiles -g  -fPIC $SEASTAR_LIBRARY -o CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -c /home/sungho/ceph/src/ceph_osd.cc

cd /home/sungho/ceph/build/src && /usr/bin/c++ -g -DCEPH_LIBDIR=\"/usr/local/lib\" -DCEPH_PKGLIBDIR=\"/usr/local/lib/ceph\" -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D__linux__ -isystem /home/sungho/ceph/build/boost/include -I/home/sungho/ceph/build/src/include -I/home/sungho/ceph/src -isystem /home/sungho/ceph/build/include -I/usr/include/nss -I/usr/include/nspr -isystem /home/sungho/ceph/src/xxHash -isystem /home/sungho/ceph/src/rapidjson/include -I/home/sungho/ceph/src/dmclock/src -I/home/sungho/ceph/src/dmclock/support/src -isystem /home/sungho/ceph/src/rocksdb/include -fno-strict-aliasing -fsigned-char -rdynamic  -ftemplate-depth-1024 -fno-new-ttp-matching -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fstack-protector-strong -std=c++17 -fdiagnostics-color=auto -I/usr/include -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -O2 -fPIE -DHAVE_CONFIG_H -D__CEPH__ -D_REENTRANT -D_THREAD_SAFE -D__STDC_FORMAT_MACROS $SEASTAR_LIBRARY -o CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -c /home/sungho/ceph/src/ceph_osd.cc

# c++ -fPIC -std=gnu++17 -g -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DSEASTAR_DEFAULT_ALLOCATOR -fPIC -DBOOST_TEST_DYN_LINK -fPIC -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -fPIC -O2 -fPIC -I/home/sungho/seastar -I/home/sungho/seastar/fmt -I/home/sungho/seastar/build/release/gen -I/home/sungho/seastar/build/release/c-ares -L/home/sungho/seastar/build/release -Wl,--whole-archive,-lseastar,--no-whole-archive -fPIC -std=gnu++17 -g -fvisibility=hidden -pthread -I/home/sungho/seastar -U_FORTIFY_SOURCE -DSEASTAR_DEFAULT_ALLOCATOR -fPIC -I/home/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -fPIC -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -Wl,--no-as-needed -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind -lyaml-cpp -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -o /home/sungho/ceph/build/bin/ceph-osd

 c++ -std=c++17 $SEASTAR_LIBRARY -fdiagnostics-color=auto -I/usr/include -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -O2 -g    -pie CMakeFiles/ceph-osd.dir/ceph_osd.cc.o  -o ../bin/ceph-osd   -lblkid -libverbs -lfuse ../lib/libdmclock.a -laio -lfuse  -lrt  -luuid ../lib/libkv.a -lleveldb -ltcmalloc rocksdb/librocksdb.a -lsnappy -llz4 ../lib/libcommon.a ../lib/libjson_spirit.a ../lib/libcommon_utf8.a ../lib/liberasure_code.a -lresolv -lblkid ../boost/lib/libboost_thread.a ../boost/lib/libboost_chrono.a ../boost/lib/libboost_atomic.a ../boost/lib/libboost_system.a ../boost/lib/libboost_random.a ../boost/lib/libboost_program_options.a ../boost/lib/libboost_date_time.a ../boost/lib/libboost_iostreams.a ../boost/lib/libboost_regex.a -lssl3 -lsmime3 -lnss3 -lnssutil3 -lplds4 -lplc4 -lnspr4 -lssl -lcrypto -lpthread -ldl -ludev -libverbs -lz -Wl,-rpath,/home/sungho/ceph/build/lib:

# /usr/bin/cmake -E cmake_echo_color --switch= --green --bold --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num= "Linking CXX executable ../bin/ceph-osd"
# cd /home/sungho/ceph/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/ceph-osd.dir/link.txt --verbose=


echo "SEASTAR MAKEFILE END"

elif [ "$1" = "custom_osd_run" ]
then
kill -9 `pidof ceph-osd`
cd /home/sungho/ceph/build/bin
/home/sungho/ceph/build/bin/ceph-osd -i 0 -c /etc/ceph/ceph.conf



elif [ "$1" = "seastar_msg_to_git" ]
then
rm -rf history/$HISTORY_SEASTAR_MSG
cp -R seastar-msg history/$HISTORY_SEASTAR_MSG
git add .
git commit -m 'seastar_msg_to_git'
git push origin master


# sudocmd ceph_seastar_msg seastar_msg_install
elif [ "$1" = "seastar_msg_install" ]
then
git pull origin master
    if [ "$HOSTS" = "w2" ]
    then
    cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    g++-5  `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` seastar-msg.cc -o seastar-msg
    # ./seastar-msg/seastar-msg --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1

    elif [ "$HOSTS" = "wenji-w1" ]
    then
    cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    # g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` client.cc -o client
    g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` seastar-msg.cc -o seastar-msg
    # ./seastar-msg/seastar-msg --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1

    elif [ "$HOSTS" = "c3n24" ]
    then
      cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
      g++-5 -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DHAVE_DPDK -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -O2 -I/data1/sungho/seastar -I/data1/sungho/seastar/build/dpdk/include -I/data1/sungho/seastar/fmt -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares  -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -Wl,--no-as-needed -Wl,--whole-archive  -Wl,--no-whole-archive -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/dpdk/lib -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lboost_thread -lcryptopp -lgnutls -lgnutlsxx -llz4 -lprotobuf -lgcc_s -lunwind -lyaml-cpp -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000  -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar seastar-msg.cc -o seastar-msg


    elif [ "$HOSTS" = "c3n25" ]
    then
      cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
      g++-5 -std=gnu++1y -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/release/fmt/fmt -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA  -Wl,--no-as-needed     -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind  -lyaml-cpp -L/data1/sungho/seastar/build/dpdk/lib -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000 -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA -O2  -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares seastar-msg.cc -o seastar-msg
    fi

# sudocmd ceph_seastar_msg seastar_msg_run
elif [ "$1" = "seastar_msg_run" ]
then

  if [ "$HOSTS" = "w2" ]
  then
  cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
  /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg/seastar-msg --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0

  elif [ "$HOSTS" = "wenji-w1" ]
  then
  cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
  /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg/seastar-msg --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0

  elif [ "$HOSTS" = "c3n24" ]
  then
    cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg/seastar-msg --dpdk-pmd --dpdk-port-index 1 --network-stack native --dhcp 0 --host-ipv4-addr 10.107.30.40 --netmask-ipv4-addr 255.255.254.0 --gw-ipv4-addr 10.107.30.1 --collectd 0 --server "10.107.30.41:1234"

  elif [ "$HOSTS" = "c3n25" ]
  then
    cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg/seastar-msg --dpdk-pmd --dpdk-port-index 1 --network-stack native --dhcp 0 --host-ipv4-addr 10.107.30.41 --netmask-ipv4-addr 255.255.254.0 --gw-ipv4-addr 10.107.30.1 --collectd 0 --server "10.107.30.40:1234"
  fi

elif [ "$HOSTS" = "ceph-install" ]
then

kill -9 `pidof seastar-msg`

git clone https://github.com/SungHoHong2/ceph.git

make ceph-common -j 8 && make ceph-osd -j 8 &&  make ceph-authtool -j 8 make monmaptool -j 8 make ceph-mon -j 8


export LC_ALL=C
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build

sudo parted -s -a optimal /dev/sdb mklabel gpt
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-data 0G 10G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-block 10G 20G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-wal 20G 30G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-db 30G 40G


sudo umount /mnt/osd-device-0-data
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data

/sbin/mkfs -t ext4 /dev/sdb1
/sbin/mkfs -t ext4 /dev/sdb2
/sbin/mkfs -t ext4 /dev/sdb3
/sbin/mkfs -t ext4 /dev/sdb4


sudo ceph osd create a81556d7-f7e7-41e1-a379-ee3296ea4a53
mkdir -p /mnt/osd-device-0-data
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data

sudo ceph-osd -i 0 --mkfs --mkkey --osd-uuid a81556d7-f7e7-41e1-a379-ee3296ea4a53
sudo ceph auth add osd.0 osd 'allow *' mon 'allow profile osd' mgr 'allow profile osd' -i /mnt/osd-device-0-data/keyring

ceph osd crush add-bucket crush0 host
ceph osd crush move crush0 root=default
ceph osd crush add osd.0 1.0 host=crush0
sudo ceph-osd -i 0 -c /etc/ceph/ceph.conf


ceph osd lspools
ceph osd pool create test-pool2 128
rados bench -p test-pool2 10 write --no-cleanup
rados bench -p test-pool2 1 write --no-cleanup >> log

rados bench -p test-pool2 1 write
ceph osd pool delete test-pool test-pool --yes-i-really-really-mean-it

ceph osd out osd.0
ceph osd down osd.0
ceph osd rm osd.0

ceph osd crush rm osd.0
ceph auth del osd.0

else
echo "no argument"

fi
