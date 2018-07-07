### Dmdedup: Device Mapper Target for Data Deduplication

- Dmdedup operates at the block layer, so it is usable with existing file systems and applications.
implemented and evaluated three backends:
  - an in-RAM table
  - an on-disk table
  - an on-disk COW B-tree.


- Removing duplicate data (deduplication) helps this problem by decreasing the amount of physically stored information
- Deduplication has often been applied to `backup datasets` because they contain many duplicates and represent the majority of enterprise data
- researchers have begun to explore primary storage deduplication


**primary storage deduplication**
- latency constraints are stricter;
- deduplication engine must compete with other processes for CPU and RAM
- 
