```
# running on DPDK
sudo ./server --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 2

./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.251 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 2



sudo ./server --dpdk-pmd --dpdk-port-index 1 --network-stack native

```
