```
sudo ceph-mon -i wenji -c /etc/ceph/ceph.conf
ceph-mgr -i 0 -c /etc/ceph/ceph.conf

sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data
sudo ceph-osd -i 0 -c /etc/ceph/ceph.conf

rados -p scbench cleanup
ceph osd pool create scbench 100 100
rados bench -p scbench 10 write --no-cleanup
```

export RTE_SDK=/data1/sungho/dpdk-stable-17.05.1
export DESTDIR=/usr/local
export RTE_TARGET=x86_64-native-linuxapp-gcc


if [ "$1" = "git_store" ]
then
git config credential.helper store

elif [ "$1" = "git" ]
then
git pull origin master

elif [ "$1" = "ceph_seastar_fuse" ]
then
cd /data1/sungho/Ceph-Experiment/Seastar-FUSE
/data1/sungho/Ceph-Experiment/Seastar-FUSE/version.sh $2 $3


elif [ "$1" = "FUSE-install-ready" ]
then

else
sudo $1 $2 $3 $4 $5 $6 $7 $8 $9

fi
