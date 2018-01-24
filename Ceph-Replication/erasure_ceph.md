# [Sprout: A Functional Caching Approach to Minimize Service Latency in Erasure-Coded Storage](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8048491)


### Abstract
- Modern distributed storage systems often use erasure codes to protect against disk and node failures to increase reliability
- while trying to meet the latency requirements of the applications and clients

- **old solution**
  - a key solution to relieve this traffic burden is caching
  - storing chunks of popular data at different locations closer to end-users
  - congestion in the network and improve service delay for processing file requests
  - **However addressing the impact of erasure coding on latency**
  -

- a novel caching framework with erasure code called functional caching.
  - an optimal functional caching placement
  - the access probabilities of the file request from different disks
  - The proposed algorithm gives significant latency improvement

<br>

### Introduction
- a significant burden on the underlying networks of datacenter storage systems.
- focus on latency analysis in erasure coded storage systems
- caching for datacenters where the files are encoded with erasure codes gives rise to new challenges.
  - First, using an (n, k) maximumdistance-separable (MDS) erasure code,
  - a file is encoded into n chunks and can be recovered from any subset of k distinct chunks
  - Thus, file access latency in such a system is determined by the delay to access file chunks on hot storage nodes with the slowest performance.

- caching the most popular data chunks is often optimal because the cache-miss rate and the resulting network load are proportional to each other.
- **this may not be true for an erasure-coded storage**
  - where cached chunks need not be identical to the chunks transferred from storage nodes.
  - a function of the data chunks can be computed and cached, so that the constructed new chunks (i.e., d), along with the existing chunks, satisfy the property of being a new MDS code.
  -  It effectively expands the existing (n, k) code to an (n + d, k), which leads to much lower access latency

- **introducing sprout**
  - efficiently capitalize on existing file coding in erasure-coded storage systems.
  - the file can now be recovered from any k out of n + d chunks (rather than k out of n under exact caching),
  - For 1000 files, we find that the algorithm converges within 20 iterations


<br>


**introducing SPROUT**
- efficiently capitalize on existing file coding in erasure-coded storage systems.
- our functional caching approach forms d new data chunks, which together with the existing n chunks satisfy the property of
being an (n + d, k) MDS code
