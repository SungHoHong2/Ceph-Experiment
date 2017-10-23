ceph-authtool --create-keyring /etc/ceph/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'

wait

ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'

wait

ceph-authtool /etc/ceph/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring

wait

monmaptool --create --add monitor01 10.0.2.14 --fsid f9a8ed78-92aa-4c20-a42e-bf97431698cf /etc/ceph/monmap

wait

mkdir /var/lib/ceph/mon/ceph-monitor01

wait

ceph-mon --mkfs -i monitor01 --monmap /etc/ceph/monmap --keyring /etc/ceph/ceph.mon.keyring

wait

touch /var/lib/ceph/mon/ceph-monitor01/done
touch /var/lib/ceph/mon/ceph-monitor01/upstart
