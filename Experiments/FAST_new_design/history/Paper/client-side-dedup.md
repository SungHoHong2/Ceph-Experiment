**abstract**
Methods and systems that use a client locality table when performing client-side data deduplication
searching one or more client locality tables for the signature of a data unit
If the signature is not found in the client locality tables, the signature is sent from a deduplication client to a deduplication server
added to one of the client locality tables.
If instead the signature is found in the client locality tables, sending of the new signature to the deduplication server is inhibited.


**How the deduplication are compared**
the detection is usually performed by comparing Smaller data signatures of each data unit instead of comparing the data units themselves.
existing techniques for detecting redundant copies often reduce system performance.

storing just the 45 signatures requires 40 GB of memory. As a result, these signatures cannot be held in computer memory on any but the most expensive computer systems

little locality of refer- 55 ence among signatures and each I/O will require seeking to different parts of the disk,
