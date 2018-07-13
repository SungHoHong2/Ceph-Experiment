export RTE_SDK=/home/sungho/dpdk-stable-17.05.2
export DESTDIR=/usr/local
export RTE_TARGET=x86_64-native-linuxapp-gcc
git pull origin master


if [ "$1" = "multiprocess_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process
make

elif [ "$1" = "multiprocess_tutorial" ]
then
  if [ "$2" = "server" ]
  then
  /home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/client_server_mp/mp_server/build/mp_server -c 0x3 -n 4 -- -p 0x3 -n 1

  elif [ "$2" = "client" ]
  then
  /home/sungho/Ceph-Experiment/DPDK-FUSE/multi_process/client_server_mp/mp_client/build/mp_client -c 0x4 -n 4 --proc-type=auto -- -n 0

  # ./mp_server/build/mp_server -c 0x3 -n 4 -- -p 0x3 -n 1
  # ./mp_client/build/mp_client -c 0x4 -n 4 --proc-type=auto -- -n 0
  # -c : core running
  # -n : number of clients
  # -p : ports
  fi


elif [ "$1" = "dpdk_server_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/
make
./build/dpdk-server -c 0x3 -n 4 -- -p 0x1 -n 1

elif [ "$1" = "dpdk_client_make" ]
then
cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-client/
make
./build/dpdk-client -c 0x3 -n 4 -- -p 0x1 -n 1

else
echo "no argument"

fi
