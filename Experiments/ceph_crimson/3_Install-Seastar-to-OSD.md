### makefile log
- I think the third part is the one atht will allow you `integrate Seastar`
- this is the result from `make ceph-osd -j`
- if the design is indeed centralized and the OSDs are only 1 : 1 relationship,
- then simply implementing Sestar with client or server is suffice
- use `sudocmd ceph_seastar_msg custom_makefile_test`

### integrating Seastar with Ceph
- use the common header file

### found issues with the BOOST version
- boost_1_67_0 is used by ceph
- boost 1.58.0 is used by seastar
- need to find a match

```
wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2
bzip2 -d boost_1_67_0.tar.bz2
tar -xvf boost_1_67_0.tar
cd boost_1_67_0
./bootstrap.sh --prefix=/home/sungho/usr && ./b2 stage threading=multi link=shared
./b2 install --prefix=/home/sungho/usr
export PATH="home/sungho/usr/lib/:$PATH"

dpkg -S /usr/include/boost/version.hpp
dpkg -S /home/sungho/ceph/build/boost/src/Boost/boost/version.hpp
```


### bug found on the boost library in Ceph
- include/boost/lockfree/spsc_queue.hpp
- Add #include <boost/next_prior.hpp>; no longer in utility.hpp

```
cd /home/sungho/ceph/build
boost/include/boost/lockfree/spsc_queue.hpp
boost/src/Boost/boost/lockfree/spsc_queue.hpp                                                           │sungho@wenji-w1:~$



cd /home/sungho/ceph/build/src && /usr/bin/c++  -DCEPH_LIBDIR=\"/usr/local/lib\" -DCEPH_PKGLIBDIR=\"/usr/local/lib/ceph\" -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D__linux__ -isystem /home/sungho/ceph/build/boost/include -I/home/sungho/ceph/build/src/include -I/home/sungho/ceph/src -isystem /home/sungho/ceph/build/include -I/usr/include/nss -I/usr/include/nspr -isystem /home/sungho/ceph/src/xxHash -isystem /home/sungho/ceph/src/rapidjson/include -I/home/sungho/ceph/src/dmclock/src -I/home/sungho/ceph/src/dmclock/support/src -isystem /home/sungho/ceph/src/googletest/googletest/include -isystem /home/sungho/ceph/src/spdk/include -isystem /home/sungho/ceph/build/src/dpdk/include -isystem /home/sungho/ceph/src/rocksdb/include  -Wall -Wtype-limits -Wignored-qualifiers -Winit-self -Wpointer-arith -Werror=format-security -fno-strict-aliasing -fsigned-char -Wno-unknown-pragmas -rdynamic  -ftemplate-depth-1024 -Wnon-virtual-dtor -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wstrict-null-sentinel -Woverloaded-virtual -fno-new-ttp-matching -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fstack-protector-strong -std=c++17 -fdiagnostics-color=auto -I/usr/include -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -O2 -g  -fPIE   -DHAVE_CONFIG_H -D__CEPH__ -D_REENTRANT -D_THREAD_SAFE -D__STDC_FORMAT_MACROS `pkg-config --cflags --libs /home/sungho/seastar/build/release/seastar.pc` -o CMakeFiles/ceph-osd.dir/ceph_osd.cc.o -c /home/sungho/ceph/src/ceph_osd.cc



```
