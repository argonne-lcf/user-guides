# ThetaGPU


## Machine Overview

ThetaGPU is an extension of Theta and is comprised of 24 NVIDIA DGX A100 nodes. Each DGX A100 node comprises eight NVIDIA A100 Tensor Core GPUs and two AMD Rome CPUs that provide 320 gigabytes (7680 GB aggregately) of GPU memory for training artificial intelligence (AI) datasets, while also enabling GPU-specific and -enhanced high-performance computing (HPC) applications for modeling and simulation.

The DGX A100’s integration into Theta is achieved via the ALCF’s Cobalt HPC scheduler and shared access to a 10-petabyte Lustre filesystem. Fixed ALCF user accounts ensure a smooth onboarding process for the expanded system.

A 15-terabyte solid-state drive offers up to 25 gigabits per second in bandwidth. The dedicated compute fabric comprises 20 Mellanox QM9700 HDR200 40-port switches wired in a fat-tree topology. ThetaGPU cannot utilize the Aries interconnect.

## Login Nodes

The [Theta login nodes](https://www.alcf.anl.gov/support-center/theta/theta-thetagpu-overview) will be the intended method to access ThetaGPU.

At first, Cobalt jobs cannot be submitted from the theta login nodes to run on the GPU nodes; until that is supported, users will need to login to the ThetaGPU service nodes (`ssh thetagpusn1` or `ssh thetagpusn2`) from the Theta login nodes, from there, Cobalt jobs can be submitted to run on the GPU nodes.

**Table 1: Theta Machine Overview**

| ThetaGPU Machine Specs    |                   |
|---------------------------|-------------------|
| Architecture              | NVIDIA DGX A100   |
| Speed                     | 3.9 petaflops     |
| Processors                | AMD EPYC 7742     |
| Nodes                     | 24                |
| DDR4 Memory               | 24 TB             |
| GPU Memory                | 7,680 GB          |
| Racks                     | 7                 |


**Table 2: ThetaGPU Compute Nodes Overview**

| COMPONENT                | PER NODE | AGGREGATE |
|--------------------------|----------|-----------|
| AMD Rome 64-core CPU     | 2        | 48        |
| GPU Memory               | 320 GB   | 7,680 GB  |
| DDR4 Memory              | 1 TB     | 24 TB     |
| NVIDIA A100 GPU          | 8        | 192       |
| HDR200 Compute Ports     | 8        | 192       |
| HDR200 Storage Ports     | 2        | 48        |
| 100GbE Ports             | 2        | 48        |
| 3.84 TB Gen4 NVME drives | 4        | 96        |







