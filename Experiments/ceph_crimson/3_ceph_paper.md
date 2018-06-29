### smart dedup
http://bit.csc.lsu.edu/~yjia/papers/SmartDedup.pdf

### The research and implementation of metadata cache backup technology based on CEPH file system
desktop/07529537.pdf
- Focused on `Meta Data Server == Ceph MDS`  
- Ceph metadata management subsystem is upgraded
- The log cache backup scheme
  - reduces metadata access delay
  - by caching logs and reducing the time of storing logs to server clusters.
- cached data backup scheme
  - prevent the loss of cached data in metadata servers



### Chirp: a practical global filesystem for cluster and Grid computing
https://link.springer.com/content/pdf/10.1007%2Fs10723-008-9100-5.pdf


### A Data Deduplication Method in the Cloud Storage Based on FP-tree
- delete the underlying redundant data in cloud storage.
- save large amount of disk space
- combined with cloud storage hash location function




### Wenji's work
- use non volatile memory
- which was also mentioned in the paper from Purdue
- async-messenger has the performance when used with RDMA.
