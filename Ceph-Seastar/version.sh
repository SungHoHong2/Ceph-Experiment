HISTORY="0_osd_seastar"

if [ "$1" = "host_to_git" ]
then
rm -rf history/$HISTORY
mkdir history/$HISTORY
cp /Users/sunghohong/Documents/Projects/ceph/Makefile src/Makefile
cp /Users/sunghohong/Documents/Projects/ceph/src/ceph_osd.cc src/ceph_osd.cc



cp src/* history/$HISTORY
git add .
git commit -m '$HISTORY'
git push origin master



elif [ "$1" = "git_to_server" ]
then
git pull origin master
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/Makefile /home/sungho/ceph/build/
cp /home/sungho/Ceph-Experiment/Ceph-Seastar/src/ceph_osd /home/sungho/ceph/build/

else
echo "no argument"

fi
