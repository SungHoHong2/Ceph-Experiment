### SSH connection without password
1. create several nodes with snapshot
2. NICs should be all initialized for each snapshot
3. create the NAT network from the virtual-box preference (upper-bar)
    - create NAT network
4. enable all node's first network adapter into NAT network
    - first network preferred since it is likely the first ethernet shares the default subnet mask and enabling the second NIC adapter into  NAT network does not work

5. **each nodes will have NIC for NAT network and internet access**


<br>

### Node configuration

```
// install ssh server
$ sudo apt-get install openssh-server
$ vi /etc/ssh/sshd_config
  + AllowUsers root sungho chara etc

$ service ssh start or stop

// ssh host
$ adduser frisk
$ passwd ******

// you can use $ pkexec visudo if you made an error in the pseudo file
$ sudo vi /etc/sudoers
  root	ALL=(ALL) ALL
  jkeesh	ALL=(ALL) ALL // add this for root permission

// client
$ ssh frisk@[address]

```

<br>

### Allow host -> guest ssh login [OPTIONAL]
- connect host-only adpater in virtual box
- fix the subnet with the host ip address
```
$  /etc/network/interfaces
auto eth0
iface eth0 inet static

address 192.168.1.128
netmask 255.255.255.0
network 192.168.1.0
broadcast 192.168.1.255
gateway 192.168.1.1
```

<br>

### Root Node self key test
```
ssh-keygen -t rsa -P ""
cat ./.ssh/id_rsa.pub >> ./.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
```

<br>

### Client Node for creating keys
- currently if we use the username the ssh works
```
$ ssh-keygen
$ ssh-copy-id -i /path/to/key.pub username@10.0.2.7

$ vi /etc/ssh/sshd_config
  + AllowUsers root sungho chara etc
```

<br>

### Allow password-less root privilege
- must add the new line below this passage
```
# Allow members of group sudo to execute any command
$ sudo vi /etc/sudoers

...
... %sudo   ALL=(ALL:ALL) ALL
+ username  ALL=(ALL) NOPASSWD:ALL
...

```

<br>

### Change hosts and hostname
- both hosts and hostname should be configured for Ceph to find the correct location
```
$ sudo vi /etc/hosts hostname
+ hosts are for all the address for you and the fellow nodes
+ hostname are for your node name
```
