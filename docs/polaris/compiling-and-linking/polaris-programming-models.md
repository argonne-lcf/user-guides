# Programming Models on Polaris

The software environment on Polaris supports several parallel programming models targeting the CPUs and GPUs.

## CPU Parallel Programming Models

The Cray compiler wrappers `cc`, `CC`, and `ftn` are recommended for MPI applications as they provide the needed include paths and libraries for each programming environment. A summary of available CPU parallel programming models and relevant compiler flags is shown below. Users are encouraged to review the corresponding man pages and documentation.

| Programming Model | GNU | NVHPC | LLVM |
| --- | --- | --- | --- |
| OpenMP | `-fopenmp` | `-mp` | `-fopenmp` |
| OpenACC | -- | `-acc=multicore` | -- |

Higher-level programming models such as [Kokkos](../programming-models/kokkos-polaris.md) and Raja may also be used for CPU programming.

## GPU Programming Models

A summary of available GPU programming models and relevant compiler flags is shown below for compilers that generate offloadable code. Users are encouraged to review the corresponding man pages and documentation.

| Programming Model | GNU | NVHPC | LLVM | ONEAPI |
| --- | --- | --- | --- | --- |
| CUDA | -- | `-cuda [-gpu=cuda8.0,cc11.0]` | -- | -- |
| HIP[^1] | `--gpu-architecture=compute_80 --gpu-code=sm_80` | -- | -- | -- |
| OpenACC | -- | `-acc` | -- | -- |
| OpenCL[^2] | -- | -- | -- | -- |
| OpenMP | -- | `-mp=gpu` | `--offload-arch=sm_80` | -- |
| SYCL | -- | -- | -- | `-fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80` |

- Note, the `llvm` and `oneapi` modules are provided by ALCF, not HPE, in order to complement the compilers provided by the Cray PE on Polaris.
- Higher-level programming models such as [Kokkos](../programming-models/kokkos-polaris.md) and Raja may also be used for GPU programming.
- Abstraction-based performance portability programming models, such as Kokkos, can be built on top of several of these lower-level programming models (see below).

[^1]: Run `module load rocm` to use this. It is only compatible with `PrgEnv-gnu`
[^2]: OpenCL is supported, but does not require specific compiler flags per se, as the offloaded kernels are just-in-time (JIT) compiled. 

## Mapping Programming Models to Polaris Modules

The table below offers some suggestions for how to get started setting up your environment on Polaris depending on the programming language and model. Note, mixed C/C++ and Fortran applications should choose the programming environment for the Fortran compiler because of `mpi.mod` and similar incompatibilities between Fortran-generated files from different compilers. Several simple examples for testing the software environment on Polaris for different programming models are available in the [ALCF GettingStarted repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris).

Note, users are encouraged to use `PrgEnv-nvhpc` instead of `PrgEnv-nvidia` as the latter will soon be deprecated in Cray's PE. They are otherwise identical, pointing to compilers from the same NVIDIA SDK version.

| Programming Language | GPU Programming Model | Likely used Modules/Compilers | Notes |
| --- | --- | --- | --- |
| C/C++ | CUDA | PrgEnv-nvhpc, PrgEnv-gnu, llvm | NVIDIA (nvcc, nvc, nvc++) and clang compilers do GPU code generation |
| C/C++ | HIP | N/A | need to install with support for A100 |
| C/C++ | Kokkos | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | OpenACC | PrgEnv-nvhpc |  |
| C/C++ | OpenCL | PrgEnv-nvhpc, PrgEnv-gnu, llvm | JIT GPU code generation |
| C/C++ | OpenMP | PrgEnv-nvhpc, llvm |  |
| C/C++ | RAJA | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | SYCL/DPC++ | llvm-sycl |  |
| Fortran | CUDA | PrgEnv-nvhpc | NVIDIA compiler (nvfortran) does GPU code generation; `gfortran` can be loaded via `gcc-mixed` |
| Fortran | HIP | N/A | need to install with support for A100 |
| Fortran | OpenACC | PrgEnv-nvhpc |  |
| Fortran | OpenCL | PrgEnv-nvhpc, PrgEnv-gnu | JIT GPU code generation |
| Fortran | OpenMP | PrgEnv-nvhpc |  |
