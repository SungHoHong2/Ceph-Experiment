### looking for the place that sends the data
- it seems that pipe sends the data
- Create one OSD
- initialize multiple SimpleMessenger
- each SimpleMessenger initialize PipeConnection
- PipeConnection initialize the pipe
- Pipe runs the reader thread and writer thread

```c++
Messenger   *cluster_messenger;
Messenger   *client_messenger;
Messenger   *objecter_messenger;

OSD::OSD(CephContext *cct_, ObjectStore *store):
      cluster_messenger(internal_messenger), // FIXME: internal_messenger
      client_messenger(external_messenger),  // FIXME: external_messenger
      objecter_messenger(osdc_messenger),    // FIXME: osdc_messenger
```


### how the data is sent
- first when the osd initializes it registers to the monitor
- second when the data is sent it needs to get the authorization from the monitor
- third osd sends the data

### the solution
- we will build a seastar that will redirect the nodes based on the ip address right nows
