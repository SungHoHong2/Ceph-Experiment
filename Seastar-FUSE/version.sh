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
    ./server --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --server "10.218.111.254:1234" --smp 1

    elif [ "$HOSTS" = "w1" ]
    then
    ./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.254 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 1
    fi

else
echo "no argument"

fi
