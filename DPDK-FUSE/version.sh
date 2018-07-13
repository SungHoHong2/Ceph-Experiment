export RTE_SDK=/home/sungho/dpdk-stable-17.05.2
export DESTDIR=/usr/local
export RTE_TARGET=x86_64-native-linuxapp-gcc
git pull origin master


if [ "$1" = "multiprocess_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process
make

elif ["$1" = "multiprocess_simple_mp" ]
then
/home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/simple_mp/build/simple_mp -c 3 -n 4 --proc-type=primary
/home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/simple_mp/build/simple_mp -c 3 -n 4 --proc-type=secondary
#  simple_mp > send hello_secondary
#
# ./mp_server/build/mp_server -c 6 -n 4 -- -p 3 -n 2
# ./mp_client/build/mp_client -c 8 -n 4 --proc-type=auto -- -n 0
# ./mp_client/build/mp_client -c 10 -n 4 --proc-type=auto -- -n 1
#
#
# elif ["$1" = "dpdk_server_make" ]
# then
# cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/
# make
# ./build/dpdk-server
#

else
echo "no argument"

fi
