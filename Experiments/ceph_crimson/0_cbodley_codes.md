### Ceph-Crimson
lets first see how original Ceph work without the help of ceph
then let us think how to swap the results or just reference the output
or build an entirely new one

**install**
```
git clone https://github.com/cbodley/ceph.git
git submodule update --force --init --recursive
git checkout -b wip-seastar-msg
git pull origin wip-seastar-msg
./install-deps.sh
./do_cmake.sh
cd build
sudo make
sudo make install
```

```c++
src/msg
src/msg/simple






// semd
int Pipe::do_sendmsg(struct msghdr *msg, unsigned len, bool more)
r = ::sendmsg(sd, msg, MSG_NOSIGNAL | (more ? MSG_MORE : 0));

// recv
ssize_t Pipe::do_recv(char *buf, size_t len, int flags)
ssize_t got = ::recv( sd, buf, len, flags );



```


```
```
