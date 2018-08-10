### Tasks for 9/3
- [x] divide latency for accessing the data during cache-hit
    - [x] we can do this by just counting the time of getting the data
- [ ] install Wenji's fuse verison
  - [ ] run the thing

  `copy the files and paste it`
  sudocmd cp /data1/sungho/server /mnt/gdcache/server
  sudocmd cat /mnt/gdcache/server

  cat /mnt/gdcache/data1/sungho/client.txt
  sudocmd ls -l /mnt/gdcache/
  lets run FIO in /mnt/gdcache shall we?
  cephadmin@c3n22:/mnt/src/cephadmin


  sudocmd cp /data1/sungho/server_1G /mnt/gdcache/server_1G
  sudocmd fio -filename=/mnt/gdcache/server_1G -filesize 1G -direct=1 -thread -rw=randread -bs=1k -numjobs=1 -runtime=20 -name=mytest








  - [ ] test the version on FIO
- [ ] add latency for local cache-miss, local-cache hit, remote cache hit
      - `need to ask this part again` the local miss we did that before and was told not to do that.
      -







### not yet
- [ ] mount sdb1 to /mnt/src for HDD
  - [ ] massive migration of c3n24 and c3n25
  - [ ] I need to swich the past permissions into data cause those ones will not work anymore
        /data/sungho/wireshark
        /data/sungho/commands_pcap.sh

  - [ ] change ./bashrc
        alias sudoconf='vi /data1/sungho/commands_pcap.sh'
        alias sudocmd='sudo /data1/sungho/commands_pcap.sh'


### problems with using gdc-fuse
sudo ./build/gdcache /mnt/gdcache -o src_path=/mnt/src -o cache_path=/dev/nvme0n1p
or sudo ./passthrough -f /mnt/gdcache
However, if I run like this then I am not able to view the gdcache.
The samething happens even if I use directories that are onwed the the root

For example, ssd_cache directory shows the same result.
sudo ./build/gdcache /mnt/ssd_cache -o src_path=/mnt/src -o cache_path=/dev/nvme0n1p

[BEFORE]
drwxr-xr-x 2 sungho    sungho    4096 Aug  1 11:55 gdcache
drwxr-xr-x 3 root      root         0 Jul 31 19:54 huge
drwxr-xr-x 2 cephadmin cephadmin 4096 Nov 15  2017 osd-device-2-data
drwxr-xr-x 2 sungho    sungho    4096 Aug  1 11:54 src
drwxr-xr-x 2 root      root      4096 Jul 28 16:26 ssd_cache
drwxr-xr-x 2 sungho    sungho    4096 Jul 24 00:14 sungho

[AFTER]
d????????? ? ?         ?            ?            ? gdcache
drwxr-xr-x 3 root      root         0 Jul 31 19:54 huge
drwxr-xr-x 2 cephadmin cephadmin 4096 Nov 15  2017 osd-device-2-data
drwxr-xr-x 2 sungho    sungho    4096 Aug  1 11:54 src
drwxr-xr-x 2 root      root      4096 Jul 28 16:26 ssd_cache
drwxr-xr-x 2 sungho    sungho    4096 Jul 24 00:14 sungho


I can prevent this by running the sudo as another user.
sudo -u sungho ./build/gdcache /mnt/gdcache -o src_path=/mnt/src -o cache_path=/dev/nvme0n1p
or sudo -u sungho ./passthrough -f /mnt/gdcache
however if I run like this then I will not be able to run the /dev/nvme0n1p1 since the sudo is sungho

[BEFORE]
drwxr-xr-x 2 sungho    sungho    4096 Aug  1 11:55 gdcache
drwxr-xr-x 3 root      root         0 Jul 31 19:54 huge
drwxr-xr-x 2 cephadmin cephadmin 4096 Nov 15  2017 osd-device-2-data
drwxr-xr-x 2 sungho    sungho    4096 Aug  1 11:54 src
drwxr-xr-x 2 root      root      4096 Jul 28 16:26 ssd_cache
drwxr-xr-x 2 sungho    sungho    4096 Jul 24 00:14 sungho


[AFTER]
*******Error: open [/dev/nvme0n1p1] failed********
drwxr-xr-x 24 root      root      4096 Aug  1 11:53 gdcache
drwxr-xr-x  3 root      root         0 Jul 31 19:54 huge
drwxr-xr-x  2 cephadmin cephadmin 4096 Nov 15  2017 osd-device-2-data
drwxr-xr-x  2 sungho    sungho    4096 Aug  1 11:54 src
drwxr-xr-x  2 root      root      4096 Jul 28 16:26 ssd_cache
drwxr-xr-x  2 sungho    sungho    4096 Jul 24 00:14 sungho



### Preliminary Measures
mv /etc/init.d/fuse3 fuse3_backup

### installing libfuse
```
# meson install
sudocmd apt-get install python3 ninja-build
sudocmd apt install python3-pip
export LC_ALL=C

get-pip.py download
python3.5 get-pip.py

pip3 install --user meson
export PATH=$PATH:/home/sungho/.local/bin/
export PATH=$PATH:/home/users/sungho/.local/bin/

# install libfuse
git clone https://github.com/libfuse/libfuse.git
mkdir build; cd build
meson ..

ninja
wget https://github.com/ninja-build/ninja/releases/download/v1.5.3/ninja-linux.zip

sudo ln -s /home/sungho/libfuse/build/lib/libfuse3.so.3.2.4 /lib/x86_64-linux-gnu/libfuse3.so.3
gcc -Wall passthrough.c -D_FILE_OFFSET_BITS=64 -I/usr/local/include/fuse3 -L/usr/local/lib/x86_64-linux-gnu -lfuse3 -lpthread -o passthrough
./passthrough -f /mnt/ssd_cache
```

gcc passthrough.c `pkg-config fuse3 --cflags --libs`  -o passthrough
