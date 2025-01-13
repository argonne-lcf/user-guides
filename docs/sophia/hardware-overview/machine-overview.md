# Sophia Machine Overview

Sophia is comprised of 24 NVIDIA DGX A100 nodes. Each DGX A100 node comprises eight NVIDIA A100 Tensor Core GPUs and two AMD Rome CPUs that provide 22 nodes with 320 GB of GPU memory and two nodes with 640 GB of GPU memory (8,320 GB in total) for training artificial intelligence (AI) datasets, while also enabling GPU-specific and -enhanced high-performance computing (HPC) applications for modeling and simulation.

A 15-terabyte solid-state drive offers up to 25 gigabits per second in bandwidth. The dedicated compute fabric comprises 20 Mellanox QM9700 HDR200 40-port switches wired in a fat-tree topology.

Table 1 summarizes the capabilities of a Sophia compute node:

| COMPONENT | COUNT PER NODE | AGGREGATE |
| --- | ----------- | ----------- |
| AMD Rome 64-core CPU | 2 | 48 |
| DDR4 Memory | 1 TB on 320 GB nodes & 2 TB on 640 GB nodes | 26 TB |
| NVIDIA A100 GPU | 8 | 192 |
| GPU Memory | 22 nodes w/ 320 GB & 2 nodes w/ 640 GB | 8,320 GB |
| HDR200 Compute Ports | 8 | 192 |
| HDR200 Storage Ports | 2 | 48 |
| 100GbE Ports | 2 | 48 |
| 3.84 TB Gen4 NVME drives | 4 | 96 |

---