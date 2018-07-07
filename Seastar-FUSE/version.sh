HOSTS="$(cat /etc/hostname)"
HISTORY="4_osd_seastar"
HISTORY_SEASTAR_MSG="3_seastar_msg"


if [ "$1" = "seastar" ]
then
rm -rf history/$HISTORY
mkdir history/$HISTORY
# cp /Users/sunghohong/Documents/Projects/ceph/src/Makefile src/Makefile
cp /Users/sunghohong/Documents/Projects/ceph/src/ceph_osd.cc                    src/
cp /Users/sunghohong/Documents/Projects/ceph/src/osd/OSD.h                      src/
cp /Users/sunghohong/Documents/Projects/ceph/src/msg/simple/SimpleMessenger.cc  src/
cp /Users/sunghohong/Documents/Projects/ceph/src/os/bluestore/NVMEDevice.cc     src/

cp src/* history/$HISTORY
cd /Users/sunghohong/Documents/Projects/Ceph-Experiment
git add .
git commit -m '$HISTORY'
git push origin master


# # sudocmd ceph_seastar_msg git_to_server
# elif [ "$1" = "git_to_server" ]
# then
# git pull origin master
# cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/ceph_osd.cc               /home/sungho/ceph/src/ceph_osd.cc
# cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/OSD.h                     /home/sungho/ceph/src/osd/OSD.h
# cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/SimpleMessenger.cc        /home/sungho/ceph/src/msg/simple/SimpleMessenger.cc
#
#
#       if [ "$HOSTS" = "w2" ]
#       then
#       # export LC_ALL=C
#       # you must! and must! only use sudo in the make install!!!
#       # this part is used and not used sometimes
#       cd /home/sungho/ceph/build
#       make ceph-osd -j 8 -n && make install -j 8
#
#       kill -9 `pidof ceph-osd`
#       ceph-osd -i 0 -c /etc/ceph/ceph.conf
#
#       elif [ "$HOSTS" = "wenji-w1" ]
#       then
#       kill -9 `pidof ceph-mon`
#       kill -9 `pidof ceph-mgr`
#       ceph-mon -i wenji -c /etc/ceph/ceph.conf
#       ceph-mgr -i 0 -c /etc/ceph/ceph.conf
#       fi



else
echo "no argument"

fi
