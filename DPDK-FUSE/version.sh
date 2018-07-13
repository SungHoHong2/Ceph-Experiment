export RTE_SDK=/home/sungho/dpdk-stable-17.05.2
export DESTDIR=/usr/local
export RTE_TARGET=x86_64-native-linuxapp-gcc
git pull origin master

if [ "$1" = "dpdk_server_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/
make
./build/dpdk-server -c 0x2 -n 4 -- -q 8 -p 0x1 -T 1

elif [ "$1" = "dpdk_client_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-client/
make
./build/dpdk-client -c 0x2 -n 4 -- -q 8 -p 0x1 -T 1

elif [ "$1" = "dpdk_client_kill" ]
then
kill -9 `pidof dpdk-client`
kill -9 `pidof dpdk-server`
else
echo "no argument"

fi
