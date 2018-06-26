HOSTS="$(cat /etc/hostname)"
HISTORY="1_osd_seastar"

if [ "$1" = "host_to_git" ]
then
rm -rf history/$HISTORY
mkdir history/$HISTORY
# cp /Users/sunghohong/Documents/Projects/ceph/src/Makefile src/Makefile
cp /Users/sunghohong/Documents/Projects/ceph/src/ceph_osd.cc                    src/
cp /Users/sunghohong/Documents/Projects/ceph/src/osd/OSD.h                      src/
cp /Users/sunghohong/Documents/Projects/ceph/src/msg/simple/SimpleMessenger.cc  src/


cp src/* history/$HISTORY
cd /Users/sunghohong/Documents/Projects/Ceph-Experiment
git add .
git commit -m '$HISTORY'
git push origin master


elif [ "$1" = "git_to_server" ]
then
git pull origin master
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/ceph_osd.cc               /home/sungho/ceph/src/ceph_osd.cc
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/OSD.h                     /home/sungho/ceph/src/osd/OSD.h
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/SimpleMessenger.cc        /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc

      if [ "$HOSTS" = "w2" ]
      then
      # export LC_ALL=C
      # this part is used and not used sometimes
      cd /home/sungho/ceph/build
      make -j 8 && make install -j 8

      kill -9 `pidof ceph-osd`
      ceph-osd -i 0 -c /etc/ceph/ceph.conf

      elif [ "$HOSTS" = "wenji-w1" ]
      then
      kill -9 `pidof ceph-mon`
      kill -9 `pidof ceph-mgr`
      ceph-mon -i wenji -c /etc/ceph/ceph.conf
      ceph-mgr -i 0 -c /etc/ceph/ceph.conf
      fi


elif [ "$1" = "seastar-msg-wrk" ]
then
git pull origin master
    if [ "$HOSTS" = "w2" ]
    then
    cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    g++-5 `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` seastar-msg.cc -o seastar-msg
    # ./seastar-msg/seastar-ms --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1

    elif [ "$HOSTS" = "wenji-w1" ]
    then
    cd /home/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
    # g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` client.cc -o client
    g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` seastar-msg.cc -o seastar-msg
    # ./seastar-msg/seastar-ms --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1

    elif [ "$HOSTS" = "c3n24" ]
    then
      cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
      g++-5 -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -U_FORTIFY_SOURCE -DHAVE_DPDK -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -O2 -I/data1/sungho/seastar -I/data1/sungho/seastar/build/dpdk/include -I/data1/sungho/seastar/fmt -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares  -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -std=gnu++1y -g -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-sign-compare -Wno-error=cpp -DFMT_HEADER_ONLY -DHAVE_HWLOC -DHAVE_NUMA -Wl,--no-as-needed -Wl,--whole-archive  -Wl,--no-whole-archive -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/dpdk/lib -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lboost_thread -lcryptopp -lgnutls -lgnutlsxx -llz4 -lprotobuf -lgcc_s -lunwind -lyaml-cpp -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000  -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar seastar-msg.cc -o seastar-msg


    elif [ "$HOSTS" = "c3n25" ]
    then
      cd /data1/sungho/Ceph-Experiment/Ceph-Seastar/seastar-msg
      g++-5 -std=gnu++1y -L/data1/sungho/seastar/build/release -L/data1/sungho/seastar/build/release/fmt/fmt -Wl,--whole-archive,-lseastar -lrte_pmd_mlx4 -libverbs -Wl,--no-whole-archive -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA  -Wl,--no-as-needed     -lboost_program_options -lboost_system -lboost_filesystem -lstdc++ -lm -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s -lunwind  -lyaml-cpp -L/data1/sungho/seastar/build/dpdk/lib -Wl,--whole-archive -lrte_pmd_vmxnet3_uio -lrte_pmd_i40e -lrte_pmd_ixgbe -lrte_pmd_e1000 -lrte_pmd_ring -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_ena -lrte_pmd_enic -lrte_pmd_fm10k -lrte_pmd_nfp -lrte_pmd_qede -lrte_pmd_sfc_efx -lrte_hash -lrte_kvargs -lrte_mbuf -lrte_ethdev -lrte_eal -lrte_mempool -lrte_mempool_ring -lrte_ring -lrte_cmdline -lrte_cfgfile -Wl,--no-whole-archive -lrt -lm -ldl -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar -lfmt -std=gnu++1y -g  -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden  -pthread -I/data1/sungho/seastar -U_FORTIFY_SOURCE  -DHAVE_DPDK -I/data1/sungho/seastar/build/dpdk/include -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2 -Wno-error=literal-suffix -Wno-literal-suffix -Wno-invalid-offsetof -I/data1/sungho/seastar/fmt -DBOOST_TEST_DYN_LINK -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -DHAVE_HWLOC -DHAVE_NUMA -O2  -I/data1/sungho/seastar/build/release/gen -I/data1/sungho/seastar/build/release/c-ares seastar-msg.cc -o seastar-msg
    fi


elif [ "$HOSTS" = "ceph-install" ]
then

kill -9 `pidof seastar-msg`

make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build

sudo parted -s -a optimal /dev/sdb mklabel gpt
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-data 0G 10G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-block 10G 20G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-wal 20G 30G
sudo parted -s -a optimal /dev/sdb mkpart osd-device-0-db 30G 40G


sudo umount /mnt/osd-device-0-data
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-block /mnt/osd-device-0-block
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-wal /mnt/osd-device-0-wal
sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-db /mnt/osd-device-0-db


/sbin/mkfs -t ext4 /dev/sdb1
/sbin/mkfs -t ext4 /dev/sdb2
/sbin/mkfs -t ext4 /dev/sdb3
/sbin/mkfs -t ext4 /dev/sdb4


sudo ceph-bluestore-tool --cluster=ceph prime-osd-dir --dev /dev/ceph-8676e29c-801c-49d6-9809-1a3486b93a05/osd-block-bd3a04ed-b14e-4a16-b3d9-8b742be019a4 --path /var/lib/ceph/osd/ceph-17
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
