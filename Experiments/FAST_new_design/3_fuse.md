### Weekly Report Objective
- Implement Cache with Fuse
- Integrate Fuse with Seastar
- Test performance on all filesystem


### Learning FUSE
- combine seastar with FUSE
- calculate the time-latency created by FUSE



### Writing a Simple Filesystem Using FUSE in C
http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
sudo apt-get install libfuse-dev

FUSE (Filesystem in Userspace) is an interface that let you write your own filesystem for Linux in the user space
`Actually you can’t build a native filesystem which can store data to disk directly`
Instead you need to use an already exist filesystem to do so
for example GDFS is a filesystem which let you mount your Google Drive in your system and access the files there as normal files

**Simple Stupid Filesystem (SSFS)**
