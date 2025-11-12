# Polaris Machine Overview
Polaris is a 560-node HPE Apollo 6500 Gen 10+ based system. Each node has a single 2.8 GHz AMD EPYC Milan 7543P 32-core CPU with 512 GB of DDR4 RAM, four NVIDIA A100 GPUs connected via NVLink, a pair of local 1.6TB SSDs in RAID0 for user use, and a pair of Slingshot 11 network adapters. There are two nodes per chassis, seven chassis per rack, and 40 racks for a total of 560 nodes. More detailed specifications are as follows:

## Polaris Compute Nodes
| POLARIS COMPUTE | DESCRIPTION | PER NODE | AGGREGATE |
|-----------------|-------------|----------|-----------|
| Processor (Note 1) | 2.8 GHz 7543P | 1 | 560 |
| Cores/Threads | AMD Zen 3 (Milan) | 32/64 | 17,920/35,840 |
| RAM (Note 2) | DDR4 | 512 GiB | 280 TiB |
| GPUs | NVIDIA A100 | 4 | 2,240 |
| Local SSD | 1.6 TB | 2/3.2 TB | 1,120/1.8 PB |

- _Note 1_: 256 MB shared L3 cache, 512 KB L2 cache per core, 32 KB L1 cache per core
- _Note 2_: 8 memory channels rated at 204.8 GiB/s

## Polaris A100 GPU Information
| DESCRIPTION | A100 PCIe | A100 HGX (Polaris) |
|-------------|-----------|--------------------|
| GPU Memory | 40 GiB HBM2 | 160 GiB HBM2 |
| GPU Memory BW | 1.6 TB/s | 6.4 TB/s |
| Interconnect | PCIe Gen4 64 GB/s | NVLink 600 GB/s |
| FP 64 | 9.7 TF | 38.8 TF |
| FP64 Tensor Core | 19.5 TF | 78 TF |
| FP 32 | 19.5 TF | 78 TF |
| BF16 Tensor Core | 312 TF | 1.3 PF |
| FP16 Tensor Core | 312 TF | 1.3 PF |
| INT8 Tensor Core | 624 TOPS | 2,496 TOPS |
| Max TDP Power | 250 W | 400 W |

## Polaris Device Affinity Information
| CPU Affinity | NUMA Affinity |        | GPU0 | GPU1 | GPU2 | GPU3 | mlx5_0 | mlx5_1 |
|--------------|---------------|--------|------|------|------|------|--------|--------|
| 24-31,56-63  | 3             | GPU0   | X    | NV4  | NV4  | NV4  | SYS    | SYS    |
| 16-23,48-55  | 2             | GPU1   | NV4  | X    | NV4  | NV4  | SYS    | PHB    |
| 8-15,40-47   | 1             | GPU2   | NV4  | NV4  | X    | NV4  | SYS    | SYS    |
| 0-7,32-39    | 0             | GPU3   | NV4  | NV4  | NV4  | X    | PHB    | SYS    |
|              |               | mlx5_0 | SYS  | SYS  | SYS  | PHB  | X      | SYS    |
|              |               | mlx5_1 | SYS  | PHB  | SYS  | SYS  | SYS    | X      |

!!! info "Legend"

    - **X** = Self
    - **SYS** = Connection traversing PCIe as well as the SMP interconnect between NUMA nodes (e.g., QPI/UPI)
    - **NODE** = Connection traversing PCIe as well as the interconnect between PCIe Host Bridges within a NUMA node
    - **PHB** = Connection traversing PCIe as well as a PCIe Host Bridge (typically the CPU)
    - **PXB** = Connection traversing multiple PCIe bridges (without traversing the PCIe Host Bridge)
    - **PIX** = Connection traversing at most a single PCIe bridge
    - **NV#** = Connection traversing a bonded set of # NVLinks

Links to detailed NVIDIA A100 documentation:

- [NVIDIA A100 Tensor Core GPU Architecture](https://images.nvidia.com/aem-dam/en-zz/Solutions/data-center/nvidia-ampere-architecture-whitepaper.pdf)
- [NVIDIA Ampere Architecture In-Depth](https://developer.nvidia.com/blog/nvidia-ampere-architecture-in-depth/)

## Login Nodes
There are four login nodes available to users for editing code, building code, submitting/monitoring jobs, checking usage (`sbank`), etc. Their full hostnames are `polaris-login-N.hsn.cm.polaris.alcf.anl.gov` for `N` equal to `01` through `04`; there are an additional two login nodes that are not user-accessible, which are used for running services such as JupyterHub. The various compilers and libraries are present on the logins, so most users should be able to build their code. However, if your build requires the physical presence of the GPU, you will need to build on a compute node.

All users share the same login nodes, so please be courteous and respectful of your fellow users. For example, please do not run computationally or I/O intensive pre- or post-processing on the logins and keep the parallelism of your builds to a reasonable level.

| POLARIS LOGIN | DESCRIPTION | PER NODE | AGGREGATE |
|---------------|-------------|----------|-----------|
| Processor (Note 1) | 2.0 GHz 7713 | 2 | 12 |
| Cores/Threads | AMD Zen 3 (Milan) | 128/256 | 768/1,536 |
| RAM (Note 2) | DDR4 | 512 GiB | 3 TiB |
| GPUs (Note 3) | No GPUs | 0 | 0 |
| Local SSD | None | 0 | 0 |

- _Note 1_: 256 MB shared L3 cache, 512 KB L2 cache per core, 32 KB L1 cache per core
- _Note 2_: 8 memory channels rated at 204.8 GiB/s per socket
- _Note 3_: If your build requires the physical presence of a GPU, you will need to build on a compute node.

## Gateway Nodes
There are 50 gateway nodes. These nodes are not user-accessible but are used transparently for access to the storage systems. Each node has a single 200 Gbps HDR IB card for access to the storage area network. This gives a theoretical peak bandwidth of 1,250 GB/s, which is approximately the aggregate bandwidth of the global file systems (1,300 GB/s).

## Storage

Polaris has access to the ALCF global file systems. Details on storage can be found [here](../data-management/filesystem-and-storage/index.md).
