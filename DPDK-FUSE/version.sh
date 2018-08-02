git pull origin master
HOSTS="$(cat /etc/hostname)"


if [ "$1" = "raw_filesystem_exp" ]
then
  if [ "$HOSTS" = "c3n24" ]
  then
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/RAW_block
  gcc -o string_generator string_generator.c
  ./string_generator

elif [ "$HOSTS" = "c3n25" ]
  then
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/RAW_block
  gcc -o random_read random_read.c
  ./random_read
  fi

elif [ "$1" = "fuselib_modify" ]
then
  if [ "$HOSTS" = "c3n25" ]
  then
  cp /data1/sungho/Ceph-Experiment/DPDK-FUSE/low_level/fuse_lowlevel.c /data1/sungho/libfuse/lib
  export PATH=$PATH:/home/users/sungho/.local/bin/
  cd /data1/sungho/libfuse
  rm -rf build
  mkdir build; cd build
  meson ..
  ninja
  sudo ninja install
  cp /data1/sungho/libfuse/build/lib/libfuse3.so /usr/local/lib/x86_64-linux-gnu/
  cp /data1/sungho/libfuse/build/lib/libfuse3.so.3 /usr/local/lib/x86_64-linux-gnu/
  cp /data1/sungho/libfuse/build/lib/libfuse3.so.3.2.4 /usr/local/lib/x86_64-linux-gnu/


  fi


elif [ "$1" = "low_level_exp" ]
then
  if [ "$HOSTS" = "c3n25" ]
  then
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/low_level
  gcc passthrough_ll.c `pkg-config fuse3 --cflags --libs`  -o passthrough_ll
  ./passthrough_ll -f /mnt/ssd_cache
  fi


# RUNNING THE FUSE-DPDK BEGN
elif [ "$1" = "filesystem_setup" ]
then
  if [ "$HOSTS" = "w1" ]
  then
  umount -l /mnt/ssd_cache
  sudo mount -t ext4 /dev/sdc2 /mnt/ssd_cache
  # mkdir /mnt/ssd_cache/test
  elif [ "$HOSTS" = "w2" ]
  then
  umount -l /mnt/ssd_cache
  sudo mount -t ext4 /dev/sdb1 /mnt/ssd_cache

  elif [ "$HOSTS" = "c3n25" ]
  then
    if [ "$2" = "DPDK" ]
    then
    umount -l /mnt/ssd_cache
    sudo mount -t ext4 /dev/nvme0n1p1 /mnt/ssd_cache
    sudo kill -9 `pidof ssfs`
    elif [ "$2" = "POSIX" ]
    then
    umount -l /mnt/hdd
    sudo kill -9 `pidof ssfs`
    cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/posix_client
    gcc -D _BSD_SOURCE ssfs.c -o ssfs -lm -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread
    fi
  fi

elif [ "$1" = "ssfs_experiment" ]
then
  if [ "$HOSTS" = "w2" ]
  then
  cat /mnt/ssd_cache/home/sungho/client.txt
  elif [ "$HOSTS" = "c3n25" ]
    then
    if [ "$2" = "DPDK" ]
    then
    cat /mnt/ssd_cache/data1/sungho/client.txt
    elif [ "$2" = "POSIX" ]
    then
    cat /mnt/hdd/data1/sungho/client.txt
    fi
  fi

elif [ "$1" = "Fuse_exp_benchmark" ]
then
  if [ "$HOSTS" = "w2" ]
  then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-4th/fuse_benchmark
  gcc bench.c -o bench -lm && ./bench
  elif [ "$HOSTS" = "c3n25" ]
  then
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-5th/fuse_benchmark
  # gcc bench.c -o bench -lm && ./bench  "TEN" "HUNDRED" "DPDK" "POSIX"
  gcc bench.c -o bench -lm && ./bench $2 $3 $4 $5
  # sudocmd chmod 777 /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-5th/fuse_benchmark/bench
  fi

elif [ "$1" = "Fuse_6th_DPDK_make_launch" ]
then
  if [ "$HOSTS" = "w1" ]
  then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/dpdk-server/
  make
  ./build/dpdk-server -c 0x2 -n 4 -- -q 8 -p 0x2 -T 1

  elif [ "$HOSTS" = "w2" ]
  then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/dpdk_client
  gcc -D _BSD_SOURCE ssfs.c -o ssfs -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs_client/build/include -I/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include -include /home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include/rte_config.h -O3 -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread  -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs_client/build/lib -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=ssfs.map -Wl,--cref && echo "cmd_ssfs = gcc -o ssfs_server -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/dpdk_client_0/build/include -I/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include -include /home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include/rte_config.h -O3 -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread  -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/dpdk_client_0/build/lib -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=ssfs_server.map -Wl,--cref " > ./.ssfs.cmd touch _postbuild
  ./ssfs -f /mnt/ssd_cache/

  elif [ "$HOSTS" = "c3n24" ]
  then
  export RTE_SDK=/data1/sungho/dpdk-stable-17.05.1
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/dpdk-server/
  make
  ./build/dpdk-server $1 $2 $3 $4
  # ./build/dpdk-server "CACHE_MISS" "nvme0n1p1"
  # ./build/dpdk-server "nvme0n1p1"
  # ./build/dpdk-server "nvme0n1p1" "DEBUG"


  elif [ "$HOSTS" = "c3n25" ]
  then
  export RTE_SDK=/data1/sungho/dpdk-stable-17.05.1
  cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/dpdk_client
  gcc -D _BSD_SOURCE ssfs.c -o ssfs -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread -m64 -pthread -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-4th/dpdk-server/build/include -I/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/include -include /data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/include/rte_config.h -O3 -L/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_mlx4 -Wl,-libverbs -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_pcap -Wl,-lpcap -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-4th/dpdk-server/build/lib -L/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=dpdk-server.map -Wl,--cref && echo "cmd_dpdk-server = gcc -o dpdk-server -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-4th/dpdk-server/build/include -I/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/include -include /data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/include/rte_config.h -O3  -L/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_mlx4 -Wl,-libverbs -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_pcap -Wl,-lpcap -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-4th/dpdk-server/build/lib -L/data1/sungho/dpdk-stable-17.05.1/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=dpdk-server.map -Wl,--cref " > ./ssfs.cmd touch _postbuild
  ./ssfs -f /mnt/ssd_cache $1 $2 $3 $4
  # sudo ./ssfs -f /mnt/ssd_cache "CACHE_MISS" "HUNDRED"
  # sudo ./ssfs -f /mnt/ssd_cache "CACHE_HIT" "HUNDRED"
  # sudo ./ssfs -f /mnt/ssd_cache "CACHE_HIT" "HUNDRED" "DEBUG"
  fi

elif [ "$1" = "Fuse_5th_POSIX_make_launch" ]
then
if [ "$HOSTS" = "w1" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/posix_server
gcc ssfs.c -o ssfs -pthread
./ssfs

elif [ "$HOSTS" = "w2" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/posix_client
gcc -D _BSD_SOURCE ssfs.c -o ssfs -lm -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread
./ssfs -f /mnt/ssd_cache/
fi

if [ "$HOSTS" = "c3n24" ]
then
cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/posix_server
gcc ssfs.c -o ssfs -lpthread
# ./ssfs
# ./ssfs "DEBUG" "CACHE_MISS" "nvme0n1p1"
./ssfs $1 $2 $3 $4

elif [ "$HOSTS" = "c3n25" ] # a lot of problem with permission issues
then
cd /data1/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-6th/posix_client
# gcc -D _BSD_SOURCE ssfs.c -o ssfs -lm -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread
./ssfs -f /mnt/hdd  $1 $2 $3 $4
# ./ssfs -f /mnt/ssd_cache "CACHE_HIT" "HUNDRED"

fi

# RUNNING THE FUSE-DPDK END
else
echo "no argument"
fi
