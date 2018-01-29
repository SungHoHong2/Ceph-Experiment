```c
class Message : public RefCountedObject {
protected:
  ceph_msg_header  header;      // headerelope
  ceph_msg_footer  footer;
  bufferlist       payload;  // "front" unaligned blob
  bufferlist       middle;   // "middle" unaligned blob
  bufferlist       data;     // data payload (page-alignment will be preserved where possible)

  /* recv_stamp is set when the Messenger starts reading the
   * Message off the wire */
  utime_t recv_stamp;
  /* dispatch_stamp is set when the Messenger starts calling dispatch() on
   * its endpoints */
  utime_t dispatch_stamp;
  /* throttle_stamp is the point at which we got throttle */
  utime_t throttle_stamp;
  /* time at which message was fully read */
  utime_t recv_complete_stamp;

  ConnectionRef connection;

  uint32_t magic = 0;

  bi::list_member_hook<> dispatch_q;

public:
  // zipkin tracing
  ZTracer::Trace trace;
  void encode_trace(bufferlist &bl, uint64_t features) const;
  void decode_trace(bufferlist::iterator &p, bool create = false);

  class CompletionHook : public Context {
  protected:
    Message *m;
    friend class Message;
  public:
    explicit CompletionHook(Message *_m) : m(_m) {}
    virtual void set_message(Message *_m) { m = _m; }
  };

  typedef bi::list< Message,
		    bi::member_hook< Message,
				     bi::list_member_hook<>,
				     &Message::dispatch_q > > Queue;

protected:
  CompletionHook* completion_hook = nullptr; // owned by Messenger

  // release our size in bytes back to this throttler when our payload
  // is adjusted or when we are destroyed.
  Throttle *byte_throttler = nullptr;

  // release a count back to this throttler when we are destroyed
  Throttle *msg_throttler = nullptr;

  // keep track of how big this message was when we reserved space in
  // the msgr dispatch_throttler, so that we can properly release it
  // later.  this is necessary because messages can enter the dispatch
  // queue locally (not via read_message()), and those are not
  // currently throttled.
  uint64_t dispatch_throttle_size = 0;

  friend class Messenger;

public:
  Message() {
    memset(&header, 0, sizeof(header));
    memset(&footer, 0, sizeof(footer));
  }
  Message(int t, int version=1, int compat_version=0) {
    memset(&header, 0, sizeof(header));
    header.type = t;
    header.version = version;
    header.compat_version = compat_version;
    header.priority = 0;  // undef
    header.data_off = 0;
    memset(&footer, 0, sizeof(footer));
  }

  Message *get() {
    return static_cast<Message *>(RefCountedObject::get());
  }

protected:
  ~Message() override {
    if (byte_throttler)
      byte_throttler->put(payload.length() + middle.length() + data.length());
    release_message_throttle();
    trace.event("message destructed");
    /* call completion hooks (if any) */
    if (completion_hook)
      completion_hook->complete(0);
  }
public:
  const ConnectionRef& get_connection() const { return connection; }
  void set_connection(const ConnectionRef& c) {
    connection = c;
  }
  CompletionHook* get_completion_hook() { return completion_hook; }
  void set_completion_hook(CompletionHook *hook) { completion_hook = hook; }
  void set_byte_throttler(Throttle *t) { byte_throttler = t; }
  Throttle *get_byte_throttler() { return byte_throttler; }
  void set_message_throttler(Throttle *t) { msg_throttler = t; }
  Throttle *get_message_throttler() { return msg_throttler; }

  void set_dispatch_throttle_size(uint64_t s) { dispatch_throttle_size = s; }
  uint64_t get_dispatch_throttle_size() const { return dispatch_throttle_size; }

  const ceph_msg_header &get_header() const { return header; }
  ceph_msg_header &get_header() { return header; }
  void set_header(const ceph_msg_header &e) { header = e; }
  void set_footer(const ceph_msg_footer &e) { footer = e; }
  const ceph_msg_footer &get_footer() const { return footer; }
  ceph_msg_footer &get_footer() { return footer; }
  void set_src(const entity_name_t& src) { header.src = src; }

  uint32_t get_magic() const { return magic; }
  void set_magic(int _magic) { magic = _magic; }

  /*
   * If you use get_[data, middle, payload] you shouldn't
   * use it to change those bufferlists unless you KNOW
   * there is no throttle being used. The other
   * functions are throttling-aware as appropriate.
   */

  void clear_payload() {
    if (byte_throttler) {
      byte_throttler->put(payload.length() + middle.length());
    }
    payload.clear();
    middle.clear();
  }

  virtual void clear_buffers() {}
  void clear_data() {
    if (byte_throttler)
      byte_throttler->put(data.length());
    data.clear();
    clear_buffers(); // let subclass drop buffers as well
  }
  void release_message_throttle() {
    if (msg_throttler)
      msg_throttler->put();
    msg_throttler = nullptr;
  }

  bool empty_payload() const { return payload.length() == 0; }
  bufferlist& get_payload() { return payload; }
  void set_payload(bufferlist& bl) {
    if (byte_throttler)
      byte_throttler->put(payload.length());
    payload.claim(bl, buffer::list::CLAIM_ALLOW_NONSHAREABLE);
    if (byte_throttler)
      byte_throttler->take(payload.length());
  }

  void set_middle(bufferlist& bl) {
    if (byte_throttler)
      byte_throttler->put(middle.length());
    middle.claim(bl, buffer::list::CLAIM_ALLOW_NONSHAREABLE);
    if (byte_throttler)
      byte_throttler->take(middle.length());
  }
  bufferlist& get_middle() { return middle; }

  void set_data(const bufferlist &bl) {
    if (byte_throttler)
      byte_throttler->put(data.length());
    data.share(bl);
    if (byte_throttler)
      byte_throttler->take(data.length());
  }

  const bufferlist& get_data() const { return data; }
  bufferlist& get_data() { return data; }
  void claim_data(bufferlist& bl,
		  unsigned int flags = buffer::list::CLAIM_DEFAULT) {
    if (byte_throttler)
      byte_throttler->put(data.length());
    bl.claim(data, flags);
  }
  off_t get_data_len() const { return data.length(); }

  void set_recv_stamp(utime_t t) { recv_stamp = t; }
  const utime_t& get_recv_stamp() const { return recv_stamp; }
  void set_dispatch_stamp(utime_t t) { dispatch_stamp = t; }
  const utime_t& get_dispatch_stamp() const { return dispatch_stamp; }
  void set_throttle_stamp(utime_t t) { throttle_stamp = t; }
  const utime_t& get_throttle_stamp() const { return throttle_stamp; }
  void set_recv_complete_stamp(utime_t t) { recv_complete_stamp = t; }
  const utime_t& get_recv_complete_stamp() const { return recv_complete_stamp; }

  void calc_header_crc() {
    header.crc = ceph_crc32c(0, (unsigned char*)&header,
			     sizeof(header) - sizeof(header.crc));
  }
  void calc_front_crc() {
    footer.front_crc = payload.crc32c(0);
    footer.middle_crc = middle.crc32c(0);
  }
  void calc_data_crc() {
    footer.data_crc = data.crc32c(0);
  }

  virtual int get_cost() const {
    return data.length();
  }

  // type
  int get_type() const { return header.type; }
  void set_type(int t) { header.type = t; }

  uint64_t get_tid() const { return header.tid; }
  void set_tid(uint64_t t) { header.tid = t; }

  uint64_t get_seq() const { return header.seq; }
  void set_seq(uint64_t s) { header.seq = s; }

  unsigned get_priority() const { return header.priority; }
  void set_priority(__s16 p) { header.priority = p; }

  // source/dest
  entity_inst_t get_source_inst() const {
    return entity_inst_t(get_source(), get_source_addr());
  }
  entity_name_t get_source() const {
    return entity_name_t(header.src);
  }
  entity_addr_t get_source_addr() const {
    if (connection)
      return connection->get_peer_addr();
    return entity_addr_t();
  }

  // forwarded?
  entity_inst_t get_orig_source_inst() const {
    return get_source_inst();
  }
  entity_name_t get_orig_source() const {
    return get_source();
  }
  entity_addr_t get_orig_source_addr() const {
    return get_source_addr();
  }

  // virtual bits
  virtual void decode_payload() = 0;
  virtual void encode_payload(uint64_t features) = 0;
  virtual const char *get_type_name() const = 0;
  virtual void print(ostream& out) const {
    out << get_type_name() << " magic: " << magic;
  }

  virtual void dump(Formatter *f) const;

  void encode(uint64_t features, int crcflags);
};
```
