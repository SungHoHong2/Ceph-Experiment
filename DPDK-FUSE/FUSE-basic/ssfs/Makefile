ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk


# COMPILER = gcc
# FILESYSTEM_FILES = ssfs.c

# build: $(FILESYSTEM_FILES)
# 	$(COMPILER) $(FILESYSTEM_FILES) -o ssfs `pkg-config fuse --cflags --libs`
#	echo 'To Mount: ./ssfs -f [mount point]'

# clean:
#	rm ssfs



# binary name
 APP = ssfs

# all source are stored in SRCS-y
 SRCS-y := ssfs.c
 CFLAGS += -O3 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -lfuse
 include $(RTE_SDK)/mk/rte.extapp.mk




# mkdir -p /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build
# make -C /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build -f /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/Makefile \
# 	S=/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs O=/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build SRCDIR=/home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs
# [ -d ./ ] || mkdir -p ./
# [ -d ./ ] || mkdir -p ./
# set -e; echo "  LD dpdk-server"; gcc -o dpdk-server -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/include -I/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include -include /home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include/rte_config.h -O3  main.o -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_pcap -Wl,-lpcap -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/lib -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=dpdk-server.map -Wl,--cref && echo "cmd_dpdk-server = gcc -o dpdk-server -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/include -I/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include -include /home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/include/rte_config.h -O3  main.o -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_pcap -Wl,-lpcap -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,-export-dynamic -Wl,-export-dynamic -L/home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/lib -L/home/sungho/dpdk-stable-17.05.2/x86_64-native-linuxapp-gcc/lib -Wl,--as-needed -Wl,-Map=dpdk-server.map -Wl,--cref " > ./.dpdk-server.cmd
# touch _postbuild
# echo "  INSTALL-APP dpdk-server"
# [ -d /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/app ] || mkdir -p /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/build/app
# cp -f dpdk-server /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build/app
# echo "  INSTALL-MAP dpdk-server.map"
# [ -d /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build/app ] || mkdir -p /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build/app
# cp -f dpdk-server.map /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs/build/app
# touch _install
# true
# touch _postinstall
