### Ceph Theory

**NETWORK CONFIGURATION REFERENCE**
- high performance Ceph Storage Cluster.
- Ceph Clients make requests directly to Ceph OSD Daemons.
- Unless you specify a cluster network Ceph assumes a single “public” network.

<br>

**Ceph storage preferred architecture**
- a public (front-side) network and a cluster (back-side) network.
- at least two 1Gbps network interface controllers (NICs) for each hosts
- Since most commodity hard disk drives have a throughput of approximately 100MB/second
    - Replicating 1TB of data across a 1Gbps network takes 3 hours,
    - By contrast, with a 10Gbps network, the replication times would be 20 minutes and 1 hour respectively
    - [One question don't the hard disk also have to be fast enough to produce good results from DPDK network]()

- In a petabyte-scale cluster, failure of an OSD disk should be an expectation, not an exception.
  - System administrators will appreciate PGs recovering from a degraded state



**Recovery and rebalancing**
- Ceph OSD Daemons handle data replication for the Ceph Clients.
- The network load between Ceph OSD Daemons easily dwarfs the network load between  Ceph Clients and the Ceph Storage Cluster.
- Scalability and Availability
  - In traditional architectures, clients talk to a centralized component
  - **Ceph uses an algorithm called CRUSH**
  - Ceph eliminates the centralized gateway to enable clients to interact with Ceph OSD Daemons directly
  - Ceph OSD Daemons create object replicas on other Ceph Nodes to ensure data safety and high availability


**CRUSH**
  - 





**Monitor / OSD Interaction**



**Security**
