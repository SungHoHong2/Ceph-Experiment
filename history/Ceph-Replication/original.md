### Ceph: A Scalable, High-Performance Distributed File System

- [related link](https://ceph.com/wp-content/uploads/2016/08/weil-ceph-osdi06.pdf)
- maximize the separation between data and metadata management
- pseudo-random data distribution function



### Introduction

- **problem**
  - Although widely used, the centralization inherent in the client/server model has proven a significant obstacle to scalable performance.
  - adopted architectures based on object-based storage
  - interact with a metadata server (MDS) to perform metadata operations
  - communicating directly with OSDs to perform file I/O
  - **this model continue to suffer from scalability limitations due to little or no distribution of the metadata workload**


- **solution**
  - accept the fact that ...
  - large systems are inevitably built incrementally,
  - node failures are the norm
  - quality and character of workloads are constantly shifting over time.


- **implementation**
  - Ceph decouples data and metadata operations
  - distribute the complexity surrounding data access, update serialization, replication and reliability, failure detection, and recovery
  - Clients perform file I/O by communicating directly with OSD
  - Each process can either link directly to a client instance or interact with a mounted file system


### System Overview
- **client**
  - each instance of which exposes a near-POSIX file system interface to a host or process
- **OSD**
  - collectively stores all data and metadata;
- **meta data server**
  - manages the namespace (file names and directories) while coordinating security, consistency and coherence


- **decoupled data and metadata**
  - Metadata operations (open, rename, etc.) are collectively managed by a metadata server cluster,
  - clients interact directly with OSDs to perform file I/O (reads and writes)
  - CRUSH [29] assigns objects to storage devices.
    - This allows any party to calculate (rather than look up) the name and location of objects comprising a file’s contents
    - reducing the metadata cluster workload.


- **Reliable Autonomic Distributed Object Storage**
  - maintain the desired level of data replication.
  - Ceph delegates responsibility for data migration, replication, failure detection, and failure recovery to the cluster of OSDs that store the data,
  - OSDs collectively provide a single logical object store to clients and metadata servers.
