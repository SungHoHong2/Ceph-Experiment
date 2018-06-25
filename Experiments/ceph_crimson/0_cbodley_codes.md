### problems
Seastar context is different
Seastar IP address will be different for DPDK

### Ceph-Crimson
lets first see how original Ceph work without the help of ceph
then let us think how to swap the results or just reference the output
or build an entirely new one

**install**
```
git clone --recursive https://github.com/ceph/ceph.git
git submodule update --force --init --recursive
git checkout -b luminous
./install-deps.sh
./do_cmake.sh
cd build
sudo make -j 8
sudo make install -j 8

sudo apt-get install python-setuptools python-dev

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 100
sudo update-alternatives --config gcc

locale.Error: unsupported locale setting

// when there is a problem with python
export LC_ALL=C
```
