### Wenji's Idea
- based on `memcacheD`
- `DPDK`
- `global cache deduplication`

**possible conrtribution**
- new algorithm for global cache-dedup management
- introduce CRUSH to memcacheD

**my goal**
- learn memcacheD
- learn global dedup
- find relationships


### Scaling Memcache at Facebook
limits the number of sending messages
https://www.youtube.com/watch?v=6phA3IAcEJ8
https://www.usenix.org/system/files/conference/nsdi13/nsdi13-final197.pdf
- [facebook.md]
- Push the compexity into the client whenever possible.
- Operational effeciency
- separate cache anad persistent store in order to scale independently

In generael, there are several memecacheD servers with one storage server in a region.
Multiple storage servers in multiple region replicate with each other.
In the Facebook scheme we can see that consistency betweem memcached and persistent storage is important.
Replication is done in the storage server and the workloads are sent back to the memcached server.

This should be obvious becasue the storage servers use MySQL and the result of the queries in the cache
should reflect the data in the storage servers.

**Question**
So the question is even though the storage server is transparent to the memcacheD server,
it still needs the mechanisms to check the consistencies between memcacheD and storage, Right?

**two way of handling memcacheD**
One is from the client to memcacheD server. (broadcasting the written data, and getting the read data)
the other is from the storage server to memcacheD server. (Data Replication is done here)


**the problem mentioned in memecacheD**
increasing the number of memecacheD servers may not increase the performance because only the limited amount of popular data are accessed. So using it as compact as possible seems an idea.
However, it is designed for reads, and not the writes.




### usage of MemcacheD in deduplications
- [Multi-level Selective Deduplication for VM Snapshots in Cloud Storage](http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=3F10BDD30305587B6C66DEDCE3B5B528?doi=10.1.1.308.9616&rep=rep1&type=pdf) is presumed to use part of memcacheD for deduplication.
Deduplication among Virtual machines.

It is divided into two (inner and cross deduplication)
The cross deduplication identify duplicated data blocks among multiple VMs through the index cache of common data set (CDS).
CDS is the most popular content blocks among snapshots across all VMs.
Each index entry contains the block fingerprint and a reference pointer to the location of its real content in the snapshot content block store. At the runtime, the CDS index resides in a distributed memory lookup table implemented using Memcached
leverage the aggregated memory in a cluster.

However the entire scheme seems to happen in one host.


### Bi-temporal memcached
Bi-temporal key value cache system
determining from said current version and at least one historical version of said data object which version is to be returned

### Persistent memory management
which was also onced mentioned in the RDMA-Paper that ming zhao provided us.
Using memcacheD and storing the data in persistent-memory device.


### Existing Deduplication Techniques - Springer
https://www.springer.com/cda/content/document/cda_downloaddocument/9783319422787-c1.pdf?SGWID=0-0-45-1588200-p180146651
this page explains all the known deduplication techniques


### Last-level Cache Deduplication
http://delivery.acm.org/10.1145/2600000/2597655/p53-tian.pdf?ip=209.147.139.8&id=2597655&acc=ACTIVE%20SERVICE&key=B63ACEF81C6334F5%2EBD7B0059B564CDBA%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1530778013_601924e8b2477ad6214ad1dcf82e3a0c
- [last-level.md]

**Question**
Is it possible to both compress and deduplicate cache at the same time?
If we use DPDK as reducing the overall latency, then shouldn't we secure all the necessary optimization methods
while producing a reasonable result?


**how it works**
With cache deduplication, the mapping from the data store to the tag store is no longer one-to-one

creates three seperate tag array, data array and hash table
data array and hash-table are one-to-one
the tag array and data-array is many to one.
the tag arrays can point to a same element in data array, and the data array points to the hash index.
the tag arrays with the same data array are connected together.
the data deduplication is checked by POST


### CacheDedup: In-line Deduplication for Flash Caching
https://www.usenix.org/system/files/conference/fast16/fast16-papers-li-wenji.pdf
- [cache-dedup.md]
- this paper is different from the previous one considering that one is inline deduplication and the other is post-process deduplication.


### Conclusion
There is no deduplication tested for MemcacheD.
Since MemcacheD is very simple, it is found to be used in different sources. even as a simple hash-table.
But considering that memcacheD was a old topic, we need to find out exactly why deduplication is not tested on MemcacheD.
I need to look for more papers or get a definite answer for this question.



*I believe that further insight will come out after I read the stories above*







### SSD-Assisted Hybrid Memory to Accelerate Memcached over High Performance Networks*
- if this is correct, then wouldn't this also help? the global cache deduplication?
- This idea is due to the fact `we are not using flash memory`
- memecacheD should not use SSD because the storage speed and the memory speed will have no difference.
- Then there is no point of using memcacheD.
https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6337608


### How MemcacheD normally go along?
- key value cache systems, for example Memcached,
- Memcached is used to cache the hot data and thus avoid database access for most data queries.


- [Delivering billions of messages exactly once](https://segment.com/blog/exactly-once-delivery/)

- [](http://eng.tapjoy.com/blog-list/real-time-deduping-at-scale) also mentions memcacheD used to detect duplicate messages
the project is SQS




### Client-side Data Deduplication




### Find possible options
**Keywords experiment**
- dedupe in userspace

[Bee](https://github.com/Zygo/bees)
[tekmark](https://github.com/tekmark/Userspace-Deduplication-Filesystem)

*userpsace dedupe exampels*
[Lessfs]
[SDFS]

*global dedup*
[REAL-TIME DEDUPING AT SCALE](http://eng.tapjoy.com/blog-list/real-time-deduping-at-scale)
[Challenges of Dedup at scale]https://www.trilio.io/resources/deduplication-scale-out-file-systems/
[FINE-GRAIN IN-MEMORY DEDUPLICATION FOR LARGE-SCALE WORKLOADS](https://stacks.stanford.edu/file/druid:rp831pj6163/StevensonFGIMDDThesis-augmented.pdf)


*papers related to memcacheD*
[Implementation of a deduplication cache mechanism using content-defined chunking]


*Deduplication relation with DPDK*
global duplication using sharding works best for both seastar but also DPDK.
key words can be sharding, lock-free

[High-speed data deduplication using parallelized cuckoo hashing](http://journals.tubitak.gov.tr/elektrik/issues/elk-18-26-3/elk-26-3-24-1708-336.pdf)
[A fast approach for parallel deduplication on multicore processors](https://pdfs.semanticscholar.org/1e00/6363628d3eeea5a449da4cd1ae48b5d103fe.pdf)
