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
