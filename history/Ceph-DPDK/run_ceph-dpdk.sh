sudo modprobe uio
sudo insmod /home/sungho/ceph-12.2.0/src/dpdk-17/build/kmod/igb_uio.ko
sudo echo 1024 | sudo tee /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

wait
sudo mount -t hugetlbfs nodev /mnt/huge

wait
sudo ifconfig enp6s0 down

wait
sudo ./home/sungho/ceph-12.2.0/src/dpdk-17/usertools/dpdk-devbind.py --bind=igb_uio enp6s0

wait
sudo ceph-mon -i monitortest -c /etc/ceph/ceph.conf
