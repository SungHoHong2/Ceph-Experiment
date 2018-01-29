# [High performance RDMA-based design of HDFS over InfiniBand](http://delivery.acm.org/10.1145/2390000/2389044/a35-islam.pdf?ip=209.147.139.8&id=2389044&acc=ACTIVE%20SERVICE&key=B63ACEF81C6334F5%2EBD7B0059B564CDBA%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1516743980_fcfcdd026aa62574fb72b80a350d2f3b)


### Abstract
- Hadoop Distributed File System (HDFS) acts as the primary storage of Hadoop
- The existing implementation of HDFS uses Javasocket interface for communication which delivers suboptimal performance in terms of latency and throughput.
- network performance becomes key component as the amount of data being stored and replicated to HDFS increases.
- a novel design of HDFS using Remote Direct Memory Access (RDMA) over InfiniBand via JNI interfaces.


<br>

### Introduction
- MapReduce programming model which has emerged as a scalable way to perform data-intensive computations.
- **Current design for using INFINIBAND is wrong**
  - Implementations of MPI, such as MVAPICH2 [5], achieve low one-way latencies in the range of 1-2µs
  - even the best implementation of sockets on InfiniBand achieves 20-25µs one-way latency
  - Due to the byte stream communication nature of TCP/IP, multiple data copies are required,
  - HDFS cannot fully utilize the hardware capability and obtain peak performance.

- **design challenges**
   1. How does the network performance impact the overall
HDFS performance?
   2.  How can we re-design HDFS to take advantage of high performance interconnects such as InfiniBand and exploit advanced features such as RDMA?
   3. What will be the performance improvement of HDFS operations with the new RDMA-based design over InfiniBand?
   4. Can we observe the performance improvement for other cloud computing middlewares such as HBase, which use HDFS as the underlying filesystem?

- **conclusion**
  - identify the impact of network performance on overall HDFS performance for different network interconnects.
  - All existing communication protocols [9] of HDFS are layered on top of TCP/IP.
  - Consequently, even though the underlying system is equipped with high performance interconnects such as InfiniBand, HDFS cannot fully utilize the hardware capability and obtain peak performance


- **the identified challenges**
  - perform detailed profiling and analysis to identify the impact of network performance on overall HDFS performance for different network interconnects
  - present a hybrid HDFS design that incorporates RDMA over InfiniBand,
  - We use the inhouse InfiniBand light weight communication library, Unified Communication Runtime (UCR) [10], in our design.
  - We perform detailed tuning of HDFS packet-size for different networks and identify the optimal packet size for each interconnect.

<br>


### background

- **Hadoop Distributed File System (HDFS)**
  - a distributed file system which is used as the primary storage for Hadoop cluster
  - An HDFS cluster consists of two types of nodes: `NameNode` and DataNode.
    - `NameNode` manages the file system namespace
      - It maintains the file system tree and stores all the meta data
      - it gets the block IDs and a list of DataNodes for each block from the NameNode.
      - Each block is split into smaller packets and sent to the first DataNode in the pipeline
    - `DataNode` then replicates each of the packets to the subsequent DataNodes.
      - Packet transmission in HDFS is pipelined; a DataNode can receive packets from the previous DataNode while it is replicating data to the next DataNode.
  - **example**
    - When a client reads an HDFS file
    - it first contacts the NameNode to check its access permission and gets the block IDs and locations for each of the blocks.
    - For each block belonging to the file, the client connects with the nearest DataNode and reads the block.


- **InfiniBand Overview**
    - InfiniBand [4] is an industry standard switched fabric that is designed for interconnecting nodes in High End Computing
(HEC) clusters
    - This feature is very powerful and can be used to implement high performance communication protocols.
    - Verbs that are used to transfer data are completely OS-bypassed.
    - InfiniBand also provides a driver for implementing the IP layer, allowing socket applications to make use of InfiniBand networks
    - **IPoIB**
      - One is the datagram mode, implemented over Unreliable Datagram (UD), and the other is connected mode, implemented over RC.
      - the other is connected mode, implemented over RC.
    -  use SSDs in our experiments to lessen the I/O bottlenecks when studying the effects of communication over different interconnects

- **Hadoop Database (HBase)**
  - Java-based database that runs on top of the Hadoop framework
  - used to host very large tables with many billions of entries
  - primarily focus on the HDFS Write operation

- **propose design**
  - propose a hybrid design for HDFS that supports both socket and RDMA-based communication.
  - key components of the hybrid design
  - we discuss the key components of the hybrid design and then describe the associated challenges
  - We concentrate on HDFS Write because it is more network intensive.
  - Our design extends the existing HDFS and uses UCR for InfiniBand communication via the JNI interfaces.

<br>


### Evaluation
- compare the performance of our design with that of 1 GigE, IPoIB and 10 GigE. In HDFS
- network performance is sensitive to different HDFS parameters (e.g. packet-size).
1. Determining the Optimal Packet-Size for Different Interconnects//Protocols:
2. Micro-benchmark Level Evaluations on Different Interconnects: In this set of experiments, we have measured the latency of HDFS Write for different file sizes
3. Experiments with TestDFSIO: In this set of experiments, we have measured the throughput of sequential write operations to HDFS.
