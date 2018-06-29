### makefile log
- I think the third part is the one atht will allow you `integrate Seastar`
- this is the result from `make ceph-osd -j`

```
make -f CMakeFiles/Makefile2 ceph-osd
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/depend
make -f src/CMakeFiles/ceph-osd.dir/build.make src/CMakeFiles/ceph-osd.dir/build

/usr/bin/cmake -E cmake_echo_color --switch= --green --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num=3 "Building CXX object src/CMakeFiles/ceph-osd.dir/ceph_osd.cc.o"

cd /home/sungho/ceph/build/src && /usr/bin/c++   -DCEPH_LIBDIR=\"/usr/local/lib\" -DCEPH_PKGLIBDIR=\"/usr/local/lib/ceph\" -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D__linux__ -isystem /home/sungho/ceph/build/boost/include -I/home/sungho/ceph/build/src/include -I/home/sungho/ceph/src -isystem /home/sungho/ceph/build/include -I/usr/include/nss -I/usr/include/nspr -isystem /home/sungho/ceph/src/xxHash -isystem /home/sungho/ceph/src/rapidjson/include -I/home/sungho/ceph/src/dmclock/src -I/home/sungho/ceph/src/dmclock/support/src -isystem /home/sungho/ceph/src/googletest/googletest/include -isystem /home/sungho/ceph/src/spdk/include -isystem /home/sungho/ceph/build/src/dpdk/include -isystem /home/sungho/ceph/src/rocksdb/include  -Wall -Wtype-limits -Wignored-qualifiers -Winit-self -Wpointer-arith -Werror=format-security -fno-strict-aliasing -fsigned-char -Wno-unknown-pragmas -rdynamic  -ftemplate-depth-1024 -Wnon-virtual-dtor -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wstrict-null-sentinel -Woverloaded-virtual -fno-new-ttp-matching -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fstack-protector-strong -std=c++17 -fdiagnostics-color=auto -I/usr/include -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -O2 -g  -fPIE   -DHAVE_CONFIG_H -D__CEPH__ -D_REENTRANT -D_THREAD_SAFE -D__STDC_FORMAT_MACROS `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` -o CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -c /home/sungho/ceph/src/ceph_osd.cc

/usr/bin/cmake -E cmake_echo_color --switch= --green --bold --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num= "Linking CXX executable ../bin/ceph-osd"

cd /home/sungho/ceph/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/ceph-osd.dir/link.txt --verbose=

/usr/bin/cmake -E cmake_echo_color --switch= --progress-dir=/home/sungho/ceph/build/CMakeFiles --progress-num=3 "Built target ceph-osd"

/usr/bin/cmake -E cmake_progress_start /home/sungho/ceph/build/CMakeFiles 0
```
