#Programming Models on Polaris

The software environment on Polaris supports several parallel programming models targeting the CPUs and GPUs.

## CPU Parallel Programming Models

The Cray compiler wrappers `cc`, `CC`, and `ftn` are recommended for MPI applications as they provide the needed include paths and libraries for each programming environment. A summary of available CPU parallel programming models and relevant compiler flags is shown below. Users are encouraged to review the corresponding man pages and documentation.

|Programming Model| GNU | NVHPC | LLVM |
| --- | --- | --- | --- |
| OpenMP | -fopenmp | -mp | -fopenmp |
| OpenACC | -- | -acc=multicore | -- | 

[//]: # (ToDo: Fill-in and test Cray compiler flags)
[//]: # (ToDo: This assumes we'll have LLVM available on day 1; distinguish from Cray)
[//]: # (ToDo: Need entry for SYCL; hipSYCL & oneAPI)

## GPU Programming Models

A summary of available GPU programming models and relevant compiler flags is shown below for compilers that generate offloadable code. Users are encouraged to review the corresponding man pages and documentation.

|Programming Model| GNU | NVHPC | LLVM | LLVM-SYCL |
| --- | --- | --- | --- | --- |
| CUDA | -- | -cuda [-gpu=cuda8.0,cc11.0] | -- | -- |
| HIP* | -- | -- | -- | -- |
| OpenACC | -- | -acc | -- | -- |
| OpenCL* | -- | -- | -- | -- |
| OpenMP | --| -mp=gpu | -fopenmp-targets=nvptx64 | -- |
| SYCL | -- | -- | -- | -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend '--cuda-gpu-arch=sm_80' |

Note, the `llvm` and `llvm-sycl` modules are provided by ALCF to complement the compilers provided by the Cray PE on Polaris.

[//]: # (ToDo: Add links to programming model pages)

OpenCL is supported, but does not require specific compiler flags per-se as the offloaded kernels are just-in-time compiled. Abstraction programming models, such as Kokkos, can be built on top of some of these programming models (see below). 

A HIP compiler supporting the A100 GPUs is still to be installed on Polaris.


[//]: # (ToDo: Fill-in and test Cray compiler flags)
[//]: # (ToDo: This assumes we'll have LLVM available on day 1; distinguish from Cray)
[//]: # (ToDo: Need entry for SYCL; hipSYCL & oneAPI)
[//]: # (ToDo: Need entry for HIP; Cray? AMD?)

## Mapping Programming Models to Polaris Modules

The table below offers some suggestions for how to get started setting up your environment on Polaris depending on the programming language and model. Note, mixed C/C++ and Fortran applications should choose the programming environment for the Fortran compiler because of mpi.mod and similar incompatibilities between Fortran-generated files from different compilers. Several simple examples for testing the software environment on Polaris for different programming models are available in the [ALCF GettingStart repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris).

Note, users are encouraged to use `PrgEnv-nvhpc` instead of `PrgEnv-nvidia` as the latter will soon be deprecated in Cray's PE. They are otherwise identical pointing to compilers from the same NVIDIA SDK version.

|Programming Language| GPU Programming Model | Likely used Modules/Compilers | Notes |
| --- | --- | --- | --- |
| C/C++ | CUDA | PrgEnv-nvhpc, PrgEnv-gnu, llvm | NVIDIA (nvcc, nvc, nvc++) and clang compilers do GPU code generation |
| C/C++ | HIP | N/A | need to install with support for A100 |
| C/C++ | Kokkos | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | OpenACC | PrgEnv-nvhpc |  |
| C/C++ | OpenCL | PrgEnv-nvhpc, PrgEnv-gnu, llvm | JIT GPU code generation |
| C/C++ | OpenMP | PrgEnv-nvhpc, llvm |  |
| C/C++ | RAJA | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | SYCL/DPC++ | llvm-sycl |  |
| | | |  |
| Fortran | CUDA | PrgEnv-nvhpc | NVIDIA compiler (nvfortran) does GPU code generation; `gfortran` can be loaded via `gcc-mixed` |
| Fortran | HIP | N/A | need to install with support for A100 |
| Fortran | OpenACC | PrgEnv-nvhpc |  |
| Fortran | OpenCL | PrgEnv-nvhpc, PrgEnv-gnu | JIT GPU code generation |
| Fortran | OpenMP | PrgEnv-nvhpc |  |

