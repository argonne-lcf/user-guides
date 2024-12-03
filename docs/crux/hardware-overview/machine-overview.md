# Crux Machine Overview

Crux is an HPE Cray Ex Liquid Cooled system with peak performance of 1.18 PF comprised of 64 compute blades connected via Slingshot. Each blade has 4 compute nodes for a total of 512 nodes in the system. Each compute node has dual AMD EPYC 7742 64-Core Processors. Each CPU core supports up to two hyperthreads for a total of 256 threads possible per node. Each CPU has 128 GB of DDR4 memory for a total of 256 GB per node. 

# Node Architecture

The output of `numactl --hardware` is very helpful in understanding the connectivity of the CPU cores in each compute node. Each of the CPUs consists of four NUMA domains containing 16 cores and connected directly to 1/4 of the DDR channels. The following information will be useful in understanding how best to affinitize processes to CPUs on each node. For example, users running multiple applications per node will likely want to localize within a set of NUMA domains in a CPU.

For CPU 0:

* NUMA 0 : cores 0-15,128-143
* NUMA 1 : cores 16-31,144-159
* NUMA 2 : cores 32-47,160-175
* NUMA 3 : cores 48-63,176-191

For CPU 1:

* NUMA 4 : cores 64-79,192-207
* NUMA 5 : cores 80-95,208-223
* NUMA 6 : cores 96-111,224-239
* NUMA 7 : cores 112-127,240-255

