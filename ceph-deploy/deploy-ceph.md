### General Picture
1. Ceph Admin node
2. Ceph Node
    - Monitor Node
    - Storage Node

<br>

### Install Ceph Admin Node
```
 wget -q -O- 'https://download.ceph.com/keys/release.asc' | sudo apt-key add -
```

<br>

### Ceph Admin permission
- ceph-deploy logs in to a Ceph node as a user, that particular user must have password-less sudo privileges.

<br>

### Create Short-cuts for ssh connection
```
$ vi ~/.ssh/config //create a file config in the ssh file
  Host nas01
     HostName 192.168.1.100
     User root
     IdentityFile ~/.ssh/nas01.key

$ ssh nas01 // after this connection will work without the ip address      
```

<br>

### Setting up firewalls for Ceph Monitors & OSDs
  - Ceph Monitors communicate using port 6789 by default.
  - Ceph OSDs communicate in a port range of 6800:7300 by default

```
iptables -I INPUT -j ACCEPT
```

<br>

### Deploy from admin to nodes
```
1. create cluster
  $ sudo vi /etc/hosts
  + 0.0.0.0   node_name

2. deploy monitor nodes
  $ ceph-deploy new [node-name]

3. install ceph packages to all nodes
  $ ceph-deploy install node1 node2 node3

```

### Stalled situation
- documentation requires the monitor nodes to have public address
- documentation requires at least 4 nodes with one monitor and managing nodes
- the virtual machine ip address is currently limited to NAT network, which does not recognize as a public address
- [link to the error log](ERROR_LOG)
- [link to the documentation](http://docs.ceph.com/docs/master/start/quick-ceph-deploy/)

```
Note If this process fails with a message similar to “Unable to find /etc/ceph/ceph.client.admin.keyring”, please ensure that the IP listed for the monitor node in ceph.conf is the Public IP, not the Private IP.
```
