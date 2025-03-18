# Single node "GPU-Peak" benchmarks

> This work was done on a pre-production supercomputer with early versions of the Aurora software development kit.

This page aims to give you a high-level overview of key performance numbers for a single Aurora node.

- We are providing both 1 Tile and Full Node numbers.
  - The Full Node numbers are the weak scaling version of the single node one.
  - The Full Node numbers have been achieved by one rank per tile, 12 ranks.
- All benchmarks' source code and launch options [are included](https://github.com/argonne-lcf/user-guides/tree/main/docs/aurora/node-performance-overview/src) so you can tweak them as needed.
   - We are not exhaustive. Please assume we cherry-picked the correct size to get the best numbers.
   - We will *not* compare the results to some “theoretical” value. Theoretical values are full of assumptions, and we want to keep this page short.
   - We will *not* compare the results to other hardware. Feel free to do it yourself 🙂
   - To improve reproducibility, only the “best” numbers are reported (e.g., we take the minimum time of repetition step). When doing "real" science, please perform better statistical analysis.
   - The code will use a mixture of OpenMP and SYCL in C++ (sorry, Fortran, Python, and Level Zero lovers).


## Micro-benchmarks

|                                     |   One Tile |   Full Node | Scaling |
|-------------------------------------|-----------:|------------:|--------:|
|         Single Precision Peak Flops | 23 TFlop/s | 267 TFlop/s |    11.8 |
|         Double Precision Peak Flops | 17 TFlop/s | 187 TFlop/s |    10.9 |
|            Memory Bandwidth (triad) |     1 TB/s |     12 TB/s |    11.9 |
| PCIe Unidirectional Bandwidth (H2D) |    54 GB/s |    329 GB/s |     6.1 |
| PCIe Unidirectional Bandwidth (D2H) |    55 GB/s |    263 GB/s |     4.8 |
|        PCIe Bidirectional Bandwidth |    76 GB/s |    357 GB/s |     4.7 |
|  Tile2Tile Unidirectional Bandwidth |   196 GB/s |      1 TB/s |     6.0 |
|   Tile2Tile Bidirectional Bandwidth |   287 GB/s |      2 TB/s |     5.9 |
|    GPU2GPU Unidirectional Bandwidth |    15 GB/s |     95 GB/s |     6.3 |
|     GPU2GPU Bidirectional Bandwidth |    23 GB/s |    142 GB/s |     6.2 |

### Benchmark description

- Double Precision Peak Flops: Chain of FMA.
- Memory Bandwidth (triad): Triad, 2 loads, 1 store.
- PCIe Unidirectional Bandwidth (H2D): Host to Device data transfer.
- PCIe Unidirectional Bandwidth (D2H): Device to Host data transfer.
- PCIe Bidirectional Bandwidth: Concurrent Host to Device and Device to Host data transfer.
- Tile2Tile Unidirectional Bandwidth: MPI Rank 0 (GPU N, Tile 0) will send a GPU buffer to Rank 1 (GPU N, Tile 1).
- Tile2Tile Bidirectional Bandwidth: MPI Rank 0 (GPU N, Tile 0) will send a GPU buffer to Rank 1 (GPU N, Tile 1). Concurrently, Rank 1 will also send a buffer to Rank 0.
- GPU2GPU Unidirectional Bandwidth: MPI Rank 0 (GPU 0, Tile 0) will send a GPU buffer to Rank 1 (GPU 1, Tile 0).
- GPU2GPU Bidirectional Bandwidth: MPI Rank 0 (GPU 0, Tile 0) will send a GPU buffer to Rank 1 (GPU 1, Tile 0). Concurrently, Rank 1 will also send a buffer to Rank 0.

## GEMM

|          |    One Tile |    Full Node | Scaling |
|----------|------------:|-------------:|--------:|
|    DGEMM |  15 TFlop/s |  179 TFlop/s |    11.9 |
|    SGEMM |  22 TFlop/s |  258 TFlop/s |    11.7 |
|    HGEMM | 263 TFlop/s | 2606 TFlop/s |     9.9 |
| BF16GEMM | 273 TFlop/s | 2645 TFlop/s |     9.7 |
| TF32GEMM | 110 TFlop/s | 1311 TFlop/s |    11.9 |
|   I8GEMM | 577 TFlop/s | 5394 TFlop/s |     9.4 |

## FFT

|                             |   One Tile |  Full Node | Scaling |
|-----------------------------|-----------:|-----------:|--------:|
| Single-precision FFT C2C 1D |  3 TFlop/s | 34 TFlop/s |    10.8 |
| Single-precision FFT C2C 2D |  3 TFlop/s | 35 TFlop/s |    10.4 |

Don't hesitate to contact ALCF staff (via email or Slack) for complaints, bug reports, or praise.
