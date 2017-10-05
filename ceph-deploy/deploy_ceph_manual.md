### Preface
- allow all nodes to have password-less ssh connection
- add hosts to easily recognize the ip-address by string
  - admin, child1, child2

<br>

### Download Ceph
```
git clone --recursive https://github.com/ceph/ceph.git
git submodule update --force --init --recursive
git checkout -b luminous  
```
<br>

### download dependencies
```
./install-deps.sh
./do_cmake.sh
cd build
make
sudo make install
```
