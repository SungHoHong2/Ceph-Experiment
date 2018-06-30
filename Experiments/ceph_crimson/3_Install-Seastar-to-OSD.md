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
