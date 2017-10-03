### Install Ceph Admin Node
```
 wget -q -O- 'https://download.ceph.com/keys/release.asc' | sudo apt-key add -


```

### SSH connection without password
```
$ sudo apt-get install openssh-server
$ sudo systemctl restart ssh
$ service ssh start or stop


```
