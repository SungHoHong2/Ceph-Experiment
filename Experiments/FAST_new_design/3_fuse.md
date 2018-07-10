### Weekly Report Objective
- Implement Cache with Fuse
- Integrate Fuse with Seastar
- Test performance on all filesystem


### Compariong FUSE with local cache
- combine seastar with FUSE
- calculate the time-latency created by FUSE
- even with using FUSE the performance should be better than normal cache, because it is using cache at least directly.
- test it on both sdb and sdc
- https://github.com/Doloops/mcachefs


```
sudo parted -s -a optimal /dev/sdb mklabel gpt
sudo parted -s -a optimal /dev/sdb mkpart fuse_local_storage 0% 50%
sudo parted -s -a optimal /dev/sdb mkpart fuse_local_storage 50% 100%
/sbin/mkfs -t ext4 /dev/sdb1
/sbin/mkfs -t ext4 /dev/sdb2
sudo mount -t ext4 /dev/sdb1 /mnt/hdd_storage
sudo mount -t ext4 /dev/sdb2 /mnt/hdd_cache

sudo parted -s -a optimal /dev/sdc mklabel gpt
sudo parted -s -a optimal /dev/sdc mkpart fuse_local_storage 0% 50%
sudo parted -s -a optimal /dev/sdc mkpart fuse_local_storage 50% 100%
/sbin/mkfs -t ext4 /dev/sdc1
/sbin/mkfs -t ext4 /dev/sdc2
sudo mount -t ext4 /dev/sdc1 /mnt/sdd_storage
sudo mount -t ext4 /dev/sdc2 /mnt/sdd_cache



mcachefs /media/input /media/output



```







<!-- ### Writing a Simple Filesystem Using FUSE in C
http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
sudo apt-get install libfuse-dev

FUSE (Filesystem in Userspace) is an interface that let you write your own filesystem for Linux in the user space
`Actually you can’t build a native filesystem which can store data to disk directly`
Instead you need to use an already exist filesystem to do so
for example GDFS is a filesystem which let you mount your Google Drive in your system and access the files there as normal files

**Simple Stupid Filesystem (SSFS)**


### Compare FIO performance using cache with FUSE and without FUSE


### Related Paper
- HyCache
https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6651104


### Run catfs and evaluate the performance
- Catfs will expose files in <from> under <mountpoint>, and cache them to <to> as they are accessed.
- Compare using catfs to cache sshfs vs sshfs only. Topology is laptop - 802.11n - router - 1Gbps wired - desktop. Laptop has SSD whereas desktop has spinning rust.


```
cd catfs
cargo install catfs
/root/.cargo/bin/catfs

$ catfs <from> <to> <mountpoint>
catfs#/src/dir#/cache/dir /mnt/point    fuse    allow_other,--uid=1001,--gid=1001,--free=1%   0       0
/root/.cargo/bin/catfs /mnt/osd-device-1-data /mnt/osd-device-1-data /mnt/osd-device-1-data/test
```

I'am trying to use the catfs to test the storage on local but the cache in the remote.
Right now I can run the cache and the storage all in local, but is there a way to enable cache as the remote?

According to the tests in the README file implies that either the cache or the storage was running in the remote side correct?
I am trying to use NFS in order to use the remote cache, but I don't know whether this is the right way


### NFS -->
