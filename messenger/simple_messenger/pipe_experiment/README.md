### Pipe Experiment

- Single messenger
- runs two threads, one each for reading and writing on a socket
- message transmission

```c
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
```

```c
void Pipe::start_reader()
{
  assert(pipe_lock.is_locked());
  assert(!reader_running);
  if (reader_needs_join) {
    reader_thread.join();
    reader_needs_join = false;
  }
  reader_running = true;
  reader_thread.create("ms_pipe_read", msgr->cct->_conf->ms_rwthread_stack_bytes);
}

void Pipe::start_writer()
{
  assert(pipe_lock.is_locked());
  assert(!writer_running);
  writer_running = true;
  writer_thread.create("ms_pipe_write", msgr->cct->_conf->ms_rwthread_stack_bytes);
}

int Pipe::do_sendmsg(struct msghdr *msg, unsigned len, bool more){
r = ::sendmsg(sd, msg, MSG_NOSIGNAL | (more ? MSG_MORE : 0));

int Pipe::read_message(Message **pm, AuthSessionHandler* auth_handler)
int Pipe::tcp_read(char *buf, unsigned len)
ssize_t Pipe::tcp_read_nonblocking(char *buf, unsigned len)
ssize_t got = buffered_recv(buf, len, MSG_DONTWAIT );
ssize_t ret = do_recv(buf, left, flags );
ssize_t got = ::recv( sd, buf, len, flags );

```
