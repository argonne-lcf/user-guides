# Single node "GPU-Peak" benchmarks

> This work was done on a pre-production supercomputer with early versions of the Aurora software development kit

This page's goal is to give you a high-level overview of some key performance numbers of a single node of Aurora. 

- We will provide both 1 Tile and Full Node numbers.
  - The Full Node numbers are the Weak scaling version of the single node one.
  - The Full Node numbers have been achieved by one Rank per Tile, 12 Ranks
- The source code and launch options of all the benchmarks are included so you can tweak them as required
   - We are not exhaustive. Please assume we cherry-picked the correct size to get the best numbers
   -  We will *not* compare the results to some “theoretical” value.  Theoretical values are full of assumptions, and we want to keep this page short.
   -  We will *not* compare the results to other hardware. Feel free to do it yourself 🙂
   - To improve reproducibility, only the “best” numbers are reported (we take the minimum of the time). When doing "real" science, please perform better statistical analysis.
   -  The code will use a mixture of OpenMP and SYCL in C++.  (sorry, Fortran, Python, and Level Zero lovers)

## Micro-benchmark

|            | One Tile   | Full Node  |
| -----------|------------|------------|
| Douple precission Peak    | 17 TFlop/S  | 203 TFlop/S| 
| Memory Bandwidth (triad)  | 1TB/S | 12TB/s | 
| PCIe Bidirectional Bandwitch  | 76GB/s | 356GB/s |

### How to run and compile

```bash
$ mpicxx  -fiopenmp -fopenmp-targets=spir64 foo.cpp
# or
$ mpicxx  -fsycl foo.cpp
# Then
$ mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- gpu_tile_compact.sh ./a.out
$ mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- gpu_tile_compact.sh ./a.out
```

## GEMM

|          | One Tile   | Full Node   |
| ---------|------------|-------------|
| DGEMM    | 14 TFlop/S  |  173 TFlop/S |
| SGEMM    | 21 TFlop/S  |  257 TFlop/S |
| HGEMM    | 224 TFlop/S | 2094 TFlop/S | 
| BF16GEMM | 238 TFlop/S | 2439 TFlop/S |
| TF32GEMM | 98 TFlop/S  | 1204 TFlop/S |
| I8GEMM   | 520 TFlop/S | 4966 TFlop/S |

### How to run and compile

```bash
$ mpicxx  -fsycl -qmkl foo.cpp
$ mpirun -n 1 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- gpu_tile_compact.sh ./a.out
$ mpirun -n 12 --cpu-bind list:1,2,3,4,5,6,52,53,54,55,56,57 -- gpu_tile_compact.sh ./a.out

```

Don't hesitate to contact ALCF staff (via email or Slack) for complaints, bug reports, or praise. 
