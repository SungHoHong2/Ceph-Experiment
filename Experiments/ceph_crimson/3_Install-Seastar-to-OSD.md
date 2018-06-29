### fixing workstation
```
the problem happened in the graphic card driver, and the driver cannot be installed in the recovery mode
the workstation is not completely normal, even after reinstalling everything.
the workstation becomes very slow
```

### makefile log

```
make ceph-osd -n -j 8 >> install_log

make -f src/CMakeFiles/ceph-fuse.dir/build.make src/CMakeFiles/ceph-fuse.dir/depend
make -f src/CMakeFiles/ceph-mon.dir/build.make src/CMakeFiles/ceph-mon.dir/depend
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/depend
make -f src/CMakeFiles/ceph-mgr.dir/build.make src/CMakeFiles/ceph-mgr.dir/depend
make -f src/CMakeFiles/cephfs.dir/build.make src/CMakeFiles/cephfs.dir/depend
cd /home/sungho/ceph/build && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/ceph /home/sungho/ceph/src /home/sungho/ceph/build /home/sungho/ceph/build/src /home/sungho/ceph/build/src/CMakeFiles/ce
ph-mon.dir/DependInfo.cmake --color=
make -f src/CMakeFiles/ceph-mon.dir/build.make src/CMakeFiles/ceph-mon.dir/build
cd /home/sungho/ceph/build && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/ceph /home/sungho/ceph/src /home/sungho/ceph/build /home/sungho/ceph/build/src /home/sungho/ceph/build/src/CMakeFiles/ceph-osd.dir/DependInfo.cmake --color=

make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build

cd /home/sungho/ceph/build && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/ceph /home/sungho/ceph/src /home/sungho/ceph/build /home/sungho/ceph/build/src /home/sungho/ceph/build/src/CMakeFiles/cephfs.dir/DependInfo.cmake --color=
make -f src/CMakeFiles/cephfs.dir/build.make src/CMakeFiles/cephfs.dir/build
```




```c++
sudo make -f CMakeFiles/Makefile2 src/CMakeFiles/ceph-osd.dir/all

#ceph-osd
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/depend
cd /home/sungho/ceph/build && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/ceph /home/sungho/ceph/src /home/sungho/ceph/build /home/sungho/ceph/build/src /home/sungho/ceph/build/src/CMakeFiles/ ceph-osd.dir/DependInfo.cmake --color=
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build


make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/depend
cd /home/sungho/ceph/build && /usr/bin/cmake -E cmake_depends "Unix Makefiles" /home/sungho/ceph /home/sungho/ceph/src

/home/sungho/ceph/build /home/sungho/ceph/build/src /home/sungho/ceph/build/src/CMakeFiles/ceph-osd.dir/DependInfo.cmake --color=
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build

/usr/bin/cmake -E cmake_echo_color --switch= --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num=3 "Built target ceph-osd"





```

```
src/all: src/CMakeFiles/ceph-osd.dir/all
```
