
### What is TCMalloc

In Ceph
Ceph MON, OSD and MDS can generate heap profiles using tcmalloc. To generate heap profiles, ensure you have google-perftools installed.

The profiler dumps output to your log file directory (i.e., /var/log/ceph). See Logging and Debugging for details. To view the profiler logs with Google’s performance tools, execute the following:

http://docs.ceph.com/docs/jewel/rados/troubleshooting/memory-profiling/

Although there is no option for removing the heap profile, the implementation and the output that this gives me the idea that it is an optional feature.




http://goog-perftools.sourceforge.net/doc/tcmalloc.html

TCMalloc can consume significant amount of CPU when it doesn’t have enough thread cache available.
There is a setting that allows the user to increase the amount of thread cache beyond the default 32MB.

TCMalloc is faster than the glibc 2.3 malloc (available as a separate library called ptmalloc2) and other mallocs
TCMalloc assigns each thread a thread-local cache. Small allocations are satisfied from the thread-local cache.
bjects are moved from central data structures into a thread-local cache as needed

TCMalloc treates objects with size <= 32K ("small" objects) differently from larger objects. Large objects are allocated directly from the central heap using a page-level allocator (a page is a 4K aligned region of memory). I.e., a large object is always page-aligned and occupies an integral number of pages.







In case of Seastar
is that seastar substitutes its own allocator for the system allocator (e.g. we provide our own malloc()),
and this allocator is not prepared for threads -- all system memory is divided among the shards, leaving nothing for TCMalloc.


So what we have to do is we have to remove TCmalloc completely from OSD first
