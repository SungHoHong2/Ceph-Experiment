
class EventCallback {

 public:
  virtual void do_request(int fd_or_id) = 0;
  virtual ~EventCallback() {}       // we want a virtual destructor!!!
};

typedef EventCallback* EventCallbackRef;
