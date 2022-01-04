#Theta Memory Modes

The Intel Xeon Phi Generation 2 (KNL) has the ability to change the memory and clustering modes. The memory and cluster mode affect the overall performance of memory access. While many combinations of the memory and clustering modes exist, this discussion will address only the most commonly used configurations.

The primary suggestion is to begin with the quadrant clustering mode and the cache memory mode.

##Clustering Mode
The clustering mode defines an infinity between the thread’s memory allocations and the memory domain in which the allocation is placed. This placement also influences which cache tag directory on the mesh is used. The clustering mode will never prevent memory from being read by any thread. The clustering mode only influences the performance. The possible clustering modes are All2All, Hemisphere, Quadrant, Sub-NUMA Clustering-2 (SNC-2), and Sub-NUMA Clustering-4 (SNC-4). These options can be selected during your Cobalt job submission using the --attrs option. Table 1 lists the clustering mode and the associated Cobalt selection.

###Clustering Modes
| Clustering Mode |	Cobalt Syntax       |
|:--------------- | :------------------ |
| All2All	      |  --attrs=numa=a2a  |
| Hemisphere      |  --attrs=numa=hemi |
| Quadrant	      |  --attrs=numa=quad |
| SNC-2	          |  --attrs=numa=snc2 |
| SNC-4	          |  --attrs=numa=snc4 |

##Usage
For the a2a, hemi, and quad modes, no special configuration is needed as all memory is placed into a single NUMA domain. For the SNC-2 and SNC-4 modes, memory is split into two or four NUMA domains and each process must be assigned to a particular NUMA domain. This can be done via libnuma API calls or via the numactl command.

##Flat SNC-4 Example
```
aprun -N 16 -d 1 -cc depth -n 4 numactl -m 4 ./app : -n 4 numactl -m 5 ./app : -n 4 numactl -m 6 ./app : -n 4 numactl -m 7 ./app
```

*Note:* The -m parameter for numactl defines the region from which to allocate memory. The valid memory regions are determined by the mode selected. The All2All, Hemisphere, and Quadrant modes all define a single NUMA domain. The SNC-2 and SNC-4 modes define two or four domains, respectively. The memory mode when using flat, split, or equal will define twice the number of memory regions that the clustering mode defined. For example, the flat-quadrant mode would have two domains, 0 (DDR), and 1 (MCDRAM). The flat-snc4 mode has eight domains, 0..3 (DDR), and 4..7 (MCDRAM).

##Memory Mode
The memory mode defines how the KNL MCDRAM (high bandwidth memory) will be addressed. Two possible ways are “cache” (where the MCDRAM is transparent to the user and acts as a last-level cache between the L2 and DDR4 memory) and “flat” (in which the MCDRAM is directly addressable and appears as a separate NUMA domain). 

The cache and flat modes can be combined in two hybrid modes, where a portion of the MCDRAM is directly addressable and a portion is cache. The possible memory modes are flat, cache, equal, and split. Table 2 lists the memory modes and associated Cobalt syntax.

###Memory Modes
| Memory Mode |	Flat (%) | Cache (%) |	Cobalt Syntax |
|-------------|----------|-----------|----------------|
| Cache | 0 | 100 |	--attrs=mcdram=cache |
| Equal	 |   50 |	50	| --attrs=mcdram=equal |
| Split	|   75 | 25	| --attrs=mcdram=split |
| Flat	|  100 |	0| --attrs=mcdram=flat |

 
##Usage
There is no special configuration for addressing the MCDRAM in cache mode. The caching model will fetch data from DRAM and store it in MCDRAM as needed. The flat memory mode can be accessed one of two ways. The user can bind the process specifically to the MCDRAM using the “numactl” command or use the libmemkind library to allocate memory from a specific area. These two methods can be used in combination as well.

##Flat Quadrant Example
The application binds to MCDRAM so that all allocations are within the high bandwidth memory. If the program tries to allocate more than the 16 GB available MCDRAM, it will fail with an error:

```
aprun -n 64 -N 64 -d 1 -cc depth numactl -m 1 ./app
```

An alternative is to prefer allocation in MCDRAM, but allow allocations to spill over into DDR if more than 16 GB is allocated:
```
aprun -n 64 -N 64 -d 1 -cc depth numactl -p 1 ./app
```
See the numactl man page for more details.

##Results
Table 3 shows the STREAM Triad memory bandwidth for common configurations.

###STREAM Triad Performance
| Mode | Size | Triad |
|------|------|-------|
| Flat - MCDRAM | 7.5 GB | 485 GB/s |
| Flat - DRAM | 7.5 GB | 88 GB/s |
| Cache | 7.5 GB | 352 GB/s |
| Cache | 120.0 GB | 59 GB/s |
 

The Intel Xeon Phi Generation 2 (KNL) has the ability to change the memory and clustering modes. The memory and cluster mode affect the overall performance of memory access. While many combinations of the memory and clustering modes exist, this discussion will address only the most commonly used configurations.

The primary suggestion is to begin with the quadrant clustering mode and the cache memory mode.

##Clustering Mode
The clustering mode defines an infinity between the thread’s memory allocations and the memory domain in which the allocation is placed. This placement also influences which cache tag directory on the mesh is used. The clustering mode will never prevent memory from being read by any thread. The clustering mode only influences the performance. 

The possible clustering modes are All2All, Hemisphere, Quadrant, Sub-NUMA Clustering-2 (SNC-2), and Sub-NUMA Clustering-4 (SNC-4). These options can be selected during your Cobalt job submission using the --attrs option. Table 1 lists the clustering mode and the associated Cobalt selection.

##Clustering Modes

| CLUSTERING MODE | COBALT SYNTAX |
|-----------------|--------------|
| All2All |	--attrs=numa=a2a |
| Hemisphere |	--attrs=numa=hemi |
| Quadrant |	--attrs=numa=quad |
| SNC-2 |	--attrs=numa=snc2 |
| SNC-4 |	--attrs=numa=snc4 |
 
#Usage
For the a2a, hemi, and quad modes, no special configuration is needed as all memory is placed into a single NUMA domain. For the SNC-2 and SNC-4 modes, memory is split into two or four NUMA domains and each process must be assigned to a particular NUMA domain. This can be done via libnuma API calls or via the numactl command.

#Flat SNC-4 Example
```
aprun -N 16 -d 1 -cc depth -n 4 numactl -m 4 ./app : -n 4 numactl -m 5 ./app : -n 4 numactl -m 6 ./app : -n 4 numactl -m 7 ./app
```
*Note:* The -m parameter for numactl defines the region from which to allocate memory. The valid memory regions are determined by the mode selected. The All2All, Hemisphere, and Quadrant modes all define a single NUMA domain. The SNC-2 and SNC-4 modes define two or four domains, respectively. 

The memory mode when using flat, split, or equal will define twice the number of memory regions that the clustering mode defined. For example, the flat-quadrant mode would have two domains, 0 (DDR), and 1 (MCDRAM). The flat-snc4 mode has eight domains, 0..3 (DDR), and 4..7 (MCDRAM).

##Memory Mode
The memory mode defines how the KNL MCDRAM (high bandwidth memory) will be addressed. Two possible ways are “cache” (where the MCDRAM is transparent to the user and acts as a last-level cache between the L2 and DDR4 memory) and “flat” (in which the MCDRAM is directly addressable and appears as a separate NUMA domain). The cache and flat modes can be combined in two hybrid modes, where a portion of the MCDRAM is directly addressable and a portion is cache. The possible memory modes are flat, cache, equal, and split. Table 2 lists the memory modes and associated Cobalt syntax.

##Memory Modes
|MEMORY MODE	| FLAT (%) |	CACHE (%) |	COBALT SYNTAX |
|----------------|----------|-------------|---------------|
|Cache |	0	|100|	--attrs=mcdram=cache|
|Equal|	50|	50|	--attrs=mcdram=equal|
|Split|	75|	25|	--attrs=mcdram=split|
|Flat|	100	|0|	--attrs=mcdram=flat|
 
##Usage
There is no special configuration for addressing the MCDRAM in cache mode. The caching model will fetch data from DRAM and store it in MCDRAM as needed. The flat memory mode can be accessed one of two ways. The user can bind the process specifically to the MCDRAM using the “numactl” command or use the libmemkind library to allocate memory from a specific area. These two methods can be used in combination as well.

##Flat Quadrant Example
The application binds to MCDRAM so that all allocations are within the high bandwidth memory. If the program tries to allocate more than the 16 GB available MCDRAM, it will fail with an error:

```
aprun -n 64 -N 64 -d 1 -cc depth numactl -m 1 ./app
```

An alternative is to prefer allocation in MCDRAM, but allow allocations to spill over into DDR if more than 16 GB is allocated:

```
aprun -n 64 -N 64 -d 1 -cc depth numactl -p 1 ./app
```

See the numactl man page for more details.

##Results
Table 3 shows the STREAM Triad memory bandwidth for common configurations.

###STREAM Triad Performance 
|MODE|	SIZE|	TRIAD|
|-----|-----|-------|
|Flat - MCDRAM|	7.5 GB|	485 GB/s|
|Flat - DRAM|	7.5 GB|	88 GB/s|
|Cache|	7.5 GB|	352 GB/s|
|Cache|	120.0 GB|	59 GB/s|

##References
[Intel HotChips Presentation PDF](https://www.alcf.anl.gov/files/HC27.25.710-Knights-Landing-Sodani-Intel.pdf)
[Colfax Research MCDRAM](https://colfaxresearch.com/knl-mcdram/)
[Colfax Research NUMA](https://colfaxresearch.com/knl-numa/)
