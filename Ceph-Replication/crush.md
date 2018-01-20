### Crush
- [CRUSH: Controlled, Scalable, Decentralized Placement of Replicated Data](https://ceph.com/wp-content/uploads/2016/08/weil-crush-sc06.pdf)

**basics**
- pseudo random
  - fast calculation, no lookup
  - repeatable, deterministic

- statistically uniform distribution
- stable mapping
  - limited data migration on change
- rule based CONFIGURATION
  - infrastructure topology aware adjustable replication and weighting


<br>

**how crush finds the key**
- all the calculation is done within the client

<br>

**replication**
- if there is a node failure
- the osds are constantly watching the situation
- `you throw the basketball and the hoop moves to the right direction`
- client has a way of find the way and the server has a way of organizing the data


<br>

**layered block device**
- makes a copied layer of the data and the client will read from the copy
- this part is one of the unqiue way of ceph


<br>
