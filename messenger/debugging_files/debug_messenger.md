### DEBUGGING MESSENGER

- **running command**
```
sudo ceph -s
```

<br>

- **/ceph/src/librados/RadosClient.cc**

```cpp
int librados::RadosClient::connect()
{
...
  messenger = Messenger::create_client_messenger(cct, "radosclient");
  if (!messenger)
    goto out;
...
```

<br>

- **messenger.c >> create_client_messenger**
    - get the message type from public or private configuration   
    - initiate number value and assign random number
    - pass Messenger with msg_type, entity_name_t(?), "radiosclient", random-number, 0
        - entity_name_t: is within msg_type.h

```cpp

// string lname = radosclient
// cct = configuration
Messenger *Messenger::create_client_messenger(CephContext *cct, string lname)
{
  std::string public_msgr_type = cct->_conf->ms_public_type.empty() ? cct->_conf->get_val<std::string>("ms_type") : cct->_conf->ms_public_type;
  uint64_t nonce = 0;
  get_random_bytes((char*)&nonce, sizeof(nonce));
  return Messenger::create(cct, public_msgr_type, entity_name_t::CLIENT(),
			   std::move(lname), nonce, 0);
}
```


<br>


- **messenger.c >> create**
    - check the message type and move on the AsyncMessenger

```cpp
Messenger *Messenger::create(CephContext *cct, const string &type,
			     entity_name_t name, string lname,
			     uint64_t nonce, uint64_t cflags)
{
  int r = -1;
  if (type == "random") {
    r = ceph::util::generate_random_number(0, 1);
  }
  if (r == 0 || type == "simple")
    return new SimpleMessenger(cct, name, std::move(lname), nonce);
  else if (r == 1 || type.find("async") != std::string::npos)
    return new AsyncMessenger(cct, name, type, std::move(lname), nonce);
#ifdef HAVE_XIO
  else if ((type == "xio") &&
	   cct->check_experimental_feature_enabled("ms-type-xio"))
    return new XioMessenger(cct, name, std::move(lname), nonce, cflags);
#endif
  lderr(cct) << "unrecognized ms_type '" << type << "'" << dendl;
  return nullptr;
}

// move is used to indicate that an object t may be "moved from", i.e. allowing the efficient transfer of resources from t to another object.
```


<br>


- **AsyncMessenger.c >> AsyncMessenger**
    - add transport type as posix
    - if other type was found in the string type, then switch to those
    - create a singleton object

```cpp
AsyncMessenger::AsyncMessenger(CephContext *cct, entity_name_t name,
                               const std::string &type, string mname, uint64_t _nonce)
  : SimplePolicyMessenger(cct, name,mname, _nonce),
    dispatch_queue(cct, this, mname),
    lock("AsyncMessenger::lock"),
    nonce(_nonce), need_addr(true), did_bind(false),
    global_seq(0), deleted_lock("AsyncMessenger::deleted_lock"),
    cluster_protocol(0), stopped(true)
{
  std::string transport_type = "posix";
  if (type.find("rdma") != std::string::npos)
    transport_type = "rdma";
  else if (type.find("dpdk") != std::string::npos)
    transport_type = "dpdk";


  StackSingleton *single;
  cct->lookup_or_create_singleton_object<StackSingleton>(single, "AsyncMessenger::NetworkStack::"+transport_type);
...
```

<br>

- **AsyncMessenger.c >> class StackSingleton**
    - call NetworkStack to create the singleton

```cpp
struct StackSingleton {
  CephContext *cct;
  std::shared_ptr<NetworkStack> stack;

  StackSingleton(CephContext *c): cct(c) {}
  void ready(std::string &type) {
    if (!stack)
      stack = NetworkStack::create(cct, type); // could not find beyond this
  }
  ~StackSingleton() {
    stack->stop();
  }
};
```

<br>

- **AsyncMessenger.c >> AsyncMessenger**
    - call ready function
    - start networkstack
    - get Worker Object from from (NetworkStack)[../reference_message/stack.h]
    - start NetworkStack

```cpp  
  single->ready(transport_type);
  stack = single->stack.get();
  stack->start();
  local_worker = stack->get_worker();
  local_connection = new AsyncConnection(cct, this, &dispatch_queue, local_worker);
```

<br>

- **stack.cc**
    - call spawnworkers from [PosixStack.h](../reference_message/PosixStack.h)

```cpp
void NetworkStack::start()
{
  std::unique_lock<decltype(pool_spin)> lk(pool_spin);

  if (started) {
    return ;
  }

  for (unsigned i = 0; i < num_workers; ++i) {
    if (workers[i]->is_init())
      continue;
    std::function<void ()> thread = add_thread(i);
    spawn_worker(i, std::move(thread));
  }
  started = true;
  lk.unlock();

  for (unsigned i = 0; i < num_workers; ++i)
    workers[i]->wait_for_init();
    /*
    #include<condition_variable>
    void wait_for_init() {
      std::unique_lock<std::mutex> l(init_lock);
      while (!init)
        init_cond.wait(l);
    }
    */
}
```


<br>

- **AsyncMessenger.h**

```cpp
AsyncConnection(CephContext *cct, AsyncMessenger *m, DispatchQueue *q, Worker *w);
~AsyncConnection() override;
void maybe_start_delay_thread();

ostream& _conn_prefix(std::ostream *_dout);

bool is_connected() override {
  return can_write.load() == WriteStatus::CANWRITE;
}
```

<br>


- **AsyncMessenger.c >> AsyncMessenger**

```cpp
  init_local_connection();
  /*
    void _init_local_connection() {
      assert(lock.is_locked());
      local_connection->peer_addr = my_inst.addr;
      local_connection->peer_type = my_inst.name.type();
      local_connection->set_features(CEPH_FEATURES_ALL);
      ms_deliver_handle_fast_connect(local_connection.get());
    }
  */

  reap_handler = new C_handle_reap(this);
  /*
  class C_handle_reap : public EventCallback {
    AsyncMessenger *msgr;

    public:
    explicit C_handle_reap(AsyncMessenger *m): msgr(m) {}
    void do_request(int id) override {
      // judge whether is a time event
      msgr->reap_dead();
    }
  };
  */

  unsigned processor_num = 1;
  if (stack->support_local_listen_table())
    processor_num = stack->get_num_worker();

  for (unsigned i = 0; i < processor_num; ++i)
    processors.push_back(new Processor(this, stack->get_worker(i), cct));
    /*
    class Processor {
      Processor(AsyncMessenger *r, Worker *w, CephContext *c);
    }

    Processor::Processor(AsyncMessenger *r, Worker *w, CephContext *c)
    : msgr(r), net(c), worker(w),
    listen_handler(new C_processor_accept(this)) {}
    */

```

<br>

- **stack.h**

```cpp

class NetworkStack : public CephContext::ForkWatcher {
  ...
  virtual bool support_local_listen_table() const { return false; }

  unsigned get_num_worker() const {
  return num_workers;
}
  ...
}

```

<br>

- **/ceph/src/librados/RadosClient.cc**
    - create [objector](../reference_message/Objecter.h)
    - [finisher](../reference_message/Finisher.h) is signaled after the operations are completed


```cpp
  objecter = new (std::nothrow) Objecter(cct, messenger, &monclient,
			  &finisher,
			  cct->_conf->rados_mon_op_timeout,
			  cct->_conf->rados_osd_op_timeout);

  if (!objecter)
    goto out;
  objecter->set_balanced_budget();

  /* in objecter.h
     void set_balanced_budget() { keep_balanced_budget = true; }
  */

  monclient.set_messenger(messenger);
  ... truncated

  mgrclient.set_messenger(messenger);
  ...truncated

  objecter->init();
  messenger->add_dispatcher_head(&mgrclient);
  messenger->add_dispatcher_tail(objecter);
  messenger->add_dispatcher_tail(this);

  messenger->start();

  objecter->set_client_incarnation(0);

  /* void set_client_incarnation(int inc) { client_inc = inc; } */


  objecter->start();
  /*  void start(const OSDMap *o = nullptr); */

  lock.Lock();

  timer.init();

  finisher.start(); // finisher actually starts the worker threads

  state = CONNECTED;
  instance_id = monclient.get_global_id();

  lock.Unlock();
...
}
```
