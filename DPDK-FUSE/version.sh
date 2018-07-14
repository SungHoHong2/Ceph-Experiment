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


elif [ "$1" = "ssfs_setup" ]
then
  umount /mnt/ssd_cache
  sudo mount -t ext4 /dev/sdc2 /mnt/ssd_cache
  mkdir /mnt/ssd_cache/test

elif [ "$1" = "ssfs_make" ]
then
  cd /home/sungho/Ceph-Experiment/DPDK-FUSE/FUSE-basic/ssfs
  make
  ./build/ssfs  -f /mnt/ssd_cache/test

elif [ "$1" = "ssfs_client" ]
then
  ls -l /mnt/ssd_cache/test
  cat /mnt/ssd_cache/test/client


else
echo "no argument"
fi
