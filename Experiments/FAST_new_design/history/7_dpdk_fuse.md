### running directly from DPDK
- need more experiments, simply doing this does not work
- lets test it with e the
- https://github.com/SungHoHong2/DPDK-Experiment/blob/master/pingpong-experiment/client/l2fwd_launch_one_lcore.h
- this part you have managed to send the data with your own pktmbuf_pool.
- running the loop on the normal thread works
-


EAL pthread and lcore Affinity
The term “lcore” refers to an EAL thread, which is really a Linux/FreeBSD pthread. “EAL pthreads” are created and managed by EAL and execute the tasks issued by remote_launch. In each EAL pthread, there is a TLS (Thread Local Storage) called _lcore_id for unique identification. As EAL pthreads usually bind 1:1 to the physical CPU, the _lcore_id is typically equal to the CPU ID.


non-EAL pthread support
It is possible to use the DPDK execution context with any user pthread (aka. Non-EAL pthreads).
In a non-EAL pthread, the _lcore_id is always LCORE_ID_ANY which identifies that it is not an EAL thread with a valid, unique, _lcore_id. Some libraries will use an alternative unique ID (e.g. TID), some will not be impacted at all, and some will work but with limitations (e.g. timer and mempool libraries).


Known Issues
The rte_mempool uses a per-lcore cache inside the mempool. For non-EAL pthreads, rte_lcore_id() will not return a valid number.
So for now, when rte_mempool is used with non-EAL pthreads, the put/get operations will bypass the default mempool cache and there is a performance penalty because of this bypass. Only user-owned external caches can be used in a non-EAL context


rte_ring
rte_ring supports multi-producer enqueue and multi-consumer dequeue. However, it is non-preemptive, this has a knock on effect of making rte_mempool non-preemptable.
This does not mean it cannot be used, simply, there is a need to narrow down the situation when it is used by multi-pthread on the same core.
It CAN be used for any single-producer or single-consumer situation.
It MAY be used by multi-producer/consumer pthread whose scheduling policy are all SCHED_OTHER(cfs). User SHOULD be aware of the performance penalty before using it.
It MUST not be used by multi-producer/consumer pthreads, whose scheduling policies are SCHED_FIFO or SCHED_RR.


40. Performance Thread Sample Application
