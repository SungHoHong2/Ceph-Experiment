> all the nodes check for connections before sending the data <br>
> this includes `clients` `object` `monitor` and `managers` <br>

> When pipe connection is connected <br>
> simple messenger is initialized

```c
bool PipeConnection::is_connected()
{
  return static_cast<SimpleMessenger*>(msgr)->is_connected(this);
}
```

```c
SimpleMessenger::SimpleMessenger(CephContext *cct, entity_name_t name,
				 string mname, uint64_t _nonce)
  : SimplePolicyMessenger(cct, name,mname, _nonce),
    accepter(this, _nonce),
    dispatch_queue(cct, this, mname),
    reaper_thread(this),
    nonce(_nonce),
    lock("SimpleMessenger::lock"), need_addr(true), did_bind(false),
    global_seq(0),
    cluster_protocol(0),
    reaper_started(false), reaper_stop(false),
    timeout(0),
    local_connection(new PipeConnection(cct, this))
{
  ANNOTATE_BENIGN_RACE_SIZED(&timeout, sizeof(timeout),
                             "SimpleMessenger read timeout");
  ceph_spin_init(&global_seq_lock);
  init_local_connection();
}
```

<br>

> all the nodes use the send_message method to send the data <br>
> this includes `clients` `object` `monitor` and `managers`

```c
void MDSMonitor::check_sub(Subscription *sub)
{
  dout(20) << __func__ << ": " << sub->type << dendl;

  if (sub->type == "fsmap") {
    if (sub->next <= fsmap.get_epoch()) {
      sub->session->con->send_message(new MFSMap(mon->monmap->fsid, fsmap));
      if (sub->onetime) {
        mon->session_map.remove_sub(sub);
      } else {
        sub->next = fsmap.get_epoch() + 1;
      }
    }
  }
}
```

<br>

> send the message

```c
int send_message(Message *m, const entity_inst_t& dest) override {
  return _send_message(m, dest);
}
```

> [Message](Message.md) - stores the `data` `header` and `footer` <br>
> the message has the type, source and destination address <br>
> `enetity_inst_t` creates a list of destination that you have to send the message

```c
void entity_inst_t::generate_test_instances(list<entity_inst_t*>& o)
{
  o.push_back(new entity_inst_t());
  entity_name_t name;
  entity_addr_t addr;
  entity_inst_t *a = new entity_inst_t(name, addr);
  o.push_back(a);
}
```

<br>


> `simple messenger` is created and this creates multiple connections <br>

```c
int SimpleMessenger::_send_message(Message *m, Connection *con)
{
  //set envelope
  m->get_header().src = get_myname();

  if (!m->get_priority()) m->set_priority(get_default_send_priority());

  ldout(cct,1) << "--> " << con->get_peer_addr()
      << " -- " << *m
      << " -- ?+" << m->get_data().length()
      << " " << m << " con " << con
      << dendl;

  submit_message(m, static_cast<PipeConnection*>(con),
		 con->get_peer_addr(), con->get_peer_type(), false);
  return 0;
}
```

> extend the connection with PipeConnection

```c
class PipeConnection : public Connection {
  Pipe* pipe;

  friend class boost::intrusive_ptr<PipeConnection>;
  friend class Pipe;

public:

  PipeConnection(CephContext *cct, Messenger *m)
    : Connection(cct, m),
      pipe(NULL) { }

  ~PipeConnection() override;

  Pipe* get_pipe();

  bool try_get_pipe(Pipe** p);

  bool clear_pipe(Pipe* old_p);

  void reset_pipe(Pipe* p);

  bool is_connected() override;

  int send_message(Message *m) override;
  void send_keepalive() override;
  void mark_down() override;
  void mark_disposable() override;

}; /* PipeConnection */
```

```c
bool PipeConnection::is_connected()
{
  return static_cast<SimpleMessenger*>(msgr)->is_connected(this);
}

int PipeConnection::send_message(Message *m)
{
  assert(msgr);
  return static_cast<SimpleMessenger*>(msgr)->send_message(m, this);
}
```

```c
  /**
   * @defgroup Messaging
   * @{
   */
  int send_message(Message *m, const entity_inst_t& dest) override {
    return _send_message(m, dest);
  }

  int send_message(Message *m, Connection *con) {
    return _send_message(m, con);
  }
```


```c
int SimpleMessenger::_send_message(Message *m, const entity_inst_t& dest)
{
  // set envelope
  m->get_header().src = get_myname();
  m->set_cct(cct);

  if (!m->get_priority()) m->set_priority(get_default_send_priority());

  ldout(cct,1) <<"--> " << dest.name << " "
          << dest.addr << " -- " << *m
    	  << " -- ?+" << m->get_data().length()
	  << " " << m
	  << dendl;

  if (dest.addr == entity_addr_t()) {
    ldout(cct,0) << "send_message message " << *m
                 << " with empty dest " << dest.addr << dendl;
    m->put();
    return -EINVAL;
  }

  lock.Lock();
  Pipe *pipe = _lookup_pipe(dest.addr);
  submit_message(m, (pipe ? pipe->connection_state.get() : NULL),
                 dest.addr, dest.name.type(), true);
  lock.Unlock();
  return 0;
}


int SimpleMessenger::_send_message(Message *m, Connection *con)
{
  //set envelope
  m->get_header().src = get_myname();

  if (!m->get_priority()) m->set_priority(get_default_send_priority());

  ldout(cct,1) << "--> " << con->get_peer_addr()
      << " -- " << *m
      << " -- ?+" << m->get_data().length()
      << " " << m << " con " << con
      << dendl;

  submit_message(m, static_cast<PipeConnection*>(con),
		 con->get_peer_addr(), con->get_peer_type(), false);
  return 0;
}
```



```c
void SimpleMessenger::submit_message(Message *m, PipeConnection *con,
				     const entity_addr_t& dest_addr, int dest_type,
				     bool already_locked)
{
  m->trace.event("simple submitting message");
  if (cct->_conf->ms_dump_on_send) {
    m->encode(-1, true);
    ldout(cct, 0) << "submit_message " << *m << "\n";
    m->get_payload().hexdump(*_dout);
    if (m->get_data().length() > 0) {
      *_dout << " data:\n";
      m->get_data().hexdump(*_dout);
    }
    *_dout << dendl;
    m->clear_payload();
  }

  // existing connection?
  if (con) {
    Pipe *pipe = NULL;
    bool ok = static_cast<PipeConnection*>(con)->try_get_pipe(&pipe);
    if (!ok) {
      ldout(cct,0) << "submit_message " << *m << " remote, " << dest_addr
		   << ", failed lossy con, dropping message " << m << dendl;
      m->put();
      return;
    }
    while (pipe && ok) {
      // we loop in case of a racing reconnect, either from us or them
      pipe->pipe_lock.Lock(); // can't use a Locker because of the Pipe ref
      if (pipe->state != Pipe::STATE_CLOSED) {
	ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", have pipe." << dendl;
	pipe->_send(m);
	pipe->pipe_lock.Unlock();
	pipe->put();
	return;
      }
      Pipe *current_pipe;
      ok = con->try_get_pipe(&current_pipe);
      pipe->pipe_lock.Unlock();
      if (current_pipe == pipe) {
	ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr
		      << ", had pipe " << pipe << ", but it closed." << dendl;
	pipe->put();
	current_pipe->put();
	m->put();
	return;
      } else {
	pipe->put();
	pipe = current_pipe;
      }
    }
  }

  // local?
  if (my_inst.addr == dest_addr) {
    // local
    ldout(cct,20) << "submit_message " << *m << " local" << dendl;
    m->set_connection(local_connection.get());
    dispatch_queue.local_delivery(m, m->get_priority());
    return;
  }

  // remote, no existing pipe.
  const Policy& policy = get_policy(dest_type);
  if (policy.server) {
    ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", lossy server for target type "
		  << ceph_entity_type_name(dest_type) << ", no session, dropping." << dendl;
    m->put();
  } else {
    ldout(cct,20) << "submit_message " << *m << " remote, " << dest_addr << ", new pipe." << dendl;
    if (!already_locked) {
      /** We couldn't handle the Message without reference to global data, so
       *  grab the lock and do it again. If we got here, we know it's a non-lossy
       *  Connection, so we can use our existing pointer without doing another lookup. */
      Mutex::Locker l(lock);
      submit_message(m, con, dest_addr, dest_type, true);
    } else {
      connect_rank(dest_addr, dest_type, static_cast<PipeConnection*>(con), m);
    }
  }
}
```

```c
/**
 * The Pipe is the most complex SimpleMessenger component. It gets
 * two threads, one each for reading and writing on a socket it's handed
 * at creation time, and is responsible for everything that happens on
 * that socket. Besides message transmission, it's responsible for
 * propagating socket errors to the SimpleMessenger and then sticking
 * around in a state where it can provide enough data for the SimpleMessenger
 * to provide reliable Message delivery when it manages to reconnect.
 */

   class Pipe : public RefCountedObject {
     /**
      * The Reader thread handles all reads off the socket -- not just
      * Messages, but also acks and other protocol bits (excepting startup,
      * when the Writer does a couple of reads).
      * All the work is implemented in Pipe itself, of course.
      */
     class Reader : public Thread {
       Pipe *pipe;
     public:
       explicit Reader(Pipe *p) : pipe(p) {}
       void *entry() override { pipe->reader(); return 0; }
     } reader_thread;

     /**
      * The Writer thread handles all writes to the socket (after startup).
      * All the work is implemented in Pipe itself, of course.
      */
     class Writer : public Thread {
       Pipe *pipe;
     public:
       explicit Writer(Pipe *p) : pipe(p) {}
       void *entry() override { pipe->writer(); return 0; }
     } writer_thread;

     class DelayedDelivery;
     DelayedDelivery *delay_thread;
   public:
     Pipe(SimpleMessenger *r, int st, PipeConnection *con);
     ~Pipe() override;

     SimpleMessenger *msgr;
     uint64_t conn_id;
     ostream& _pipe_prefix(std::ostream &out) const;

     Pipe* get() {
       return static_cast<Pipe*>(RefCountedObject::get());
     }

     bool is_connected() {
       Mutex::Locker l(pipe_lock);
       return state == STATE_OPEN;
     }

     char *recv_buf;
     size_t recv_max_prefetch;
     size_t recv_ofs;
     size_t recv_len;

     enum {
       STATE_ACCEPTING,
       STATE_CONNECTING,
       STATE_OPEN,
       STATE_STANDBY,
       STATE_CLOSED,
       STATE_CLOSING,
       STATE_WAIT       // just wait for racing connection
     };

     static const char *get_state_name(int s) {
       switch (s) {
       case STATE_ACCEPTING: return "accepting";
       case STATE_CONNECTING: return "connecting";
       case STATE_OPEN: return "open";
       case STATE_STANDBY: return "standby";
       case STATE_CLOSED: return "closed";
       case STATE_CLOSING: return "closing";
       case STATE_WAIT: return "wait";
       default: return "UNKNOWN";
       }
     }
     const char *get_state_name() {
       return get_state_name(state);
     }

   private:
     int sd;
     struct iovec msgvec[SM_IOV_MAX];
 #if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE)
     sigset_t sigpipe_mask;
     bool sigpipe_pending;
     bool sigpipe_unblock;
 #endif

   public:
     int port;
     int peer_type;
     entity_addr_t peer_addr;
     Messenger::Policy policy;

     Mutex pipe_lock;
     int state;
     std::atomic<bool> state_closed = { false }; // true iff state = STATE_CLOSED

     // session_security handles any signatures or encryptions required for this pipe's msgs. PLR

     ceph::shared_ptr<AuthSessionHandler> session_security;

   protected:
     friend class SimpleMessenger;
     PipeConnectionRef connection_state;

     utime_t backoff;         // backoff time

     bool reader_running, reader_needs_join;
     bool reader_dispatching; /// reader thread is dispatching without pipe_lock
     bool notify_on_dispatch_done; /// something wants a signal when dispatch done
     bool writer_running;

     map<int, list<Message*> > out_q;  // priority queue for outbound msgs
     DispatchQueue *in_q;
     list<Message*> sent;
     Cond cond;
     bool send_keepalive;
     bool send_keepalive_ack;
     utime_t keepalive_ack_stamp;
     bool halt_delivery; //if a pipe's queue is destroyed, stop adding to it

     __u32 connect_seq, peer_global_seq;
     uint64_t out_seq;
     uint64_t in_seq, in_seq_acked;

     void set_socket_options();

     int accept();   // server handshake
     int connect();  // client handshake
     void reader();
     void writer();
     void unlock_maybe_reap();

     int randomize_out_seq();

     int read_message(Message **pm,
 		     AuthSessionHandler *session_security_copy);
     int write_message(const ceph_msg_header& h, const ceph_msg_footer& f, bufferlist& body);
     /**
      * Write the given data (of length len) to the Pipe's socket. This function
      * will loop until all passed data has been written out.
      * If more is set, the function will optimize socket writes
      * for additional data (by passing the MSG_MORE flag, aka TCP_CORK).
      *
      * @param msg The msghdr to write out
      * @param len The length of the data in msg
      * @param more Should be set true if this is one part of a larger message
      * @return 0, or -1 on failure (unrecoverable -- close the socket).
      */
     int do_sendmsg(struct msghdr *msg, unsigned len, bool more=false);
     int write_ack(uint64_t s);
     int write_keepalive();
     int write_keepalive2(char tag, const utime_t &t);

     void suppress_sigpipe();
     void restore_sigpipe();


     void fault(bool reader=false);

     void was_session_reset();

     /* Clean up sent list */
     void handle_ack(uint64_t seq);

     public:
     Pipe(const Pipe& other);
     const Pipe& operator=(const Pipe& other);

     void start_reader();
     void start_writer();
     void maybe_start_delay_thread();
     void join_reader();

     // public constructors
     static const Pipe& Server(int s);
     static const Pipe& Client(const entity_addr_t& pi);

     uint64_t get_out_seq() { return out_seq; }

     bool is_queued() { return !out_q.empty() || send_keepalive || send_keepalive_ack; }

     entity_addr_t& get_peer_addr() { return peer_addr; }

     void set_peer_addr(const entity_addr_t& a) {
       if (&peer_addr != &a)  // shut up valgrind
         peer_addr = a;
       connection_state->set_peer_addr(a);
     }
     void set_peer_type(int t) {
       peer_type = t;
       connection_state->set_peer_type(t);
     }

     void register_pipe();
     void unregister_pipe();
     void join();
     /// stop a Pipe by closing its socket and setting it to STATE_CLOSED
     void stop();
     /// stop() a Pipe if not already done, and wait for it to finish any
     /// fast_dispatch in progress.
     void stop_and_wait();

     void _send(Message *m) {
       assert(pipe_lock.is_locked());
       out_q[m->get_priority()].push_back(m);
       cond.Signal();
     }
     void _send_keepalive() {
       assert(pipe_lock.is_locked());
       send_keepalive = true;
       cond.Signal();
     }
     Message *_get_next_outgoing() {
       assert(pipe_lock.is_locked());
       Message *m = 0;
       while (!m && !out_q.empty()) {
         map<int, list<Message*> >::reverse_iterator p = out_q.rbegin();
         if (!p->second.empty()) {
           m = p->second.front();
           p->second.pop_front();
         }
         if (p->second.empty())
           out_q.erase(p->first);
       }
       return m;
     }

     /// move all messages in the sent list back into the queue at the highest priority.
     void requeue_sent();
     /// discard messages requeued by requeued_sent() up to a given seq
     void discard_requeued_up_to(uint64_t seq);
     void discard_out_queue();

     void shutdown_socket() {
       recv_reset();
       if (sd >= 0)
         ::shutdown(sd, SHUT_RDWR);
     }

     void recv_reset() {
       recv_len = 0;
       recv_ofs = 0;
     }
     ssize_t do_recv(char *buf, size_t len, int flags);
     ssize_t buffered_recv(char *buf, size_t len, int flags);
     bool has_pending_data() { return recv_len > recv_ofs; }

     /**
      * do a blocking read of len bytes from socket
      *
      * @param buf buffer to read into
      * @param len exact number of bytes to read
      * @return 0 for success, or -1 on error
      */
     int tcp_read(char *buf, unsigned len);

     /**
      * wait for bytes to become available on the socket
      *
      * @return 0 for success, or -1 on error
      */
     int tcp_read_wait();

     /**
      * non-blocking read of available bytes on socket
      *
      * This is expected to be used after tcp_read_wait(), and will return
      * an error if there is no data on the socket to consume.
      *
      * @param buf buffer to read into
      * @param len maximum number of bytes to read
      * @return bytes read, or -1 on error or when there is no data
      */
     ssize_t tcp_read_nonblocking(char *buf, unsigned len);

     /**
      * blocking write of bytes to socket
      *
      * @param buf buffer
      * @param len number of bytes to write
      * @return 0 for success, or -1 on error
      */
     int tcp_write(const char *buf, unsigned len);

   };


```



```c
struct RefCountedObject {
private:
  mutable std::atomic<uint64_t> nref;
  CephContext *cct;
public:
  RefCountedObject(CephContext *c = NULL, int n=1) : nref(n), cct(c) {}
  virtual ~RefCountedObject() {
    assert(nref == 0);
  }

  const RefCountedObject *get() const {
    int v = ++nref;
    if (cct)
      lsubdout(cct, refs, 1) << "RefCountedObject::get " << this << " "
			     << (v - 1) << " -> " << v
			     << dendl;
    return this;
  }
  RefCountedObject *get() {
    int v = ++nref;
    if (cct)
      lsubdout(cct, refs, 1) << "RefCountedObject::get " << this << " "
			     << (v - 1) << " -> " << v
			     << dendl;
    return this;
  }
  void put() const {
    CephContext *local_cct = cct;
    int v = --nref;
    if (v == 0) {
      ANNOTATE_HAPPENS_AFTER(&nref);
      ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(&nref);
      delete this;
    } else {
      ANNOTATE_HAPPENS_BEFORE(&nref);
    }
    if (local_cct)
      lsubdout(local_cct, refs, 1) << "RefCountedObject::put " << this << " "
				   << (v + 1) << " -> " << v
				   << dendl;
  }
  void set_cct(CephContext *c) {
    cct = c;
  }

  uint64_t get_nref() const {
    return nref;
  }
};
```
