# Single node "GPU-Peak" benchmarks

> This work was done on a pre-production supercomputer with early versions of the Aurora software development kit

This page aims to give you a high-level overview of key performance numbers for a single Aurora node.

- We are providing both 1 Tile and Full Node numbers.
  - The Full Node numbers are the Weak scaling version of the single node one.
  - The Full Node numbers have been achieved by one Rank per Tile, 12 Ranks.
- All benchmarks' source code and launch options [are included](src/) so you can tweak them as needed.
   - We are not exhaustive. Please assume we cherry-picked the correct size to get the best numbers.
   - We will *not* compare the results to some “theoretical” value.  Theoretical values are full of assumptions, and we want to keep this page short.
   - We will *not* compare the results to other hardware. Feel free to do it yourself 🙂
   - To improve reproducibility, only the “best” numbers are reported (e.g., we take the minimum time of repetition step). When doing "real" science, please perform better statistical analysis.
   - The code will use a mixture of OpenMP and SYCL in C++ (sorry, Fortran, Python, and Level Zero lovers).

The asterisk (`*`) means that the data was collected on sunspot with older software stack...

## Micro-benchmarks

|            | One Tile   | Full Node  |
| -----------|------------|------------|
| Double precision Peak    | 17 TFlop/s  | 203 TFlop/s|
| Memory Bandwidth (triad)  | 1 TB/s | 12 TB/s |
| PCIe Unidirectional Bandwidth (H2D)* | 50 GB/s | 173 GB/s |
| PCIe Unidirectional Bandwidth (D2H)* | 49 GB/s | 122 GB/s |
| PCIe Bidirectional Bandwidth  | 76 GB/s | 356 GB/s |
| Tile2Tile Unidirectional Bandwidth* | 296 GB/s | 1766 GB/s |
| Tile2Tile Bidirectional Bandwidth* | 147 GB/s | 689 GB/s |
| GPU2GPU Unidirectional Bandwidth* | 29 GB/s | 174 GB/s |
| GPU2GPU Bidirectional Bandwidth* | 57 GB/s | 316 GB/s |

## GEMM

|          | One Tile   | Full Node   |
| ---------|------------|-------------|
| DGEMM    | 14 TFlop/s  |  173 TFlop/s |
| SGEMM    | 21 TFlop/s  |  257 TFlop/s |
| HGEMM    | 224 TFlop/s | 2094 TFlop/s |
| BF16GEMM | 238 TFlop/s | 2439 TFlop/s |
| TF32GEMM | 98 TFlop/s  | 1204 TFlop/s |
| I8GEMM   | 520 TFlop/s | 4966 TFlop/s |

## FFT

|          | One Tile   | Full Node   |
| ---------|------------|-------------|
| Single-precision C2C 1D* | 2.3 TFlop/s  | 25 TFlop/s |
| Single-precision C2C 2D* | 2.1 TFlop/s | 21 TFlops/s |

Don't hesitate to contact ALCF staff (via email or Slack) for complaints, bug reports, or praise.
