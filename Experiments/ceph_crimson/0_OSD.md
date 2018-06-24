```c++
Messenger   *cluster_messenger;
Messenger   *client_messenger;
Messenger   *objecter_messenger;

OSD::OSD(CephContext *cct_, ObjectStore *store):
cluster_messenger(internal_messenger), // FIXME: internal_messenger
client_messenger(external_messenger),  // FIXME: external_messenger
objecter_messenger(osdc_messenger),    // FIXME: osdc_messenger


```
