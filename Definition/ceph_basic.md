### What is Ceph
- open source software
- designed to provide highly scalable object-, block- and file-based storage under a unified system.

### Problem
- companies develop hardware, software specifically for cluster storage
- proprietary software, hardware, and support of maintenance is needed

### Solution
- use standard hardware
- open source software
- individuals can handle the cluster storage

### features of Ceph
1. scalable
2. no single point of failure
3. software-based


### Cepth storage cluster
| definition | description |
| --- | --- |
| Objects | **CEPH OBJECT GATEWAY** that powers the Ceph object to store modern applications |
| Virtual Disks | **CEPH BLOCK DEVICE** that is a distributed virtual block device for virtual machines and legacy applications |
| File Directories | **CEPH FILESYSTEM** that is distributed scale out file system with POSIX semantics that provides storage |

| components | description |
| RADOS | access the cluster storage as a logical unit, comprised of OSDs and Monitors  |  
| LIBRADOS | library for applications to communicate with RADOS applications |
| RADOSGW | translate REST to direct RADOS call |
| RBD | block storage interface, provide a virtualized disk to the virtual machine |
| CEPHS | manage meta-data |



### Cepth terminology
```
RADOS - a reliable, distributed object store comprised of self healing self managing storage nodes
OSD - serve stored objects to clients
Monitors - Maintain cluster membership and state
meta-data of ceph : syntax for POSIX
```


### CRUSH
- how it finds the data and place the data
- random placement algorithm
- ** the entire calculation of CRUSH is done in the client **

1. hash the object into numbers
2. calculate the location in the client
3. this provides an statistically even distribution









### Terminology
```
# Legacy
Legacy storage is storage hardware or storage software that is still functional, but is either outdated or unsupported.


# POSIX
POSIX is a family of standards, specified by the IEEE, to clarify and make uniform the application programming interface
Many Linux system calls exist to implement a specific POSIX C API function and make Linux compliant, e.g. sys_write, sys_read, ...
Major Linux desktop implementation: glibc, which in many cases just provides a shallow wrapper to system calls.


# REST
REST architecture involves reading a designated Web page that contains an XML file. The XML file describes and includes the desired content. Once dynamically defined, consumers may access the interface.

```
