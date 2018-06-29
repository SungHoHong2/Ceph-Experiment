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


### building Seastar version cusotmized to the needs
- need to modify the packets into fixed size so that there would be no problem
- polling is designed that is done in the MPI library
- once the connection is on it sends a fixed fake data.
- the test will be done on one OSD (OSD sends data to localhost when they use the data)
- after that I will test it on the server but need permission in the cluster. 3 cluster (one montior, two osds)
- I don't need to modify the montior so need a node that can execute one.
- I am having trouble using sharding with Seastar,
- need to experience more about how the sharding is done in Seastar.
- The data sends fine when only use the client and server interface separately in each node.
- but the application is not working properly when we combine client and server together.


### use two independent threads
- use two different threads to run the Seastar client and server application.
- when the read and write functions of the socket run indepenently the data is often send as duplicates
- send fixed size packets
- listening interface only accepts the fixed size packets and ignore overwhelmend packets
- need to send dummy packet from the client with the same size





Hello^
