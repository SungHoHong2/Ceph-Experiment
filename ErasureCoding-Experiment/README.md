### Erasure Coding

**basic idea**
- targeted on Clouds and Recovery
- [tutorial links](http://web.eecs.utk.edu/~plank/plank/papers/2013-02-11-FAST-Tutorial.pdf)
- not recommended for professional workloads


<br>

**What is erasure coding?**
erasure coding is a space save mechanism where multiple redundant copies are reduced to a single party block

- in replication we have `8G(4G+4G)` of real data with `16G of actual space` using
- in erasure `8G` of real data and `12G of actual space` using because 4G is saved

<br>

- In case the one of the block fails, the parity block should be in different location from the block where actual data is stored

- In RF2 system the max code automatically chosen will be 4:1 (6+ nodes)
- In RF3 clusters 4:2 (with 7+nodes)

<br>

**drawbacks of erasure coding**
- everytime there is a change the parity code needs the update
- if there is a lot of updates then erasure coding is not the best option
- erasure coding needs a spare `rebuilding block` so it will only run on `4+` node cluster


<br>

**Optimizing of erasure coding**
- [related link](https://www.youtube.com/watch?v=KXd5U5fNQ6s)
- replication is being replaced by erasure coding due to storage overhead
- Facebook uses Reed Solomon
  - Compute 4 parity blocks per 10 data blocks
  - Storage overhead 1.4x = 14/10
  - For a single failure, needs 10 data blocks over network from other nodes to recover.
    - problems with degraded replaced `extra latency`
    - data reconstruction

- if recovery cost `the number nodes it has to access to recover` is big and storage overhead is small
- if storage overhead is  big then the recovery cost is small

<br>

- **which lead us to the next question**
  - is there a way to recover faster and use less storage?
  - coded with a compact code with high storage efficiency
