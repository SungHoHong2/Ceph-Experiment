```
# running on DPDK
sudo ./server --dpdk-pmd --dpdk-port-index 1 --network-stack native --dhcp 0 --host-ipv4-addr 10.107.30.40 --netmask-ipv4-addr 255.255.254.0 --gw-ipv4-addr 10.107.30.1 --collectd 0 --smp 2

sudo ./server --dpdk-pmd --dpdk-port-index 1 --network-stack native
```
