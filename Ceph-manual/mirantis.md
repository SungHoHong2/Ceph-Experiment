### Ceph Tactics
[related page](https://docs.mirantis.com/openstack/fuel/fuel-9.1/assets/CephBestPractices.pdf)

In writeback mode, the data in the cache tier must be `replicated` or `erasure­coded` to ensure data is not lost 

<br>


### Implementation

- **deployment**
  - deploy the cache nodes as regular Ceph nodes.
  - rules are created in the crush map to place the cache pools on the SSD backed OSDs 
  - the regular pools on HDD backed OSDs.


- **Cached Pools**
  - a separate cache pool must be created on the SSD infrastructure for each pool that requires caching.

  - The pools that benefit most from caching are pools that either have high performance SLAs and `experience heavy` read and especially `write traffic`


- **Copy ­on ­Write**
  - [Block devices and Open stack](http://docs.ceph.com/docs/master/rbd/rbd­openstack/) 
  - [Ceph Cache Tiering documentation](http://docs.ceph.com/docs/master/rados/operations/cache­tiering)

  
