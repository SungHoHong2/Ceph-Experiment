ceph-authtool --create-keyring /etc/ceph/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'

wait 

ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'

wait 

ceph-authtool /etc/ceph/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring

wait 

monmaptool --create --add node1 10.0.2.10 --fsid f9a8ed78-92aa-4c20-a42e-bf97431698cf /etc/ceph/monmap

wait 

mkdir /var/lib/ceph/mon/ceph-node1

wait 

ceph-mon --mkfs -i node1 --monmap /etc/ceph/monmap --keyring /etc/ceph/ceph.mon.keyring

wait 

touch /var/lib/ceph/mon/ceph-node1/done
touch /var/lib/ceph/mon/ceph-node1/upstart
