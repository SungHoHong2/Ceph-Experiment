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
  - [paper](https://ceph.com/wp-content/uploads/2016/08/weil-crush-sc06.pdf)
  - a scalable pseudorandom data distribution function designed for distributed object-based storage systems efficiently maps data objects to storage devices without relying on a central directory.
  - large systems are inherently dynamic, CRUSH is designed to facilitate the addition and removal of storage while minimizing unnecessary data movement.
  - fundamental task of distributing data among thousands of storage devices—typically with varying capacities and performance characteristics remains
  - A robust solution is to distribute all data in a system randomly among available storage devices.


  <br>

  - However, simple hashbased distribution fails to cope with changes in the number of devices, incurring a massive reshuffling of data
  - Further, existing randomized distribution schemes that decluster replication by spreading each disk’s replicas across many other devices suffer from a high probability of data loss from coincident device failures.


  - CRUSH needs only a compact, hierarchical description of the devices comprising the storage cluster and knowledge of the replica placement policy.

  - CRUSH is designed to optimally distribute data to utilize available resources,

  <br>

  - Storage devices are assigned weights by the administrator to control the relative amount of data they are responsible for storing.
  - CRUSH defines placement rules for each replication strategy or distribution policy employed that allow the storage system or administrator to specify exactly how object replicas are place



<br>

**Crush algorithm**
- The CRUSH algorithm distributes data objects among storage devices according to a per-device weight value approximating
a uniform probability distribution
