### CacheDedup: In-line Deduplication for Flash Caching
a novel architecture that integrates the caching of data and deduplication metadata
(source addresses and fingerprints of the data)
proposes duplication-aware cache replacement algorithms


### Introduction
flash-memory-based storage as a caching layer between the DRAM-based main memory and HDD-based primary storage in a typical I/O stack of a storage system

**problem**
the demands on cache capacity have skyrocketed compared to the limited capacity of commodity flash devices.
since flash memories wear out with writes, the use of flash for caching aggravates the endurance issue


**positive**
deduplication reduces the cache footprint of workloads, allowing the cache to better store their working sets and reduce capacity misses.

deduplication reduces the number of necessary cache insertions caused by compulsory misses and capacity misses, thereby reducing flash memory wear-out and enhancing cache durability.


integrates the `caching of data` and `deduplication metadata`
- `source addresses` and `fingerprints of the data`
- using a separate Data Cache and Metadata Cache


allows CacheDedup to bound the space usage of metadata, making it flexible enough to be deployed either on the client side or the server side of a storage system, and implemented either in software or in hardware
enables the optimization of caching historical source addresses and fingerprints in the Metadata Cache after their data is evicted from the Data Cache
D-LRU, a duplication-aware version of LRU which can be efficiently implemented by enforcing an LRU policy on both the Data and Metadata Caches.



### Background
**problem**
Although one can take existing flash caching and deduplication solutions and stack them together to realize cache deduplication, the lack of integration will lead to inefficiencies in both layers.

First, the deduplication layer has to manage the fingerprints for the entire primary storage, and may make fingerprintmanagement
decisions that are detrimental to the cache. evicting fingerprints belonging to data with good locality and causing duplicate copies in the cache

**related works**
recent work Nitro [19] studied the use of both deduplication and compression for server-side flash caches.


**design**
CacheDedup does in-line deduplication to prevent any duplicate block from entering the cache, thereby achieving the greatest reduction of data footprint and wear-out.

Inline deduplication is the removal of redundancies from data before or as it is being written to a backup device.
in comparison to post-process deduplication.

CacheDedup choo ses deduplication at the granularity of cache blocks. which fits the structure of flash caches and facilitates

the design of duplication-aware cache replacement.


“scan resistant”, i.e., items accessed only once could occupy the cache and reduce the space available for items that are accessed repeatedly.
ARC [21] is an adaptive algorithm that considers both recency and frequency in cache replacement.
It is “scan resistant” and shown to offer better performance for many real-world workloads.




### Architecture
A traditional cache needs to manage the mappings from the source addresses of blocks on the primary storage to the cache addresses of blocks on the cache device
A deduplication layer needs to track the fingerprints of the data blocks in order to identify duplicate blocks.

First, unlike a traditional cache, the number of source-to cache address mappings in CacheDedup is not bounded by
the size of the cache, because with deduplication, there is now a many-to-one relationship between these two address
spaces


Second, even though the number of fingerprints that a cache has to track is bounded by the cache size, there is an important
reason for CacheDedup to track fingerprints for databeyond what is currently stored in the cache

Specifically, it is beneficial to keep historical fingerprints for the blocks that have already been evicted from the cache, so that when these blocks are requested again, CacheDedup does not have to fetch them from the primary storage in order to determine whether they are duplicates of the currently cached blocks
