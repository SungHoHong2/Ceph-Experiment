
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
sudo make
sudo make install
```

```c++
src/msg
src/msg/simple


Monitor && RADOSCLIENT

void OSDService::send_message_osd_cluster(int peer, Message *m, epoch_t from_epoch)
run_recovery_op
send_message_peer


client application






Messenger *Messenger::create_client_messenger(CephContext *cct, string lname)

int Pipe::read_message(Message **pm, AuthSessionHandler* auth_handler)
if (tcp_read_wait() < 0)
map<ceph_tid_t,pair<bufferlist,int> >::iterator p = connection_state->rx_buffers.find(header.tid);
if (tcp_read((char*)&footer, sizeof(footer)) < 0)
ssize_t got = tcp_read_nonblocking(buf, len);
ssize_t got = buffered_recv(buf, len, MSG_DONTWAIT );
ssize_t got = do_recv(recv_buf, recv_max_prefetch, flags);
ssize_t got = ::recv( sd, buf, len, flags );

int Pipe::write_message(const ceph_msg_header& header, const ceph_msg_footer& footer, bufferlist& blist)
int Pipe::do_sendmsg(struct msghdr *msg, unsigned len, bool more)
::sendmsg(sd, msg, MSG_NOSIGNAL | (more ? MSG_MORE : 0));

void Pipe::start_reader()
void Pipe::start_writer()

// semd
int Pipe::do_sendmsg(struct msghdr *msg, unsigned len, bool more)
r = ::sendmsg(sd, msg, MSG_NOSIGNAL | (more ? MSG_MORE : 0));

// recv
ssize_t Pipe::do_recv(char *buf, size_t len, int flags)
ssize_t got = ::recv( sd, buf, len, flags );

```
