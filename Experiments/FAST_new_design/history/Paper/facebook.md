### Abstract
Facebook leverages memcached as a building block to construct and scale a distributed key-value store that supports the world’s largest social network.


### Introduction
A social network’s infrastructure needs to

(1) allow near realtime communication
(2) aggregate content on-the-fly from multiple sources
(3) be able to access and update very popular shared content
(4) scale to process millions of user requests per second



### Overview
*losts of data*
users consume an order of magnitude more content than they create

*independent*
our read operations fetch data from a variety of sources such as MySQL databases, HDFS installations, and backend services


*Query cache*
lighten the read load on our databases
sends a delete request to memcache that invalidates any stale data. We choose to delete cached data instead of updating it because deletes are idempotent

memcached provides no server-to-server coordination;
It is an in-memory hash table running on a single server



### In a Cluster: Latency and Load

**Reducing Latency**
*problem*
A single user web request can often result in hundreds of individual memcache get requests.
Items are distributed across the memcached servers through consistent hashing
Consistent hashing provides elastic scaling (a term used to describe dynamic adding/removing of servers based on usage load) for cache servers.
web servers have to routinely communicate with many memcached servers to satisfy a user request.
As a result, all web servers communicate with every memcached server in a short period of time

*solution*
reduce latency mainly by focusing on the memcache client
Clients maintain a map of all available servers

**parallel requests and batching**
directed acyclic graph (DAG) representing the dependencies between data

**Client-server communication**
Memcached servers do not communicate with each other
embed the complexity of the system into a stateless client rather than in the memcached servers

**sliding window**
to control the number of outstanding requests.
grows slowly upon a successful request
shrinks when a request goes unanswered



### Reducing Load
use memcache to reduce the frequency of fetching data along more expensive paths such as database queries

**Leases**
A stale set occurs when a web server sets a value in memcache that does not reflect the latest value that should be cached.
A thundering herd happens when a specific key undergoes heavy read and write activity.

With the lease token, memcached can verify and determine whether the data should be stored and thus arbitrate concurrent writes
Requests for a key’s value within 10 seconds of a token being issued results in a special notification telling the client to wait a short amount of time.
Typically, the client with the lease will have successfully set the data within a few milliseconds. Thus, when waiting clients
retry the request, the data is often present in cache.


### Memcache Pools
Using memcache as a general-purpose caching layer requires workloads to share infrastructure despite different access patterns, memory footprints, and quality-ofservice requirements

low-churn keys that are still valuable are evicted before high-churn keys that are no longer being accessed. Placing these keys in different pools prevents this kind of negative interference,


### Replication Within Pools
replication to improve the latency and efficiency of memcached servers.


### Replication
However, na¨ıvely scaling the system does not eliminate all problems.
Highly requested items will only become more popular as more web servers are added to cope with increased user traffic
We therefore split our web and memcached servers into multiple frontend clusters.
These clusters, along with a storage cluster that contain the databases, define a region.


**Invalidation check**
storage cluster in a region holds the authoritative copy of data, user demand may replicate that data into frontend clusters.

mcsqueal could contact memcached servers directly, the resulting rate of packets sent from a backend cluster to frontend clusters would be unacceptably high.
This packet rate problem is a consequence of having many databases and many memcached servers communicating across a cluster
boundary.
Invalidation daemons batch deletes into fewer packets and send them to a set of dedicated servers running mcrouter instances in each frontend cluster

*Invalidation via web servers*
simpler for web servers to broadcast invalidations to all frontend clusters.
This approach unfortunately suffers from two problems.
First, it incurs more packet overhead as web servers are less effective at batching invalidations than mcsqueal pipeline

Second, it provides little recourse when a systemic invalidation problem arises such as misrouting of deletes due to a configuration error.


### Regional Pools
reduce the number of replicas by having multiple frontend clusters share the same set of memcached servers.


### Across Regions: Consistency
Each region consists of a storage cluster and several frontend clusters.
We designate one region to hold the master databases and the other regions to contain read-only replicas
*maintaining consistency between data in memcache and the persistent storage becomes the primary technical challenge.*


###  Single Server Improvements
(1) allow automatic expansion of the hash table to avoid look-up times drifting to
(2) make the server multi-threaded using a global lock to protect multiple data structures
(3) giving each thread its own UDP port to reduce contention when sending replies and later spreading interrupt processing overhead.


**Adaptive Slab Allocator**
employs a slab allocator to manage memory.
The allocator organizes memory into slab classes, each of which contains pre-allocated, uniformly sized chunks of memory.
Memcached stores items in the smallest possible slab class that can fit the item’s metadata, key

Once a memcached server can no longer allocate free memory, storage for new items is done by evicting the least recently used (LRU)


**expiration time**
When we added a short expiration time to a heavily used set of keys whose items have short useful lifespans; the proportion
of memcache pool used by this key family was reduced from 6% to 0.3% without affecting the hit rate.


### Conclusion
show how to scale a memcached-based architecture to meet the growing demand of Facebook.
(1) Separating cache and persistent storage systems allows us to independently scale them
(2) Features that improve monitoring, debugging and operational efficiency are as important as performance.
(3) Managing stateful components(memcacheD) is operationally more complex than stateless ones(client).
(4) The system must support gradual rollout and rollback of new features even if it leads to temporary heterogeneity of feature sets.
(5) Simplicity is vital
