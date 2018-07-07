# /home/sungho/Ceph-Experiment/Seastar-FUSE

HOSTS="$(cat /etc/hostname)"
HISTORY="4_osd_seastar"
HISTORY_SEASTAR_MSG="3_seastar_msg"

if [ "$1" = "seastar_make" ]
then
git pull origin master
cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
# ./configure.py --enable-dpdk --compiler g++-5 --mode release
      if [ "$HOSTS" = "w2" ] # DK
      then
      g++-5  `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` client.cc -o client

      elif [ "$HOSTS" = "w1" ] # WENJI
      then
      g++ `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` server.cc -o server
      fi

elif [ "$1" = "seastar_run" ]
then
cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
    if [ "$HOSTS" = "w2" ]
    then
    ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --test "$2" --smp 1

    elif [ "$HOSTS" = "w1" ]
    then
    ./server --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1
    fi

elif [ "$1" = "FIO_TESTS" ]
then
FILENAME_ARG="/dev/sdb"

cd /home/sungho/Ceph-Experiment/Seastar-FUSE/seastar
rm -rf temp_file && touch temp_file # | grep "READ:"

echo "READ" >> temp_file
fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randread -bs=4k -numjobs=1 -size 30M  --group_reporting -name=mytest | grep "READ:" >> temp_file
wait
echo "WRITE" >> temp_file
fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randwrite -bs=4k -numjobs=1 -size 30M  --group_reporting -name=mytest | grep "WRITE:" >> temp_file
wait
echo "RANDRW" >> temp_file
fio -filename=$FILENAME_ARG -direct=1 -iodepth=1 -thread -rw=randrw -bs=4k -numjobs=1 -size 30M  --group_reporting -name=mytest | grep "WRITE:|READ:" >> temp_file


# sudo fio --name=randwrite --ioengine=libaio --iodepth=1 --rw=randwrite --bs=4k --direct=0 --size=512M --numjobs=2 --runtime=240 --group_reporting
# sudo fio --filename=/dev/sdb --iodepth=1 --numjobs 1 --direct=1 --rw=randwrite --bs=4k --size 512M --group_reporting –-name=benchtest

else
echo "no argument"

fi
