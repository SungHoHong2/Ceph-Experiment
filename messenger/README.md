### How src/msg/async works
  - [ ] know the basic CRUD of LIBRADOS
  - [ ] debug how the CRUD functions affect the  
  - [ ]

<br>

### Creating pools for storing data
  1. **creating pool**
    -  for some reason directly creating the pool from rados fails
    ```
    $ ceph osd pool create {pool-name} {pg-num} [{pgp-num}]
    $ ceph osd lspools
    $ rados df // available to check the file from here
    ```
