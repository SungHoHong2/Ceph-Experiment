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


<br>

**CRUSH**
- **requirements**
  - hierarchical description
  - knowledge of the replica placement policy
- **advantages**
  - it is completely distributed such that any party in a large system can independently calculate the location of any object
  - metadata is required is mostly static, changing only when devices are added or removed
- **support**
  - A wide variety of data safety mechanisms are supported,

<br>


**algorithm**
- The distribution is controlled by a hierarchical `cluster map` representing the available storage resources
- distribution is composed of the `logical elements` from which it is built.
  - rows of server cabinets
  - cabinets filled with disk shelves
  - shelves filled with storage devices
- distribution policy
  - placement rules that specify how many replica targets are chosen from the cluster
  - what restrictions are imposed on replica placement
    - one might specify that three mirrored replicas are to be placed on devices in different physical cabinets
- a strong multi-input integer hash function
  - making the mapping completely deterministic and independently calculable
  - using only the `cluster map`, `placement rules`, and `input x`.
= pseudo-random distribution
  - there is no apparent correlation between the resulting output from similar inputs
  - a declustered distribution of replicas in that the set of devices


<br>

**cluster map**
- composed of `devices` and `buckets`
  - numerical identifiers and weight values associated
  - `Buckets` can contain any number of devices or other buckets weights are defined as the sum of the weights of the items they contain
  - `devices` are assigned weights by the administrator to control the relative amount of data

- device load is on average proportional to the amount of data stored.


<br>

**Replica Placement**
-  CRUSH placement policies can separate object replicas across different failure domains while still maintaining the desired distribution.
- placement rules for each replication
- distribution policy employed that allow the storage system or administrator to specify exactly how object replicas are placed
  - one might have a rule selecting a pair of targets for 2-way mirroring,
  - one for selecting three targets in two different data centers

<br>

**Rule example**
- `x` is typically an object name or other identifier
- `take(a)` operation selects an item (typically a bucket) within the storage and assigns to `vector i.array`
- `vectgor i.array` serves as an input to subsequent operations
- `select(n,t)` operation iterates over `i.array` ,and chooses n distinct items of type t in the subtree rooted at that point.
  - Storage devices have a known, `fixed type`
  - each bucket in the system has a `type` field that is used to distinguish between classes of buckets
  - those representing “rows” and those representing “cabinets”
  - iterates over n items requested and recursively descends through any intermediate buckets
- function `c(r,x)`  pseudo-randomly selecting a nested item in each bucket
- `emit` operation moves input `i.array` into the result vector

```c
CRUSH placement for object x

procedure TAKE(a)
 i.array = a                                // put item a in working vector
end proceduer

procedure SELECT(n,t)                       // select n items of type t
  o = 0                                     // initialize output
  for i in i.array
    f = 0                                   //initialize failures
    for r=1 in n                            // loop over n replicas
        fr = 0                              //no failures in this replica
        retry_descent = false
        repeat                              // redistribution
          b = bucket(i)
          retry_bucket = false
          repeat                            // in case of collision
            if (first n) then               // for primary copy recursion themes
              r = r + f                     
            else
              r = r + fr * n                // defining a sequence of candidates for each replica rank that are    
                                            // probabilistically independent of others’ failures.
            end if
            o = b.c(r, x) // section 3.4
            if type(o) != t then
              b = bucket(0)
              retry_bucket = true
            else if o is in o.array or failed or overloaded
              fr = fr + 1
              f = f + 1
              if o is in o.array and fr < 3 then
                retry_bucket = true
              else
                retry_descent = true
              end if
            end if
          until // retry bucket
        until // retry descent  
      o.array.append(o)
    end for
  end for
  i.array = o.array // copy output back to i.array

end procedure

procedure EMIT()
 R.array.append(i.array) // append working vector to result
end procedure

```


<br>

**example**
- distributing three replicas across thre cabinets in the same row

```
take(root)  root
select(1, row)  row2
select(3, cabinet) cab21 cab23 cab24
select(1, disk) disk2107  disk2013  disk2437
emit
```

- `select(1,row)` chooses a single bucket of type “row” (it selects row2).
- `select(3,cabinet)` chooses three distinct cabinets n
- `select(1,disk)` iterates over the three cabinet buckets in the input vector and chooses a single disk nested beneath each of them
- three disks spread over three cabinets, but in the same row
  - allows replicas to be simultaneously separated across
  - constrained within container types
  - `take, emit` blocks allow storage targets to be explicitly drawn from different pools of storage,



<br>


**Collisions Failure and Overload**
- `select(n,t)` operation may traverse many levels of the storage hierarchy in order to locate n distinct items of the specified type t
- During this process, CRUSH may reject and reselect items using a modified input
  -  `collision` if an item has already been selected in the current se
  - if a device is `failed`
  - if a device is `overloaded`.
  - CRUSH uniformly redistributes items across the storage cluster by restarting the recursion at the beginning of the select(n,t)


<br>

**Map Changes and Data Movement**
- When an individual device fails,
  - CRUSH flags the device but leaves it in the hierarchy
  - where it will be rejected and its contents uniformly redistributed by the placement algorithm
  - The CRUSH mapping process, which uses the cluster map as a weighted hierarchical decision tree,
  - can result in additional data movement beyond the theoretical optimum


<br>

**bucket types**
- Uniform Buckets
  - CRUSH can map replicas into uniform buckets in constant time
  - if the size of a uniform bucket changes, there is a complete reshuffling of data between devices,
  - When buckets are expected to be fixed (e. g.,a shelf of identical disks), uniform buckets are fastest.
- List buckets
  - Items removed from the middle or tail of the list, however, can result in a significant amount of unnecessary movement,
  - bucket is only expected to expand, list buckets provide optimal data movement when new items are added at the head of the list
- Tree buckets
  - this strategy keeps movement to a reasonable level, while offering efficient mapping even for very large buckets
  - an all around compromise, providing excellent performance and decent reorganization efficiency
- Straw buckets
  - The straw bucket type allows all items to fairly “compete” against each other for replica placement through a process analogous to a draw of straws.
  - straw buckets result in optimal data movement between nested items when modified.
  - In circumstances where removal is expected and reorganization efficiency is critical (e. g., near the root of the storage hierarchy), straw buckets provide optimal migration behavior between subtrees
