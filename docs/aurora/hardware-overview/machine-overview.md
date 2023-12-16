# Aurora System Overview

Aurora is a 10,624 node HPE Cray-Ex based system. Each node consists of 2 Intel
Xeon CPU Max Series (codename Sapphire Rapids or SPR) with on-package HBM and
6 Intel Data Center GPU Max Series (codename Ponte Vecchio or PVC). Each Xeon
has 52 physical cores supporting 2 hardware threads per core and 64GB of
HBM. Each CPU has 512 GB of DDR5. The GPUs are connected all-to-all with Intel
XeLink interfaces. Each node has 8 HPE Slingshot-11 NICs, and the system is
connected in a dragonfly topology. The GPUs may send messages directly to the
NIC via PCIe, without the need to copy into CPU memory.

## Aurora Compute Node

| NODE COMPONENT | DESCRIPTION                       | PER NODE | AGGREGATE           |
|----------------|-----------------------------------|----------|---------------------|
| Processor      | ?.? GHz ????                      | 2        | 21,248              |
| Cores/Threads  | Intel Xeon CPU Max Series         | 104/208  | 1,104,896/2,209,792 |
| CPU HBM        | HBM2e                             | 128 GiB  | 1.297 PiB           |
| CPU DRAM       | DDR5                              | 512 GiB  | 10.375 PiB          |
| GPUS           | Intel Data Center Max Series 1550 | 6        | 63,744              |
| GPU HBM        | HBM2e                             | 768 GiB  | 7.781 PiB           |

## Aurora PVC GPU Components

| GPU COMPONENT    | DESCRIPTION                    | COUNT                      | CAPABILITY           |
|------------------|--------------------------------|----------------------------|----------------------|
| Stack            | a.k.a. Tile                    | 2                          |                      |
| Xe Vector Engine | a.k.a. EU (execution unit)     | 512 per Stack (4?? active) | 8 threads, 512b SIMD |
| Xe Matrix Engine | a.k.a  systolic part of EU     | 512 per Stack (4?? active) |                      |
| Register         | 512 bit register               | 128 per thread             |                      |
| Xe Core          | a.k.a. subslice; unit of 8 EUs | 64 per Stack               | 128 per GPU          |
| L1 cache         |                                | 448KB per Xe Core          | 57344KB per GPU      |
| L2 cache         |                                |                            | 98 MB per GPU?       |
| L3 cache         | a.k.a. RAMBO cache             |                            | 416 MB per GPU       |

## Aurora PVC GPU Performance

| DESCRIPTION          | CAPABILITY |
|----------------------|------------|
| DP FLOP/clock        | 15,872     |
| BF16/FP16 FLOP/clock | 508,904    |
| INT8/FP8 FLOP/clock  | 1,015,808  |
| SP TFLOPS            | ?          |
| DP TFLOPS            | ?          |
| BF16/FP16 TFLOPS     | ?          |
| INT8/FP8 TFLOPS      | ?          |
| HBM bandwidth        | 3.28 TB/s  |
| L3 cache             | ??? TB/s   |


