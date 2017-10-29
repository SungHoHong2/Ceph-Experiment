
class Objecter : public md_config_obs_t, public Dispatcher {
public:
  // config observer bits
  const char** get_tracked_conf_keys() const override;
  void handle_conf_change(const struct md_config_t *conf,
                          const std::set <std::string> &changed) override;

public:
  Messenger *messenger;
  MonClient *monc;
  Finisher *finisher;
  ZTracer::Endpoint trace_endpoint;
private:
  OSDMap    *osdmap;
public:
  using Dispatcher::cct;
  std::multimap<string,string> crush_location;

  std::atomic<bool> initialized{false};

private:
  std::atomic<uint64_t> last_tid{0};
  std::atomic<unsigned> inflight_ops{0};
  std::atomic<int> client_inc{-1};
  uint64_t max_linger_id;
  std::atomic<unsigned> num_in_flight{0};
  std::atomic<int> global_op_flags{0}; // flags which are applied to each IO op
  bool keep_balanced_budget;
  bool honor_osdmap_full;
  bool osdmap_full_try;

  // If this is true, accumulate a set of blacklisted entities
  // to be drained by consume_blacklist_events.
  bool blacklist_events_enabled;
  std::set<entity_addr_t> blacklist_events;

public:
  void maybe_request_map();

  void enable_blacklist_events();
private:

  void _maybe_request_map();

  version_t last_seen_osdmap_version;
  version_t last_seen_pgmap_version;

  mutable boost::shared_mutex rwlock;
  using lock_guard = std::unique_lock<decltype(rwlock)>;
  using unique_lock = std::unique_lock<decltype(rwlock)>;
  using shared_lock = boost::shared_lock<decltype(rwlock)>;
  using shunique_lock = ceph::shunique_lock<decltype(rwlock)>;
  ceph::timer<ceph::mono_clock> timer;

  PerfCounters *logger;

  uint64_t tick_event;

  void start_tick();
  void tick();
  void update_crush_location();

  class RequestStateHook;

  RequestStateHook *m_request_state_hook;

public:
  /*** track pending operations ***/
  // read
 public:

  struct OSDSession;

  struct op_target_t {
    int flags = 0;

    epoch_t epoch = 0;  ///< latest epoch we calculated the mapping

    object_t base_oid;
    object_locator_t base_oloc;
    object_t target_oid;
    object_locator_t target_oloc;

    ///< true if we are directed at base_pgid, not base_oid
    bool precalc_pgid = false;

    ///< true if we have ever mapped to a valid pool
    bool pool_ever_existed = false;

    ///< explcit pg target, if any
    pg_t base_pgid;

    pg_t pgid; ///< last (raw) pg we mapped to
    spg_t actual_pgid; ///< last (actual) spg_t we mapped to
    unsigned pg_num = 0; ///< last pg_num we mapped to
    unsigned pg_num_mask = 0; ///< last pg_num_mask we mapped to
    vector<int> up; ///< set of up osds for last pg we mapped to
    vector<int> acting; ///< set of acting osds for last pg we mapped to
    int up_primary = -1; ///< last up_primary we mapped to
    int acting_primary = -1;  ///< last acting_primary we mapped to
    int size = -1; ///< the size of the pool when were were last mapped
    int min_size = -1; ///< the min size of the pool when were were last mapped
    bool sort_bitwise = false; ///< whether the hobject_t sort order is bitwise
    bool recovery_deletes = false; ///< whether the deletes are performed during recovery instead of peering

    bool used_replica = false;
    bool paused = false;

    int osd = -1;      ///< the final target osd, or -1

    epoch_t last_force_resend = 0;

    op_target_t(object_t oid, object_locator_t oloc, int flags)
      : flags(flags),
	base_oid(oid),
	base_oloc(oloc)
      {}

    op_target_t(pg_t pgid)
      : base_oloc(pgid.pool(), pgid.ps()),
	precalc_pgid(true),
	base_pgid(pgid)
      {}

    op_target_t() = default;

    hobject_t get_hobj() {
      return hobject_t(target_oid,
		       target_oloc.key,
		       CEPH_NOSNAP,
		       target_oloc.hash >= 0 ? target_oloc.hash : pgid.ps(),
		       target_oloc.pool,
		       target_oloc.nspace);
    }

    bool contained_by(const hobject_t& begin, const hobject_t& end) {
      hobject_t h = get_hobj();
      int r = cmp(h, begin);
      return r == 0 || (r > 0 && h < end);
    }

    void dump(Formatter *f) const;
  };

  struct Op : public RefCountedObject {
    OSDSession *session;
    int incarnation;

    op_target_t target;

    ConnectionRef con;  // for rx buffer only
    uint64_t features;  // explicitly specified op features

    vector<OSDOp> ops;

    snapid_t snapid;
    SnapContext snapc;
    ceph::real_time mtime;

    bufferlist *outbl;
    vector<bufferlist*> out_bl;
    vector<Context*> out_handler;
    vector<int*> out_rval;

    int priority;
    Context *onfinish;
    uint64_t ontimeout;

    ceph_tid_t tid;
    int attempts;

    version_t *objver;
    epoch_t *reply_epoch;

    ceph::mono_time stamp;

    epoch_t map_dne_bound;

    bool budgeted;

    /// true if we should resend this message on failure
    bool should_resend;

    /// true if the throttle budget is get/put on a series of OPs,
    /// instead of per OP basis, when this flag is set, the budget is
    /// acquired before sending the very first OP of the series and
    /// released upon receiving the last OP reply.
    bool ctx_budgeted;

    int *data_offset;

    osd_reqid_t reqid; // explicitly setting reqid
    ZTracer::Trace trace;

    Op(const object_t& o, const object_locator_t& ol, vector<OSDOp>& op,
       int f, Context *fin, version_t *ov, int *offset = NULL,
       ZTracer::Trace *parent_trace = nullptr) :
      session(NULL), incarnation(0),
      target(o, ol, f),
      con(NULL),
      features(CEPH_FEATURES_SUPPORTED_DEFAULT),
      snapid(CEPH_NOSNAP),
      outbl(NULL),
      priority(0),
      onfinish(fin),
      ontimeout(0),
      tid(0),
      attempts(0),
      objver(ov),
      reply_epoch(NULL),
      map_dne_bound(0),
      budgeted(false),
      should_resend(true),
      ctx_budgeted(false),
      data_offset(offset) {
      ops.swap(op);

      /* initialize out_* to match op vector */
      out_bl.resize(ops.size());
      out_rval.resize(ops.size());
      out_handler.resize(ops.size());
      for (unsigned i = 0; i < ops.size(); i++) {
	out_bl[i] = NULL;
	out_handler[i] = NULL;
	out_rval[i] = NULL;
      }

      if (target.base_oloc.key == o)
	target.base_oloc.key.clear();

      if (parent_trace && parent_trace->valid()) {
        trace.init("op", nullptr, parent_trace);
        trace.event("start");
      }
    }

    bool operator<(const Op& other) const {
      return tid < other.tid;
    }

    bool respects_full() const {
      return
	(target.flags & (CEPH_OSD_FLAG_WRITE | CEPH_OSD_FLAG_RWORDERED)) &&
	!(target.flags & (CEPH_OSD_FLAG_FULL_TRY | CEPH_OSD_FLAG_FULL_FORCE));
    }

  private:
    ~Op() override {
      while (!out_handler.empty()) {
	delete out_handler.back();
	out_handler.pop_back();
      }
      trace.event("finish");
    }
  };
