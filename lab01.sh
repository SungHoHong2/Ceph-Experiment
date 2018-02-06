```
sudo ceph-mon -i wenji -c /etc/ceph/ceph.conf
ceph-mgr -i 0 -c /etc/ceph/ceph.conf

sudo mount -t ext4 /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data
sudo ceph-osd -i 0 -c /etc/ceph/ceph.conf

rados -p scbench cleanup
ceph osd pool create scbench 100 100
rados bench -p scbench 10 write --no-cleanup
```
