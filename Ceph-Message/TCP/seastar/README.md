### SEASTAR Experiments
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
- [seastar threads](tutorialthreads.c)
- future and continuations
  - building blocks of asynchronous programming
  - `future` is a result of a computation that may not be available yet.
  - `continuation` is a callback (typically a lambda) to run when a future becomes available
