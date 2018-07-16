### run fuse

apt-get install gcc fuse libfuse-dev make cmake
git clone https://github.com/fntlnz/fuse-example.git
cmake CMakeLists.txt
make


sudo mount /dev/sdb1 /mnt/hdd_storage
https://engineering.facile.it/blog/eng/write-filesystem-fuse/
./bin/fuse-example -d -s -f /mnt/hdd_storage

umount /mnt/hdd_storage
/sbin/mkfs -t ext4 /dev/sdb1
sudo mount -t ext4 /dev/sdb1 /mnt/hdd_storage
sudo mount -t ext4 /dev/sdc2 /mnt/ssd_cache


./ssfs -f /mnt/ssd_cache/test

lets test it by using basic read and write

1. talk to filesystem (write & read)
2. FUSE grabs the message
3. the message is sent from the client
4. the message is received from the server
5. the message is returned to the client
1. we want to have a duplicated scheme


./ssfs -f /mnt/hdd_cache/

### combing DPDK with FUSE
- right now FUSE does not run with single thread,
- however we also have founded out that DPDK can run using for loop. (worst case scenario)
- for thank god




### to-do original both TCP and DPDK
- []client-side : request to filesystem -> filesystem talk to dpdk - > put data to `fuse-tx-queue`
- [x]server-side : get data from `fuse-rx-queue` - > dpdk talk to filesystem -> request to file-system -> put the result in the `fuse-tx-queue`
- []client-side : get data from `fuse-rx-queue` -> dpdk respond to the client that it has returned.
