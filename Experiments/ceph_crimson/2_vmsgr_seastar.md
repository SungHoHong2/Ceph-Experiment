### Experiment of VMSGR-V2
- we need to also buy batteries on the way.
- the design initialize the listen and connection service
- both the listen and the connection service sends the data whenever there is a pending data
- If I can identify the `pending data` in Ceph, then I will be able to send the data right away.
- So in short, let the POSIX run independently and do its job.
- Just add `additional Seastar connections that handle the OSD operations`


```c++
// src/new-vmsgrd.cc
int main(int ac, char **av);
    app_template app;
    VMessenger *vm = vm->Instance();

    /* Construct listen list */
    vm->llisn = vm->vm_mem->construct<joblist_t>
        (llisn_nm())(vm->vm_mem->get_segment_manager());

    /* Construct connected list */
    vm->lconned = vm->vm_mem->construct<joblist_t>
        (lconned_nm())(vm->vm_mem->get_segment_manager());

    /* Construct connection requst list */
    vm->lconn = vm->vm_mem->construct<joblist_t>
        (lconn_nm())(vm->vm_mem->get_segment_manager());

    // distribute the listener and the connector == shard listener and connector
    return app.run_deprecated(ac, av, [&app]);
          VMessenger *vm = vm->Instance();
          return repeat([vm]);

          if (!vm->llisn->empty());
          job_t job = vm->llisn->front();
          vm->llisn->pop_front();
          auto lisn_distr = new distributed<Listener>;
          lisn_distr->start();
          lisn_distr->invoke_on_all(&Listener::listen_ports, job);

          if (!vm->lconn->empty());
          job_t job = vm->lconn->front();
          vm->lconn->pop_front();
          auto conn_distr = new distributed<Connector>;
          conn_distr->start();
          conn_distr->invoke_on_all(&Connector::start, job);
          return stop_iteration::no;

```

- listen shard
```c++
future<> listen_ports(job_t job);
    VMessenger *vmg = vmg->Instance();
        VMessenger* VMessenger::Instance();
             static VMessenger *vmsgr_srv = new VMessenger();
                  VMessenger::VMessenger():cur_conn_id(1), conn_core_id(1);

    seastar::keep_doing([vmg, job]());
        seastar::listen_options lo;
        lo.proto = transport::TCP;
        lo.reuse_address = true;
        seastar::ipv4_addr addr({job.port});
            seastar::do_with();
                  seastar::listen(seastar::make_ipv4_address(addr), lo),[job](auto &listener);
                  listener.accept();
                  VMConn *conn = new VMConn(std::move(s),std::move(a),vmg, job);
                  conn->process();
                      when_all(process_read(), process_write()).discard_result());

                            // process read loop BEGIN
                            future<> VMConn::process_read();
                                _in.read();
                                vmsgr_cc.debug("RX: Recv {}\n ", buf.get());
                                sleep(1us);
                                this->process_read();
                            // process read loop END

                            // process write loop BEGIN
                            future<> VMConn::process_write();
                                // when there is nothing to send just continue the loop
                                out_buf->empty();
                                sleep(1us);
                                          return this->process_write();
                                _out.write(out_buf->front().c_str());
                                this->process_write();
                            // process write loop END

```

- connect shard
- this part is necessary only when we implement Ceph librados `client part`
- we will ignore this implementation part for the moment.

```c++
    future<> start(job_t job);
    VMessenger *vmg = vmg->Instance();
    transport protocol = transport::TCP;

    engine().net().connect(make_ipv4_address(ipv4_addr{server_addr}), local, protocol);
    struct sockaddr_in posix_addr = {
       .sin_family = AF_INET,
       .sin_port = htons(job.peer_port),
       .sin_addr = job.peer_sin_addr,
    };
    seastar::socket_address ss_addr(posix_addr);
    VMConn *conn = new VMConn(std::move(fd),std::move(ss_addr), vmg, job);
    conn->process();

```
