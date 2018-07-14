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

1. talk to filesystem
2. FUSE grabs the message
3. the message is sent from the client
4. the message is received from the server
5. the message is returned to the client

1. we want to have a duplicated scheme
