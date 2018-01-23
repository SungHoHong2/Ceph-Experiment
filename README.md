# Ceph-Experiment

### Contents
1. [Network Basics](/network-basic)
2. [Deploy Ceph](/ceph-deploy)
3. [Messenger Mechanism](/messenger)
4. [GDB Experiments](/gdb-Experiment)

<br>


### Ceph Replication
- [Explanation about CRUSH](Ceph-Replication/crush.md)
- [A Weak Consistency Model For Ceph](Ceph-Replication/weak.md)
- [ErasureCoding](ErasureCoding-Experiment)

<br>

### Ceph Manual
- [Ceph-manual from mirantis.com](Ceph-manual/mirantis.md)


<br>

### Abstract

1. **Message Passing Used by Ceph**
    - Uses Singleton Message Stack as the protocol template
    - The stack is passed on to the Threads for running RW ( Asynchronous mode )
![image-2](img/2_i.png)


2. **Implemented Structure**
    - used virtual box
    - three virtual os served as monitor and two osds
    - used bluestore
![ceph_structure](/ceph-deploy/ceph_test.png)
