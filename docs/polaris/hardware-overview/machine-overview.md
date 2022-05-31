# Polaris
Polaris is a 560 node HPE Apollo 6500 Gen 10+ based system.  Each node has a single 2.8 Ghz AMD EPYC Milan 7543P 32 core CPU with 512 GB of DDR4 RAM and four Nvidia A100 GPUs, a pair of local 1.6TB of SSDs in RAID0 for the users use, and a pair of slingshot network adapters.  They are currently slingshot 10, but are scheduled to be upgraded to slingshot 11 in the fall of 2022.  There are two nodes per chassis, seven chassis per rack, and 40 racks for a total of 560 nodes.  More detailed specifications are as follows:

## Polaris Compute Nodes
| POLARIS COMPUTE | DESCRIPTION | PER NODE | AGGREGATE |
|---------|-------------|----------|-----------|
| Processor (Note 1) | 2.8 GHz 7543P | 1 | 560 |
| Cores/Threads | AMD Zen 3 | 32/64 | 17,920/35,840 |
| RAM (Note 2) | DDR4 | 512 GiB | 280 TiB |
| GPUS | Nvidia A100 | 4 | 2240 |
| Local SSD | 1.6 TB | 2/3.2 TB | 1120/1.8PB |

Note 1: 256MB shared L3 cache, 512KB L2 cache per core, 32 KB L1 cache per core  
Note 2: 8 memory channels rated at 204.8 GiB/s

## Polaris A100 GPU Information
| DESCRIPTION | A100 PCIe | A100 HGX (Polaris) |
|-------------|----------|-----------|
| GPU Memory | 40 GiB HBM2 | 160 GiB HBM2 |
| GPU Memory BW | 1.6 TB/s | 6.4 TB/s |
| Interconnect | PCIe Gen4 64 GB/s | NVLink 600 GB/s |
| FP 64 | 9.7 TF | 38.8 TF |
| FP64 Tensor Core | 19.5 TF | 78 TF |
| FP 32 | 19.5 TF | 78 TF |
| BF16 Tensor Core | 312 TF | 1.3 PF |
| FP16 Tensor Core | 312 TF | 1.3 PF |
| INT8 Tensor Core | 624 TOPS | 2496 TOPS |
| Max TDP Power | 250 W | 400 W |


## Login nodes
There are six login nodes for editing code, building code, submitting / monitoring jobs, checking usage (sbank), etc..  The various compilers and libraries are present on the logins, so most users should be able to build their code.  However, if your build requires the physical presence of the GPU, you will need to build on a compute node.  

All users share the same login nodes so please be courteous and respectful of your fellow users.  For example, please do not run computationally or IO intensive pre- or post-processing on the logins and keep the parallelism of your builds to a reasonable level.

| POLARIS LOGIN | DESCRIPTION | PER NODE | AGGREGATE |
|---------|-------------|----------|-----------|
| Processor (Note 1) | 2.0 GHz 7702 | 2 | 12 |
| Cores/Threads | AMD Zen 3 | 128/256 | 768/1536 |
| RAM (Note 2) | DDR4 | 512 GiB | 3 TiB |
| GPUS (Note 3) |No GPUS | 0 | 0 |
| Local SSD | None | 0 | 0 |

Note 1: 256MB shared L3 cache, 512KB L2 cache per core, 32 KB L1 cache per core  
Note 2: 8 memory channels rated at 204.8 GiB/s per socket  
Note 3: If your build requires the physical presence of a GPU you will need to build on a compute node.


## Gateway nodes
There are 50 gateway nodes.  These nodes are not user accessible, but are used transparently for access to the storage systems.  Each node has a single 200 Gbs HDR IB card for access to the storage area network.  This gives a theoretical peak bandwidth of 1250 GB/s which is approximately the aggregate bandwidth of the global file systems (1300 GB/s).


## Storage
Polaris has access to the ALCF global file systems.  Details can be found [here](../data-management/filesystem-and-storage/polaris-disk-quota.md)
