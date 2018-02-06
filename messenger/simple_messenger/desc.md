SimpleMessenger create two threads
The messenger classes that you see which sends the messages puts the message to the queue
and the Pipe write is the one actually takes out the pipes from the queue and run the operation

### How does Ceph write object
1. Get the Admin keyring
2. initialize rados object
3. set up rados with the configuration from the ceph.conf file
4. connect by invoking `rados_connect`
    1. reads the required info from the configuration file
    2. read the monitor information from monitor map
    3. initialize a `client messenger` **what does this client messenger do**
    4. set policy for the client messenger
    5. initialize `objecter` **is objector used for all purposess**
      1. pass configuration file as a argument
      2. pass messenger as a argument
      3. pass monitor as a argument
  6. set objecter with balanced budget   
  7. set monitor with messenger pointer
  8. set manager with messenger pointer
  9. initializze objector
  10. assign mgr client client in the dispatcher head of the messenger
  11. assign objecter in the dispatcher tail of the messenger
  12. assign rados connect in the dispatcher tail of the messenger
  13. start messenger
  14. set monitor with keys
  15. initialize monitor client
  16. initialize mgr client
  17. `objecter` start
  18. lock
  19. timer start
  20. finisher start
  21. assign global id from monitor client
5. add hello data in the bufferlist
6. run `ioctx.write_full` to submit data in the object pool
  1. lock
  2. check version
  3. run C_SafeCond and assign it with a lock
  4. objecter runs prepare_mutate_op **is this a snapshot the function?**
  5. objector runs op_submit
    1. lock read and write
    2. run op_submit with budget
    3. calculate the latest map for the latest map **RECALC_OP_TARGET_POOL_DNE**
    4. check the request is either read or write
    5. run `_send_op`  to send the data
      1. prepare OSD operation
      2. get the connection reference which is the `Pipeconnection`
      3. run `Simple Connection` to send message
      4. get the destination from messenger
      5. lock
      6. look up pipe with destination address **how the pipeline works?**
      7. submit message using a pipe
        1.
      8. unlock
    6. unlock the read and write lock
    7. free the session
  6. wait unti lthe C-SafeCon is finished
  7. sync operation version  
