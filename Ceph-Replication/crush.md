### Crush
- [CRUSH: Controlled, Scalable, Decentralized Placement of Replicated Data](https://ceph.com/wp-content/uploads/2016/08/weil-crush-sc06.pdf)

- evenly distribute data and workload to efficiently utilize available resources


<br>

**introducing CRUSH**
- a scalable pseudorandom data distribution
- works in distributed object-based storage systems
- maps data objects to storage devices without relying on a central directory.
- facilitate the addition and removal of storage while minimizing unnecessary data movement
- accommodate wide variety of data replication
- distribute data in terms of user defined policies


<br>

**Object-based storage**
- Object-based storage is an emerging architecture that promises improved manageability, scalability, and performance
- manage disk block allocation internally, exposing an interface that allows others to read and write to variably-sized, named
objects.
- each file’s data is typically striped across a relatively small number of named objects distributed throughout the storage cluster
- Objects are replicated across multiple devices
- vastly improves scalability by reducing file allocation metadata and complexity,
- `distributing data among thousands of storage devices`

<br>

**problem with traditional distribution**
- Most systems simply write new data to underutilized devices.
  - data is rarely, if ever, moved once it is written.
  - robust solution is to distribute all data in a system randomly among available storage devices.
  - a probabilistically balanced distribution and uniformly mixes old and new data together.
  - When new storage is added, a random sample of existing data is migrated onto new storage devices to restore balance.
-  simple hashbased distribution fails to cope with changes in the number of devices,
  - incurring a massive reshuffling of data
  - existing randomized distribution schemes that decluster replication by spreading each disk’s replicas across many other devices suffer from a high probability of data loss
