### SEASTAR Experiments
- creating shortcut
```
ln -s /home/sungho/Ceph-Experiment/Ceph-Message/TCP/seastar/send_recv seastar
```

- installing in workstation
```
// for some god damn reason don't every compile with g++-5 in the cluster
// it just does not work
git clone https://github.com/scylladb/seastar.git
sudo ./configure.py --enable-dpdk
sudo ninja

export SEASTAR=/home/sungho/seastar
g++ `pkg-config --cflags --libs $SEASTAR/build/release/seastar.pc` getting-started.cc -o hello
```

<br>

### Seastar Tutorial
- `app_template` starts the main event loop
- `app.run()` when the future returned by it resolves the app will shut down, stopping event loops on all cpus, and `app.run()` will return
- `app.run_deprecated()` application doesn't exit when the callback returns, explicitly stopped by calling `engine_exit()`

- [seastar threads](tutorialthreads.c)
  - cannot start more threads than the number of hardware threads
- seastar memory
  - Each thread is preallocated with a large piece of memory on the same NUMA node it is running on
  - uses only that memory for its allocations
  - By default, the machine's entire memory except a small reservation left for the OS (defaulting to 512 MB) is pre-allocated

- future and continuations
  - building blocks of asynchronous programming
  - `future` is a result of a computation that may not be available yet.
  - `continuation` is a callback (typically a lambda) to run when a future becomes available
