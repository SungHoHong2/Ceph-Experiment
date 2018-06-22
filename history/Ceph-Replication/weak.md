# A Weak Consistency Model For Ceph
- [related link](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=7823769)
- the main idea is the do asynchronus replication.
- there are groups of OSDs in a ring and each OSD is the leader of some group
- the leaders make the change and asynchronously update others


<br>

### Abstract
- **The primary-copy consistency model used in Ceph**
  - cannot satisfy the low latency requirement of write operation required by users
- **propose a weak consistency model, PROAR**
  - a distributed hash ring mechanism to allow clients `to only commit data to the primary node`
  - synchronize data to replication nodes `asynchronously in Ceph`
  - Based on the distributed hash ring mechanism, `the low latency requirement of write operation can be met.`
- **additional features**
  - the workload of the primary node can be reduced while that of replication nodes can be more balanced
  - evaluated the proposed scheme on a Ceph storage system with 3 storage nodes
- **conclusion**
  - PROAR can reduce about 50% write overhead compared to that of Ceph
  - has a more balanced workload around all the replication nodes.


<br>


### Introduction
- The main issue of data replications
  - how to synchronize replications to the same state when one of them has been modified
  - `strong consistency model` the replication process must ensure all replications are at the same state before any other file operations can be processed
    - primary-copy
    - chain
    - **result in a high latency of write operations.**
  - ` weak consistency models`  the replication process is completed when one of the replication is updated and other file operations can be proceeded.
    - Dynamo
    - Cassandra
    - PNUTS
    - Bayou
    - **will lead to a low latency of write operations compared to that of the strong consistency model**

  - `hybrid consistency models`
    - Gemini is a representative example of the hybrid consistency mode since it provides both strong consistency and weak consistency models and can choose a consistency model for a request operation automatically.

  - `Rados`
    - It only implements the primary-copy consistency model
    - As a cloud storage system in contemporary conditions, this is not enough to satisfy the requirements of clients



- **Introducing PROAR**
  - PROAR can reduce about 50% write overhead compared to that of Ceph
  - has a more balanced workload around all the replication nodes


<br>


### Related works

- **Customized consistency model.**
  - When clients write data to servers, system only commits data to N/2 servers, where N is the number of replication
  - Furthermore, when clients read data from servers, system only chooses the closest sever that contains the data as the read replica sever
  - The data in the selected server may be out-of-dated.

- **Ceph consistency model.**
  - the primary node executes each client operation in serialization
  - informs the replication nodes to execute this operation as the orders of primary node
  - After all the replications are executed successfully, the primary node returns client the successful response code.

- **PG (placement group)**
  - each object stored by the system is first mapped into a placement group (PG),
  -  a logical collection of objects that is replicated by the same set of devices
    - The PG of an object `o` is determined by a hash of the `name of o`,
    - the desired `level` of replication `r`
    - a bit mask `m` that controls `the total number of placement groups`
      -  `pgid = (r, hash(o) & m), where & is a bit-wise AND and the mask m= 2𝑘-1`



- **ceph overview**
  - each object stored by the system is first mapped into a placement group
  - PGs are allocated to object storage devices (OSDs) based on `CRUSH algorithm`
    - a robust replica distribution algorithm that calculates a stable and pseudo-random mapping
    - PGs are assigned to OSDs based on the cluster map,
    - PGs are distributed deterministically but pseudo-randomly
    - When one (or many) of devices join or leave the cluster, most PGs remain where they are and only small amount of data are shifted to maintain a balanced distribution
    - uses weights to control the relative amount of data assigned to each device based on its capacity and loading.
  - PGs are assigned to OSDs based on the cluster map   
    - most PGs remain where they are and only small amount of data are shifted to maintain a balanced distribution.
    - CRUSH, it also uses weights to control the relative amount of data assigned to each device based on its capacity and loading.



<br>

- **primary-copy consistency model**
  1. When a client writes an object to Ceph
  2. the client will first hash the object into a PG
  3. client sends a message to the monitor cluster to get the current cluster map,
    -  CRUSH algorithm is used to calculate all the OSDs of the PG.
  4. client directly sends data to the primary role OSD
    - the primary role OSD does not responds client with ack until all replicas have been committed to disk.
  - `all the operations of the same object will be directed to the same primary role OSD`

![img](img/ceph-1.png)

<br>


### PROAR CONSISTENCY MODEL
**avoid the disadvantages of primary-copy algorithm**
-  propose a weak consistency model called PROAR (Primary Role Hash Ring
    - PROAR builds a primary role hash ring after PG getting all the OSDs via CRUSH algorithm
    - PROAR only commits data to the primary role OSD and responses client with the result of commit
-  PROAR stores logs of all the replication role OSDs of the PG  
  - ensure the primary node and replication nodes at the same state when system converges
  - can be used for the recovery of down OSD.


<br>


**Primary role hash ring**
- OSDs of a PG are hashed into a 32-bit space circle
- They will play as the primary role OSDs of those objects between them in the primary hash ring.
- there are three replication OSDs of all the objects in a PG
- osd.1 is the primary role OSD of objects between PROARHash(osd. 3) and PROARHash(osd.1) in the primary role hash ring
- PROARHash is a function to get the hash values of OSDs in the primary role hash ring.
- S1 is the scope in between PROARHash(osd.3) and PROARHash(osd.1)
- all the operations of objects in the PG will be dispatched to the 3 primary role OSDs based on the hashing of object

<br>

### PROARHash
- the input is CRUSHArray that is obtained by using the CRUSH algorithm
- output is HashArray.

```
// 1 Get the hash value of OSDs
input: 𝐶𝑅𝑈𝑆𝐻𝐴𝑟𝑟𝑎y
output: HashArray
  function(PROARHash(CRUSH Array)
    for i=0 to CRUSHArray.size() do
      unitValue = ULONG_MAX|CRUSHArray.size()
      HashArray[i] = unitValue
    end for
    return HashArray
  end function
```

- The CRUSH algorithm is used to get the replication OSDs
- Client uses PROARHash function to get the hash value array, named HashArray,
- Client compares the object with all the values in HashArray to get its scope and get the corresponding primary role OSD.



<br>

### Example
- if client1 wants to write data to object1,
- it uses CRUSH algorithm to get CRUSHArray
- PROARHash function to get HashArray , and compare the object1.id with all the values of Hasharry
- If HashArray[i] <object1.id < HashArray[i+1], the primary role OSD of object1 is osd
- PROARHash takes advantages of CRUSH algorithm and hashes OSD to the invariant value according to their locations in the CRUSHArray.


<br>

### Summary of PROARHash
- Ceph, PGs are hashed to different OSDs by CRUSH algorithm
- The first OSD in the OSD array is selected as the primary node and other OSDs are replication nodes.
- The primary node will handle all the object operation requests from clients
- PROARHash can make the workloads of the primary role OSD and replication role OSDs more balanced since all OSDs will play the primary role for some objects.


<br>

### single primary node write
- data on primary node and replication data copies will divergent.
- Like write operation, read operation also uses PROAR and PROARhash to locate the primary OSD
- once an updated data is written to disk by the primary role OSD, all operations can be proceeded even the replication role OSDs
have not yet completed the writing of their replications.
- **However, this method also suffers that a read operation may get an out-of-dated data.**
  - a read operation only get data from the corresponding primary role OSD

<br>

**implementation**
- Client uses CRUSH algorithm to get CRUSHArray;
- According to CRUSHArray, get the corresponding primary role OSD;
- Client writes data to the primary role OSD. If operation is successful, primary role OSD responses the client with successful answer, else return false



<br>

### Convergence
- use PG log to ensure the synchronization between the primary role OSD and the replication role OSDs.
- In the primary role OSD, when each operation is executed successfully, system callback is invoked to write the operation entry to the PG log of this primary role OSD.


<br>

### Evaluation
- Setting 1: Ceph with the primary-copy consistency model, 333PGs, and 0 replication;
- Setting 2: Ceph with the primary-copy consistency model, 333PGs, and 3 replications
- Setting 3: Ceph with PROAR consistency model, 333 PGs and 3 replications.
- the performance of the write latency of the three settings with 1800 write requests
- data size at 4M, 8M, 16M, 32M and 64M bytes.

- **PROAR can reduce about 10% and 50% latency of write operation**
