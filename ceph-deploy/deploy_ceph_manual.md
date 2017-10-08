### Preface
- allow all nodes to have password-less ssh connection
- add hosts to easily recognize the ip-address by string
  - admin, child1(monitor), child2

<br>


### Download Ceph
```
git clone --recursive https://github.com/ceph/ceph.git
git submodule update --force --init --recursive
git checkout -b luminous  
```
<br>

### before installing
- need to install python-rados
- change the hostname from /etc/hostname for each nodes


### download dependencies
```
./install-deps.sh
./do_cmake.sh
cd build
make
sudo make install
```

<br>

### General idea of the nodes
- the installment of ceph takes a quite a long time so install one and clone them
- all the nodes can share the keys and configuration file (it might be best to configure everything and then clone them)
- monitor is not a node even if the configuration file has declared it as one of the node


<br>

### create monitor
- use uuidgen command to created fsid
- create configuration file
- need to add the Add mgr daemon in the monitor

```
Create a /etc/ceph/ceph.conf file

[global]
fsid = f9a8ed78-92aa-4c20-a42e-bf97431698cf
mon initial members = node1              
mon host = x.x.x.x
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd journal size = 1024
osd pool default size = 2
osd pool default min size = 1
osd pool default pg num = 333
osd pool default pgp num = 333
osd crush chooseleaf type = 1
```

- create keyring and give permission process for monitor
```
ceph-authtool --create-keyring /etc/ceph/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
```

- create keyring and give permission process for admin
```
ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'
```

- Add client.admin keyring to the monitor keyring
```
ceph-authtool /etc/ceph/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
```

- Create monitor map, where node1 is the hostname of monitor, x.x.x.x is its IP address, 240f2d59-dcd3-474e-b459-8d872b38dca2 is just put in the configuration file fsid.
```
monmaptool --create --add node1 x.x.x.x --fsid f9a8ed78-92aa-4c20-a42e-bf97431698cf /etc/ceph/monmap
```

- Create monitor default data directory, the name of the rule is mkdir / var / lib / ceph / mon / {cluster-name} - {hostname}
  - the default {cluster-name} is ceph
```
mkdir /var/lib/ceph/mon/ceph-node1
```

- Create a monitor files. Where node1 is hostname
```
ceph-mon --mkfs -i node1 --monmap /etc/ceph/monmap --keyring /etc/ceph/ceph.mon.keyring
```

- Create a file named done in this directory that monitor creation is complete
```
sudo touch /var/lib/ceph/mon/ceph-node1/done
```

- Restart automatically after the need to create a file:
```
touch /var/lib/ceph/mon/ceph-node1/upstart
```

- Run monitor
```
sudo ceph-mon -i node1 -c /etc/ceph/ceph.conf
```

- testing Monitor
```
ceph osd lspools
0 rbd，
ceph -s (status)
```
<br>

### Installing OSD

```
[global]
fsid = 240f2d59-dcd3-474e-b459-8d872b38dca2
mon initial members = c3n21
mon host = xxxxxxx
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd journal size = 1024
osd pool default size = 2
osd pool default min size = 1
osd pool default pg num = 64
osd pool default pgp num = 64
osd crush chooseleaf type = 1

enable experimental unrecoverable data corrupting features = bluestore rocksdb
osd objectstore = bluestore
bluestore default buffered read = true

[mgr.0]
mgr data = /var/lib/ceph/mgr.0
mgr log = /var/lib/ceph/0.log

[osd.0]
host = xxxx
osd data = /mnt/osd-device-0-data
bluestore block path = /dev/disk/by-partlabel/osd-device-0-block
bluestore block db path = /dev/disk/by-partlabel/osd-device-0-db
bluestore block wal path = /dev/disk/by-partlabel/osd-device-0-wal

[osd.1]
host = xxxxx
osd data = /mnt/osd-device-1-data
bluestore block path = /dev/disk/by-partlabel/osd-device-1-block
bluestore block db path = /dev/disk/by-partlabel/osd-device-1-db
bluestore block wal path = /dev/disk/by-partlabel/osd-device-1-wal
```


- Return value for the osd id number, and then use ceph osd dump can be seen just created osd uuid. This id need to write down in the back to create a data file used. Then create the osd data directory:

```
mkdir -p /mnt/osd-device-0-data
```

- The name rule is still the cluster name plus the osd name. And then use the following order to prepare our osd installed in this directory.

```
sudo mount -t xfs /dev/disk/by-partlabel/osd-device-0-data /mnt/osd-device-0-data
```

- And then create the osd data file, the number 0 for the osd number, u uid is the first step inside we generated uuid number:
```
sudo ceph-osd -i 0 --mkfs --mkkey --osd-uuid [{uuid}]
```

- Register osd authorization key
```
sudo ceph auth add osd.0 osd 'allow *' mon 'allow profile osd' mgr 'allow profile osd' -i /mnt/osd-device-0-data/keyring
```

- Add a tree bucket to osd crush and change its position
```
ceph osd crush add-bucket c3n21 host
ceph osd crush move c3n21 root=default
```

- Add the osd.0 we just created and give weight 1 to c3n21
```
ceph osd crush add osd.0 1.0 host=c3n21
```


- Start the osd daemon:
```
sudo ceph-osd -i 0 -c /etc/ceph/ceph.conf
```

<br>

### establish a mgr daemon

```
ceph auth get-or-create mgr.0 mon 'allow profile mgr' osd 'allow *' mds 'allow *' > /var/lib/ceph/mgr.0/keyring
ceph-mgr -i 0 -c /etc/ceph/ceph.conf

```
