
### default configuration

```
// added in the ceph.conf file in both OSD and monitor
// case 1: using simple
ms_type = simple

// case 2: using asynchronous
ms_type = async+posix
```


<br>

### running write for 10 seconds without cleanup
- running command
```
$ rados bench -p test 10 write --no-cleanup
```
- asynchronous test
```
Maintaining 16 concurrent writes of 4194304 bytes to objects of size 4194304 for up to
 10 seconds or 0 objects                   
Object prefix: benchmark_data_wenji-wrk_32043                                         
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)         
    0       0         0         0         0         0           -           0         
    1      16        18         2   7.99958         8    0.763515    0.593221         
    2      16        21         5   9.99915        12     1.83858     1.12538         
    3      16        24         8   10.6656        12     2.90522     1.65951         
    4      16        27        11   10.9988        12     3.97179     2.19469         
    5      16        29        13   10.3989         8     4.69652     2.55204         
    6      16        32        16   10.6655        12     5.76314     3.08693         
    7      16        35        19   10.8559        12     5.72674     3.50195         
    8      16        38        22   10.9987        12     5.71805     3.80341         
    9      16        41        25   11.1098        12     5.70951     4.03181         
   10      16        43        27   10.7988         8     5.72634      4.1567         
   11      14        44        30   10.9078        12     5.70969     4.31201         
   12      11        44        33   10.9987        12     5.70139     4.43882         
   13       8        44        36   11.0756        12     5.70947     4.54517         
   14       5        44        39   11.1416        12     5.71795     4.63496         
   15       3        44        41   10.9321         8     5.69287     4.68719         
Total time run:         15.760662          
Total writes made:      44                 
Write size:             4194304            
Object size:            4194304            
Bandwidth (MB/sec):     11.167             
Stddev Bandwidth:       1.83095            
Max bandwidth (MB/sec): 12                 
Min bandwidth (MB/sec): 8                  
Average IOPS:           2                  
Stddev IOPS:            0                  
Max IOPS:               3                  
Min IOPS:               2                  
Average Latency(s):     4.7571             
Stddev Latency(s):      1.62389            
Max latency(s):         5.76314            
Min latency(s):         0.422928           
```

- simple messenger test
```
Maintaining 16 concurrent writes of 4194304 bytes to objects of size 4194304 for up t
 10 seconds or 0 objects                   
Object prefix: benchmark_data_wenji-wrk_32160                                        
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)        
    0       0         0         0         0         0           -           0        
    1      16        18         2   7.99965         8    0.778403    0.603725        
    2      16        21         5     9.999        12     1.84497     1.13351        
    3      16        24         8   10.6655        12     2.90334     1.66489        
    4      16        27        11   10.9988        12     3.97824     2.20036        
    5      16        29        13   10.3988         8      4.7034      2.5585        
    6      16        32        16   10.6654        12     5.76138     3.09346        
    7      16        35        19   10.8559        12     5.71807     3.50567        
    8      16        38        22   10.9987        12     5.70959     3.81227        
    9      16        41        25   11.1098        12     5.70142     4.04063        
   10      16        43        27   10.7987         8      5.7013     4.16334        
   11      14        44        30   10.9078        12     5.70098     4.31658        
   12      11        44        33   10.9987        12     5.70934     4.44296        
   13       8        44        36   11.0756        12     5.70123     4.54804        
   14       5        44        39   11.1415        12     5.69264     4.63312        
   15       3        44        41    10.932         8     5.70934     4.68521        
Total time run:         15.758034          
Total writes made:      44                 
Write size:             4194304            
Object size:            4194304            
Bandwidth (MB/sec):     11.1689            
Stddev Bandwidth:       1.83095            
Max bandwidth (MB/sec): 12                 
Min bandwidth (MB/sec): 8                  
Average IOPS:           2                  
Stddev IOPS:            0                  
Max IOPS:               3                  
Min IOPS:               2                  
Average Latency(s):     4.75525            
Stddev Latency(s):      1.61916            
Max latency(s):         5.84311            
Min latency(s):         0.429046           
```

<br>


### running 10 second sequential read on rbd pool

- running command
```
rados bench -p test 10 seq
```
- asynchronous test
```                             
2018-01-29 18:21:32.300109 7f9c538c6180  0 Messenger ===@@@ Create Messenger radosclie
nt                   
2018-01-29 18:21:32.301664 7f9c3d7fa700  0 -- run_local_delivery- run_local_delivery  
2018-01-29 18:21:32.302585 7f9c538c6180  0 monclient(hunting): start try to connect 10
.218.111.252:6789/0  
hints = 1            
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)         
    0       0         0         0         0         0           -           0         
    1      16        18         2   7.99894         8    0.795712    0.617809         
    2      16        21         5    9.9987        12     1.86597     1.15278         
    3      16        24         8   10.6653        12     2.93641     1.68773         
    4      16        26        10   9.99874         8     3.64954     2.04441         
    5      16        29        13   10.3987        12      4.7203      2.5795         
    6      16        32        16   10.6654        12     5.79183     3.11486         
    7      16        35        19   10.8559        12     5.70933      3.5245         
    8      16        38        22   10.9988        12     5.70922     3.82238         
    9      16        40        24   10.6655         8     5.70949     3.97967         
   10      16        43        27   10.7988        12     5.70933     4.17183         
   11      14        44        30   10.9079        12     5.70912     4.32563         
   12      13        44        31   10.3322         4     5.70758      4.3702         
   13      13        44        31   9.53741         0           -      4.3702         
   14       9        44        35    9.9989         8     5.70724     4.59455         
   15       8        44        36   9.59894         4      5.7076     4.62547         
Total time run:       15.784622            
Total reads made:     44                   
Read size:            4194304              
Object size:          4194304              
Bandwidth (MB/sec):   11.1501              
Average IOPS:         2                    
Stddev IOPS:          1                    
Max IOPS:             3                    
Min IOPS:             0                    
Average Latency(s):   5.13116              
Max latency(s):       9.99615              
Min latency(s):       0.439906             
```

- simple messenger test

```
2018-01-29 18:32:44.108854 7f40c27fc700  0 -- run_local_delivery- run_local_delivery  
2018-01-29 18:32:44.109765 7f40d75f0180  0 monclient(hunting): start try to connect 10
.218.111.252:6789/0  
hints = 1            
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)         
    0       0         0         0         0         0           -           0         
    1      16        18         2   7.99879         8     0.76335    0.584807         
    2      16        21         5   9.99843        12     1.83219     1.11952         
    3      16        24         8    10.665        12     2.90271     1.65423         
    4      16        27        11   10.9983        12     3.97276     2.18912         
    5      16        29        13   10.3984         8     4.68582     2.54583         
    6      16        32        16    10.665        12     5.75581     3.05546         
    7      16        35        19   10.8554        12     5.70623     3.49539         
    8      16        38        22   10.9983        12     5.70621     3.79689         
    9      16        41        25   11.1094        12     5.70832     4.02613         
   10      16        43        27   10.7983         8     5.70579     4.15059         
   11      15        44        29   10.5438         8     5.70652     4.25789         
   12      13        44        31   10.3317         8     5.70628     4.35134         
   13      13        44        31   9.53696         0           -     4.35134         
   14       9        44        35   9.99843         8     6.42288     4.64904         
   15       6        44        38   10.1317        12     6.78049     4.80775         
Total time run:       15.749513            
Total reads made:     44                   
Read size:            4194304              
Object size:          4194304              
Bandwidth (MB/sec):   11.1749              
Average IOPS:         2                    
Stddev IOPS:          0                    
Max IOPS:             3                    
Min IOPS:             0                    
Average Latency(s):   5.10079              
Max latency(s):       9.98908              
Min latency(s):       0.406264             
```

<br>


### running 10 second random read on rbd pool

- running command
```
rados bench -p test 10 rand
```
- asynchronous test
```
2018-01-29 18:28:35.736710 7f305b7fe700  0 -- run_local_delivery- run_local_delivery  
2018-01-29 18:28:35.737500 7f3079a45180  0 monclient(hunting): start try to connect 10
.218.111.252:6789/0  
hints = 1            
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)         
    0       0         0         0         0         0           -           0         
    1      16        18         2   7.99856         8    0.715489    0.537303         
    2      16        21         5   9.99808        12     1.78548     1.07216         
    3      16        24         8   10.6647        12     2.85613     1.60742         
    4      16        27        11   10.9981        12     3.92467     2.14231         
    5      16        30        14   11.1983        12     4.99577     2.67726         
    6      16        32        16    10.665         8     5.70828     3.03387         
    7      16        35        19   10.8555        12     5.70689     3.45594         
    8      16        38        22   10.9983        12     5.70572      3.7629         
    9      16        41        25   11.1094        12     5.70623     3.99611         
   10      16        44        28   11.1983        12     5.70744     4.17947         
   11      16        45        29   10.5438         4     5.70652     4.23213         
   12      12        45        33   10.9983        16     5.70548     4.42164         
   13       9        45        36   11.0752        12     5.70641     4.53868         
   14       7        45        38   10.8555         8     5.70675      4.6002         
   15       3        45        42   11.1983        16     5.70733     4.71411         
   16       2        45        43   10.7484         4     5.70723     4.73721         
Total time run:       16.058322            
Total reads made:     45                   
Read size:            4194304              
Object size:          4194304              
Bandwidth (MB/sec):   11.2091              
Average IOPS:         2                    
Stddev IOPS:          1                    
Max IOPS:             4                    
Min IOPS:             1                    
Average Latency(s):   4.78828              
Max latency(s):       6.065                
Min latency(s):       0.359117             
```

- simple messenger test
```
2018-01-29 18:33:31.941331 7ff621f6f700  0 -- run_local_delivery- run_local_delivery  
2018-01-29 18:33:31.942309 7ff6326ae180  0 monclient(hunting): start try to connect 10
.218.111.252:6789/0  
hints = 1            
  sec Cur ops   started  finished  avg MB/s  cur MB/s last lat(s)  avg lat(s)         
    0       0         0         0         0         0           -           0         
    1      16        18         2   7.99854         8    0.715988    0.537591         
    2      16        21         5   9.99815        12     1.78653     1.07325         
    3      16        24         8   10.6649        12     2.85838     1.60869         
    4      16        27        11   10.9982        12     3.92842     2.14409         
    5      15        29        14   11.1982        12     4.99815     2.67932         
    6      16        32        16   10.6649         8     5.71146     3.03602         
    7      16        35        19   10.8554        12     5.70897     3.45826         
    8      16        38        22   10.9983        12     5.70931      3.7652         
    9      16        41        25   11.1094        12     5.70679     3.99821         
   10      16        44        28   11.1983        12     5.70562     4.18123         
   11      15        45        30   10.9074         8     5.70682     4.28291         
   12      12        45        33   10.9983        12     5.70547     4.42329         
   13       9        45        36   11.0752        12     5.70742     4.53021         
   14       6        45        39   11.1412        12     5.70747     4.62076         
   15       3        45        42   11.1983        12     5.70632     4.70682         
   16       1        45        44   10.9983         8     5.70815     4.75233         
Total time run:       16.061321            
Total reads made:     45                   
Read size:            4194304              
Object size:          4194304              
Bandwidth (MB/sec):   11.207               
Average IOPS:         2                    
Stddev IOPS:          0                    
Max IOPS:             3                    
Min IOPS:             2                    
Average Latency(s):   4.77359              
Max latency(s):       6.06723              
Min latency(s):       0.359193             
```
