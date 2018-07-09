# /home/sungho/Ceph-Experiment/Seastar-FUSE

HOSTS="$(cat /etc/hostname)"
HISTORY="4_osd_seastar"
HISTORY_SEASTAR_MSG="3_seastar_msg"

if [ "$1" = "seastar_make" ]
then
git pull origin master
# ./configure.py --enable-dpdk --compiler g++-5 --mode release
      if [ "$HOSTS" = "w2" ] # DK
      then
      cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
      g++-5  `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` client.cc -o client

      elif [ "$HOSTS" = "w1" ] # WENJI
      then
      cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
      g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` server.cc -o server

    elif [ "$HOSTS" = "c3n24" ]
    then
      cd /data1/sungho/Ceph-Experiment/Seastar-FUSE/seastar
      g++-5 -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DHAVE_DPDK -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -O2 -I/data1/sungho/seastar -I/data1/sungho/seastar/build/dpdk/include -I/data1/sungho/seastar/fmt -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares  -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -Wl,--no-as-needed -Wl,--whole-archive  -Wl,--no-whole-archive -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/dpdk/lib -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lboost_thread -lcryptopp -lgnutls -lgnutlsxx -llz4 -lprotobuf -lgcc_s -lunwind -lyaml-cpp -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000  -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar client.cc -o client


    elif [ "$HOSTS" = "c3n25" ]
    then
      cd /data1/sungho/Ceph-Experiment/Seastar-FUSE/seastar
      g++-5 -std=gnu++1y -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/release/fmt/fmt -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA  -Wl,--no-as-needed     -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind  -lyaml-cpp -L/data1/sungho/seastar/build/dpdk/lib -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000 -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA -O2  -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares server.cc -o server
    fi

      fi

elif [ "$1" = "seastar_run" ]
then
cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
    if [ "$HOSTS" = "w2" ]
    then
    ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --test "$2" --smp 1

    elif [ "$HOSTS" = "w1" ]
    then
    ./server --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1
    fi

elif [ "$1" = "SEASTAR_TESTS" ]
then
  if [ "$HOSTS" = "w2" ]
  then
  cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
  SEARCH_TEXT="AVG\|STDEV"
  TEMP_FILE_LOG="/home/sungho/Ceph-Experiment/Seastar-FUSE/seastar_file"
  rm -rf $TEMP_FILE_LOG && touch $TEMP_FILE_LOG # | grep "READ:"

  echo "PING" >> $TEMP_FILE_LOG
  ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --test ping --smp 1 | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG
  wait
  echo "TXTX" >> $TEMP_FILE_LOG
  ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --test txtx --smp 1 | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG
  wait
  echo "RXRX" >> $TEMP_FILE_LOG
  ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --test rxrx --smp 1 | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG
  fi

elif [ "$1" = "FIO_TESTS" ]
then
  if [ "$HOSTS" = "w1" ]
  then
  FILENAME_ARG="/dev/sdb"
  SEARCH_TEXT="read :\|write:\|lat (\|clat ("
  cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
  TEMP_FILE_LOG="/home/sungho/Ceph-Experiment/Seastar-FUSE/temp_file"
  rm -rf $TEMP_FILE_LOG && touch $TEMP_FILE_LOG # | grep "READ:"

  echo "READ" >> $TEMP_FILE_LOG
  fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randread -bs=4k -numjobs=1 -size 1G  --group_reporting -name=mytest | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG
  echo "WRITE" >>  $TEMP_FILE_LOG
  fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randwrite -bs=4k -numjobs=1 -size 1G  --group_reporting -name=mytest | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG
  echo "RANDRW" >> $TEMP_FILE_LOG
  fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randrw -bs=4k -numjobs=1 -size 1G --group_reporting -name=mytest | grep "$SEARCH_TEXT" >> $TEMP_FILE_LOG

  # sudo fio --name=randwrite --ioengine=libaio --iodepth=1 --rw=randwrite --bs=4k --direct=0 --size=512M --numjobs=2 --runtime=240 --group_reporting
  # sudo fio --filename=/dev/sdb --iodepth=1 --numjobs 1 --direct=1 --rw=randwrite --bs=4k --size 512M --group_reporting –-name=benchtest
  fi

else
echo "no argument"

fi
