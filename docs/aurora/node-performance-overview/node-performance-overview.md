# Single node "GPU-Peak" benchmarks

> This work was done on a pre-production supercomputer with early versions of the Aurora software development kit

This page aims to give you a high-level overview of key performance numbers for a single Aurora node. 

- We are providing both 1 Tile and Full Node numbers.
  - The Full Node numbers are the Weak scaling version of the single node one.
  - The Full Node numbers have been achieved by one Rank per Tile, 12 Ranks.
- All benchmarks' source code and launch options are included so you can tweak them as needed.
   - We are not exhaustive. Please assume we cherry-picked the correct size to get the best numbers.
   - We will *not* compare the results to some “theoretical” value.  Theoretical values are full of assumptions, and we want to keep this page short.
   - We will *not* compare the results to other hardware. Feel free to do it yourself 🙂
   - To improve reproducibility, only the “best” numbers are reported (e.g., we take the minimum time of repetition step). When doing "real" science, please perform better statistical analysis.
   - The code will use a mixture of OpenMP and SYCL in C++ (sorry, Fortran, Python, and Level Zero lovers).

The asterisk (`*`) Means run on sunspot...

## Micro-benchmarks

|            | One Tile   | Full Node  |
| -----------|------------|------------|
| Double precision Peak    | 17 TFlop/S  | 203 TFlop/S|
| Memory Bandwidth (triad)  | 1 TB/S | 12 TB/s |
| PCIe Unidirectional Bandwitch (H2D)* | 50 GB/s | 173 GB/s |
| PCIe Unidirectional Bandwitch (D2H)* | 49 GB/s | 122 GB/s |
| PCIe Bidirectional Bandwitch  | 76 GB/s | 356 GB/s |
| Tile2Tile Unidirectional Bandwitch* | 73 GB/s | 438 GB/s |
| Tile2Tile Bidirectional Bandwitch* | 134 GB/s | 802 GB/s |
| GPU2GPU Unidirectional Bandwitch* | 29 GB/s | 174 GB/s |
| GPU2GPU Bidirectional Bandwitch* | 57 GB/s | 316 GB/s |

## GEMM

|          | One Tile   | Full Node   |
| ---------|------------|-------------|
| DGEMM    | 14 TFlop/S  |  173 TFlop/S |
| SGEMM    | 21 TFlop/S  |  257 TFlop/S |
| HGEMM    | 224 TFlop/S | 2094 TFlop/S | 
| BF16GEMM | 238 TFlop/S | 2439 TFlop/S |
| TF32GEMM | 98 TFlop/S  | 1204 TFlop/S |
| I8GEMM   | 520 TFlop/S | 4966 TFlop/S |

Don't hesitate to contact ALCF staff (via email or Slack) for complaints, bug reports, or praise. 
