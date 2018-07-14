export RTE_SDK=/home/sungho/dpdk-stable-17.05.2
export DESTDIR=/usr/local
export RTE_TARGET=x86_64-native-linuxapp-gcc
git pull origin master
HOSTS="$(cat /etc/hostname)"


if [ "$1" = "dpdk_application_make" ]
then
  if [ "$HOSTS" = "w1" ]
  then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-server/
  make
  ./build/dpdk-server -c 0x2 -n 4 -- -q 8 -p 0x1 -T 1

  elif [ "$HOSTS" = "w2" ]
  then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/dpdk-client/
  make
  ./build/dpdk-client -c 0x2 -n 4 -- -q 8 -p 0x1 -T 1
  fi

elif [ "$1" = "dpdk_client_kill" ]
then
  kill -9 `pidof dpdk-client`
  kill -9 `pidof dpdk-server`


elif [ "$1" = "mcachefs_make" ]
then
echo "mcachefs /media/input /media/output"
echo "The target filesystem: slow filesystem that cache accesses to"
echo "the backing filesystem: mcachefs can stash stuff which it has copied"

echo "--direct: If true, use non-buffered I/O"
echo "non-buffered I/O involves reading or writing data one element at a time"

cp /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/mcachefs/mcachefs.c /home/sungho/mcachefs/src
cd /home/sungho/mcachefs/src
make

#  /sbin/mkfs -t ext4 /dev/sdb2
umount /mnt/hdd_cache
/home/sungho/mcachefs/src/mcachefs /dev/sdb2 /mnt/hdd_cache






else
echo "no argument"
fi
