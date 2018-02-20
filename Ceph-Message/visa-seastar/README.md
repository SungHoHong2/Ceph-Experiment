```
# running on DPDK
./server --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 2

./client --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.251 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 2
```

./a.out --dpdk-pmd --network-stack native --dhcp 0 --host-ipv4-addr 10.218.111.253 --netmask-ipv4-addr 255.255.248.0 --gw-ipv4-addr 10.218.111.1 --collectd 0 --smp 2

### FINDING out the reason why Seastar is not installed
- whatever I do it will only link to the older version of liboost
- so I think I have to find the configure.py


```
sungho@c3n24:/data1/sungho/seastar$ ldd a.out                                         
        linux-vdso.so.1 =>  (0x00007fffefbfc000)                                      
        libboost_program_options.so.1.66.0 => /usr/lib/x86_64-linux-gnu/libboost_prog$
am_options.so.1.66.0 (0x00007f5f92cc0000)                                             
        libboost_system.so.1.66.0 => /usr/lib/x86_64-linux-gnu/libboost_system.so.1.6$
.0 (0x00007f5f92abc000)                                                               
        libboost_filesystem.so.1.66.0 => /usr/lib/x86_64-linux-gnu/libboost_filesyste$
.so.1.66.0 (0x00007f5f928a2000)                                                       
        libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f5f9258f000
)                                                                                     
        libboost_thread.so.1.66.0 => /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.66


sungho@c3n25:/data1/sungho/seastar$ ldd a.out                                         
        linux-vdso.so.1 =>  (0x00007fff891fc000)                                      
        libboost_program_options.so.1.54.0 => /usr/lib/x86_64-linux-gnu/libboost_progr
am_options.so.1.54.0 (0x00007feb43a1a000)                                             
        libboost_system.so.1.54.0 => /usr/lib/x86_64-linux-gnu/libboost_system.so.1.5$
.0 (0x00007feb43816000)                    
        libboost_filesystem.so.1.54.0 => /usr/lib/x86_64-linux-gnu/libboost_filesyste$
.so.1.54.0 (0x00007feb435ff000)            
        libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007feb432ec00$

```
