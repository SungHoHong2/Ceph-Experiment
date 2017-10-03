### SSH connection without password
1. create several nodes with snapshot
2. NICs should be all initialized for each snapshot
3. create the NAT network from the virtual-box preference (upper-bar)
    - create NAT network
4. enable all node's first network adapter into NAT network
    - first network preferred since it is likely the first ethernet shares the default subnet mask and enabling the second NIC adapter into  NAT network does not work

5. **each nodes will have NIC for NAT network and internet access**


### Node configuration

```
// install ssh server
$ sudo apt-get install openssh-server
$ vi /etc/ssh/sshd_config
  Port 22222 //if you want to change the port (22222)
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

### Root Node self key test
```
ssh-keygen -t rsa -P ""
cat ./.ssh/id_rsa.pub >> ./.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
```

### Client Node for creating keys
- currently if we use the username the ssh works
- need to find ssh root login
```
$ ssh-keygen
$ ssh-copy-id -i /path/to/key.pub username@10.0.2.7

$ vi /etc/ssh/sshd_config
  + AllowUsers root sungho chara etc
```

### Allowing direct root logins
- still prompts the password even if the key is installed to the ssh server
```
$ vi /etc/ssh/sshd_config
  + AllowUsers root sungho chara etc

// add root pasword
$ passwd root

$ ssh-keygen
$ ssh-copy-id -i /path/to/key.pub root@10.0.2.7
```


### Allow sudo password-less privilege
- must add the new line below this passage
```
# Allow members of group sudo to execute any command
%sudo   ALL=(ALL:ALL) ALL

%username  ALL=(ALL) NOPASSWD:ALL
```
