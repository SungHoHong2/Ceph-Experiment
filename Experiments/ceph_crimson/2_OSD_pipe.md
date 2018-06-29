### Design for Simple-Messenger - Pipe
- need to completely understand how the command is received and then work as _send_message
- suspected order
  1) read from accept in pip.cc
  2) add the message in the queue
  3) the message in the queue is treated as a command
  4) the command runs in the OSD


**makefile**
- need to find the makefile that build ceph-osd execution file

**server**
- run two different reads (POSIX and DPDK)
  - pip.cc refer to the `accept` function
- create two different queues for (POSIX and DPDK)
  - refer in simple messenger `_send_message`
- submit to different queues based on different data.
-
- accept both two types of queues

**client**
- redirect the destination address to DPDK when transferring data to OSD.
