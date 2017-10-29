/*******************
 * Processor
 */

class Processor::C_processor_accept : public EventCallback {
  Processor *pro;

 public:
  explicit C_processor_accept(Processor *p): pro(p) {}
  void do_request(int id) override {
    pro->accept();
  }
};

Processor::Processor(AsyncMessenger *r, Worker *w, CephContext *c)
  : msgr(r), net(c), worker(w),
    listen_handler(new C_processor_accept(this)) {}

int Processor::bind(const entity_addr_t &bind_addr,
		    const set<int>& avoid_ports,
		    entity_addr_t* bound_addr)
{
  const md_config_t *conf = msgr->cct->_conf;
  // bind to a socket
  ldout(msgr->cct, 10) << __func__ << dendl;

  int family;
  switch (bind_addr.get_family()) {
    case AF_INET:
    case AF_INET6:
      family = bind_addr.get_family();
      break;

    default:
      // bind_addr is empty
      family = conf->ms_bind_ipv6 ? AF_INET6 : AF_INET;
  }

  SocketOptions opts;
  opts.nodelay = msgr->cct->_conf->ms_tcp_nodelay;
  opts.rcbuf_size = msgr->cct->_conf->ms_tcp_rcvbuf;

  // use whatever user specified (if anything)
  entity_addr_t listen_addr = bind_addr;
  if (listen_addr.get_type() == entity_addr_t::TYPE_NONE) {
    listen_addr.set_type(entity_addr_t::TYPE_LEGACY);
  }
  listen_addr.set_family(family);

  /* bind to port */
  int r = -1;

  for (int i = 0; i < conf->ms_bind_retry_count; i++) {
    if (i > 0) {
      lderr(msgr->cct) << __func__ << " was unable to bind. Trying again in "
                       << conf->ms_bind_retry_delay << " seconds " << dendl;
      sleep(conf->ms_bind_retry_delay);
    }

    if (listen_addr.get_port()) {
      worker->center.submit_to(worker->center.get_id(), [this, &listen_addr, &opts, &r]() {
        r = worker->listen(listen_addr, opts, &listen_socket);
      }, false);
      if (r < 0) {
        lderr(msgr->cct) << __func__ << " unable to bind to " << listen_addr
                         << ": " << cpp_strerror(r) << dendl;
        continue;
      }
    } else {
      // try a range of ports
      for (int port = msgr->cct->_conf->ms_bind_port_min; port <= msgr->cct->_conf->ms_bind_port_max; port++) {
        if (avoid_ports.count(port))
          continue;

        listen_addr.set_port(port);
        worker->center.submit_to(worker->center.get_id(), [this, &listen_addr, &opts, &r]() {
          r = worker->listen(listen_addr, opts, &listen_socket);
        }, false);
        if (r == 0)
          break;
      }
      if (r < 0) {
        lderr(msgr->cct) << __func__ << " unable to bind to " << listen_addr
                         << " on any port in range " << msgr->cct->_conf->ms_bind_port_min
                         << "-" << msgr->cct->_conf->ms_bind_port_max << ": "
                         << cpp_strerror(r) << dendl;
        listen_addr.set_port(0); // Clear port before retry, otherwise we shall fail again.
        continue;
      }
      ldout(msgr->cct, 10) << __func__ << " bound on random port " << listen_addr << dendl;
    }
    if (r == 0)
      break;
  }
  // It seems that binding completely failed, return with that exit status
  if (r < 0) {
    lderr(msgr->cct) << __func__ << " was unable to bind after " << conf->ms_bind_retry_count
                     << " attempts: " << cpp_strerror(r) << dendl;
    return r;
  }

  ldout(msgr->cct, 10) << __func__ << " bound to " << listen_addr << dendl;
  *bound_addr = listen_addr;
  return 0;
}

void Processor::start()
{
  ldout(msgr->cct, 1) << __func__ << dendl;

  // start thread
  if (listen_socket) {
    worker->center.submit_to(worker->center.get_id(), [this]() {
      worker->center.create_file_event(listen_socket.fd(), EVENT_READABLE, listen_handler); }, false);
  }
}

void Processor::accept()
{
  ldout(msgr->cct, 10) << __func__ << " listen_fd=" << listen_socket.fd() << dendl;
  SocketOptions opts;
  opts.nodelay = msgr->cct->_conf->ms_tcp_nodelay;
  opts.rcbuf_size = msgr->cct->_conf->ms_tcp_rcvbuf;
  opts.priority = msgr->get_socket_priority();
  while (true) {
    entity_addr_t addr;
    ConnectedSocket cli_socket;
    Worker *w = worker;
    if (!msgr->get_stack()->support_local_listen_table())
      w = msgr->get_stack()->get_worker();
    int r = listen_socket.accept(&cli_socket, opts, &addr, w);
    if (r == 0) {
      ldout(msgr->cct, 10) << __func__ << " accepted incoming on sd " << cli_socket.fd() << dendl;

      msgr->add_accept(w, std::move(cli_socket), addr);
      continue;
    } else {
      if (r == -EINTR) {
        continue;
      } else if (r == -EAGAIN) {
        break;
      } else if (r == -EMFILE || r == -ENFILE) {
        lderr(msgr->cct) << __func__ << " open file descriptions limit reached sd = " << listen_socket.fd()
                         << " errno " << r << " " << cpp_strerror(r) << dendl;
        break;
      } else if (r == -ECONNABORTED) {
        ldout(msgr->cct, 0) << __func__ << " it was closed because of rst arrived sd = " << listen_socket.fd()
                            << " errno " << r << " " << cpp_strerror(r) << dendl;
        continue;
      } else {
        lderr(msgr->cct) << __func__ << " no incoming connection?"
                         << " errno " << r << " " << cpp_strerror(r) << dendl;
        break;
      }
    }
  }
}

void Processor::stop()
{
  ldout(msgr->cct,10) << __func__ << dendl;

  if (listen_socket) {
    worker->center.submit_to(worker->center.get_id(), [this]() {
      worker->center.delete_file_event(listen_socket.fd(), EVENT_READABLE);
      listen_socket.abort_accept();
    }, false);
  }
}



struct StackSingleton {
  CephContext *cct;
  std::shared_ptr<NetworkStack> stack;

  StackSingleton(CephContext *c): cct(c) {}
  void ready(std::string &type) {
    if (!stack)
      stack = NetworkStack::create(cct, type);
  }
  ~StackSingleton() {
    stack->stop();
  }
};



class C_handle_reap : public EventCallback {
  AsyncMessenger *msgr;

  public:
  explicit C_handle_reap(AsyncMessenger *m): msgr(m) {}
  void do_request(int id) override {
    // judge whether is a time event
    msgr->reap_dead();
  }
};
