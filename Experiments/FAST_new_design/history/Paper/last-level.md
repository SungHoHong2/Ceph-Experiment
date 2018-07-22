### Last-level Cache Deduplication

### abstract
many blocks in the working set of typical benchmark programs have the same values
propose cache deduplication that eectively increases lastlevel cache capacity
detects duplicate data blocks and stores only one copy of the data

detects duplicate data blocks and stores only one copy of the data in a way that can be accessed through multiple physical addresses

Dierent cache blocks with dierent addresses can contain copies of identical data.
These duplicated blocks waste cache capacity because they store redundant information.

Previous cache-compression techniques proposed to compress specific
while eliminating all possible duplication leads to 47.5% of cache blocks removed/invalidated, as shown in Figure 2



### Challneges
way to compare data to detect possible duplication.
caches contain a large number of blocks, direct comparison with all blocks is prohibitively expensive.

**how to find duplicate**
1. A tree-based structure requires more metadata to maintain the tree
2. Indexing using a hash function

**when**

**Deduplication granularity**
Granularity at the sub-blocklevel may lead to a higher rate of deduplication, but it also causes increased access latency

**Write hit and replacement of duplicated blocks**


### DEDUPLICATED LAST-LEVEL CACHE
uses augmented hashing to detect duplication
post-process detection
uses block-level-deduplication granularity
e distinct-first random replacement (DFRR)

**structure**
three decoupled structures
- More than one tag can share a data block.
*tag array*
- set-associative structure that keeps the semantics of cache blocks
- tag bits, LRU bits, valid bit, and dirty bit
- a reference that indexes the data array, and two references that point to other tag entries that maintain a doubly-linked list of tags all pointing to the same data block.
- all tags stored in the tag array that share identical data are linked
- The linked list of tags of duplicated blocks is referred to as the tag-list
- When there is a replacement in the data array, all associated tags can be tracked along with the taglist of the data block and invalidated.

*Data Array*
When a tag is inserted into the tag array, the corresponding Ctr in the data array is incremented by 1
When a Ctr becomes zero, the data block can be reused


**Operations**
On a cache access, the tag of the requested block is compared in parallel with all tags in a specific set of the tag array.
The placement of the cache block then is separated into two parts: placement in the tag array and placement in the data array


1. On a cache miss to Block A, the requested block is fetched from the main memory. The tag is inserted in the tag array and the data d1 is inserted in an invalid data entry, as in Step 1.
2. On the next cache miss to Block B, during the memory access time, the previously placed data d1 of Block A is detected for duplication. The hash value of d1 indexes a bucket in the hash table (Step 2)
3. Because the bucket is empty, the location of d1 and its hash value hd1 are placed in this bucket. After Block B is fetched from the memory, it is filled in the cache (Step 3)
4.
