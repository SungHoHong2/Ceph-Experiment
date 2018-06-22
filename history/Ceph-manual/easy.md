# easy ceph
explaining ceph in the most easiest way


### Advantages of ceph
- avoids centralized data lookups
- even data distribution
- healing is distributed
- abtracted storage backends


<br>

### Precautions

- faster osds should not get mixed with slower osds
  - slow osds will just keep piling backup and slow the fast osds
  - this is because ceph loves recurrency

<br>

### How much penalty you get from the replication
- this is the main dish for every companies
- the tests are implemented by
  - hundreds of Cluster Configuration
  - hundreds of Tunable Settings
  - hundreds of Potential IO pattern

- the penalty of write performance from the client side and the server side
can be different.
