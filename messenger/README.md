### How src/msg/async works
  - [x] CRUD commands for pools
  - [ ] The definition of pools in Ceph
  - [ ] CRUD commands for data objects  
  - [ ] know the basic CRUD of LIBRADOS
  - [ ] debug how the CRUD functions affect the  

<br>

### CRUD commands for pools
  1. **creating pool**
      - for some reason directly creating the pool from rados fails

```
$ ceph osd pool create {pool-name} {pg-num} [{pgp-num}]
$ ceph osd lspools
$ rados df // available to check the file from here
```

  2. **renamng pool**
```
ceph osd pool rename {current-pool-name} {new-pool-name}
```

  3. **removing pool**
      - need to set the mon_allow_pool_delete config option to true


  4. **pool snapshot**
```
$ ceph osd pool mksnap {pool-name} {snap-name}
$ ceph osd pool rmsnap {pool-name} {snap-name} //this will remove the snapshot from that pool
```


<br>


### CRUD commands for data objects  

  1. **CREATE object**

```
rados -p {pool-name} put myobject blah.txt
```
  2. **RETREIVE objects**
```
rados -p foo ls -
```
  3. **DELETE objects**
```
rados -p foo rm myobject
```
  4. **SNAPSHOT objects**
      - ignored due to other pending issues with higher priority
