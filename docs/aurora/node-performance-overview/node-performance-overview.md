# Single node "GPU-Peak" benchmarks

> This work was done on a pre-production supercomputer with early versions of the Aurora software development kit.

This page aims to give you a high-level overview of key performance numbers for a single Aurora node.

- We are providing both 1 Tile and Full Node numbers.
- The Full Node numbers are the weak scaling version of the single node one.
- The Full Node numbers have been achieved by one rank per tile, 12 ranks.
- All benchmarks' source code and launch options [are included](https://github.com/argonne-lcf/user-guides/tree/main/docs/aurora/node-performance-overview/src) so you can tweak them as needed.
- The results we provide are not exhaustive and do not paint a complete picture of a node's performance. Please assume we cherry-picked the correct size to get the best numbers.
- We will *not* compare the results to some “theoretical” value. Theoretical values are full of assumptions, and we want to keep this page short.
- We will *not* compare the results to other hardware. Feel free to do it yourself 🙂
- To improve reproducibility, only the “best” numbers are reported (e.g., we take the minimum time of repetition step). When doing "real" science, please perform better statistical analysis.
- The code will use a mixture of OpenMP and SYCL in C++ (sorry, Fortran, Python, and Level Zero lovers).

Don't hesitate to contact ALCF staff (via email or Slack) for complaints, bug reports, or praise for these benchmarks.

!!! note
     These values were measured on Jan. 12th, 2026. The default GPU frequency on Aurora was 1.5 Ghz.

## Micro-benchmarks

|                                     |   One Tile |   Full Node | Scaling |
|-------------------------------------|-----------:|------------:|--------:|
|         Single Precision Peak Flops | 21 TFlop/s | 252 TFlop/s |    11.8 |
|         Double Precision Peak Flops | 11 TFlop/s | 127 TFlop/s |    11.9 |
|            Memory Bandwidth (triad) |     1 TB/s |     12 TB/s |    11.9 |
| PCIe Unidirectional Bandwidth (H2D) |    54 GB/s |    318 GB/s |     5.9 |
| PCIe Unidirectional Bandwidth (D2H) |    55 GB/s |    257 GB/s |     4.7 |
|        PCIe Bidirectional Bandwidth |    76 GB/s |    339 GB/s |     4.5 |
|  Tile2Tile Unidirectional Bandwidth |   185 GB/s |      1 TB/s |     6.0 |
|   Tile2Tile Bidirectional Bandwidth |   269 GB/s |      2 TB/s |     6.0 |
|    GPU2GPU Unidirectional Bandwidth |    15 GB/s |     95 GB/s |     6.3 |
|     GPU2GPU Bidirectional Bandwidth |    23 GB/s |    143 GB/s |     6.2 |

### Benchmark description

- Double Precision Peak Flops: Chain of FMA.  ([`flops.cpp`](./src/flops.cpp))
- Memory Bandwidth (triad): Triad, 2 loads, 1 store ([`triad.cpp`](./src/triad.cpp))
- PCIe Unidirectional Bandwidth (H2D): Host to Device data transfer. ([`pci.cpp`](./src/pci.cpp))
- PCIe Unidirectional Bandwidth (D2H): Device to Host data transfer.
- PCIe Bidirectional Bandwidth: Concurrent Host to Device and Device to Host data transfer.
- Tile2Tile Bidirectional Bandwidth: MPI Rank 0 (GPU N, Tile 0) will send a GPU buffer to Rank 1 (GPU N, Tile 1). Concurrently, Rank 1 will also send a buffer to Rank 0. ([`peer2peer.cpp`](./src/peer2peer.cpp))
- GPU2GPU Unidirectional Bandwidth: MPI Rank 0 (GPU 0, Tile 0) will send a GPU buffer to Rank 1 (GPU 1, Tile 0). ([`peer2peer.cpp`](./src/peer2peer.cpp))
- GPU2GPU Bidirectional Bandwidth: MPI Rank 0 (GPU 0, Tile 0) will send a GPU buffer to Rank 1 (GPU 1, Tile 0). Concurrently, Rank 1 will also send a buffer to Rank 0. ([`peer2peer.cpp`](./src/peer2peer.cpp))

## Tile2Tile unidirectional bandwidth

For this Tile2Tile Unidirectional Bandwidth test, MPI Rank 0 (GPU N, Tile 0) will send a GPU buffer to Rank 1 (GPU N, Tile 1). The bandwidth is higher for writes vs. reads, but the performance is roughly uniform among all possible combinations of tiles (excluding the case of two tiles on the same physical GPU, which has much higher overall bandwidth, with Read bandwidth > Write bandwidth):

```output
Detected 12 GPUs
.......{WRITE} GPU 0 to GPU <*>.......
GPU 0 -> GPU 1: Bandwidth = 158.52 GB/s
GPU 0 -> GPU 2: Bandwidth = 18.8828 GB/s
GPU 0 -> GPU 3: Bandwidth = 18.9062 GB/s
GPU 0 -> GPU 4: Bandwidth = 18.9062 GB/s
GPU 0 -> GPU 5: Bandwidth = 18.8858 GB/s
GPU 0 -> GPU 6: Bandwidth = 18.9068 GB/s
GPU 0 -> GPU 7: Bandwidth = 18.887 GB/s
GPU 0 -> GPU 8: Bandwidth = 18.9071 GB/s
GPU 0 -> GPU 9: Bandwidth = 18.8874 GB/s
GPU 0 -> GPU 10: Bandwidth = 18.8833 GB/s
GPU 0 -> GPU 11: Bandwidth = 18.9067 GB/s
.......{READ} GPU<*> to GPU 0.......
GPU 1 -> GPU 0: Bandwidth = 173.569 GB/s
GPU 2 -> GPU 0: Bandwidth = 15.779 GB/s
GPU 3 -> GPU 0: Bandwidth = 15.7959 GB/s
GPU 4 -> GPU 0: Bandwidth = 15.7963 GB/s
GPU 5 -> GPU 0: Bandwidth = 15.7815 GB/s
GPU 6 -> GPU 0: Bandwidth = 15.7963 GB/s
GPU 7 -> GPU 0: Bandwidth = 15.7821 GB/s
GPU 8 -> GPU 0: Bandwidth = 15.7971 GB/s
GPU 9 -> GPU 0: Bandwidth = 15.7821 GB/s
GPU 10 -> GPU 0: Bandwidth = 15.7795 GB/s
GPU 11 -> GPU 0: Bandwidth = 15.7961 GB/s
```

See the source code for this test: [`mpi_sycl_intranode_bw.cpp`](./src/mpi_sycl_intranode_bw.cpp)

## GEMM

|          |    One Tile |    Full Node | Scaling |
|       ---| -----------:|  -----------:|    ----:|
|    DGEMM |  10 TFlop/s |  125 TFlop/s |    11.9 |
|    SGEMM |  21 TFlop/s |  247 TFlop/s |    12.0 |
|    HGEMM | 247 TFlop/s | 2469 TFlop/s |    10.0 |
| BF16GEMM | 256 TFlop/s | 2487 TFlop/s |     9.7 |
| TF32GEMM | 130 TFlop/s | 1254 TFlop/s |     9.7 |
|   I8GEMM | 529 TFlop/s | 5128 TFlop/s |     9.7 |

See the source code for this test: [`gemm.cpp`](./src/gemm.cpp)

## FFT

|                             |   One Tile |  Full Node | Scaling |
|                          ---|-----------:|-----------:|    ----:|
| Single-precision FFT C2C 1D |  3 TFlop/s | 36 TFlop/s |    11.6 |
| Single-precision FFT C2C 2D |  3 TFlop/s | 35 TFlop/s |    10.8 |

See the source code for this test: [`fftc2c.cpp`](./src/fftc2c.cpp)
