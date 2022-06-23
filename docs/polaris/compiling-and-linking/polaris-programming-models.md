#Programming Models on Polaris

The software environment on Polaris supports several parallel programming models targeting the CPUs and GPUs.

## CPU Parallel Programming Models

The Cray compiler wrappers `cc`, `CC`, and `ftn` are recommended for MPI applications as they provide the needed include paths and libraries for each programming environment. A summary of available CPU parallel programming models and relevant compiler flags is shown below. Users are encouraged to review the corresponding man pages and documentation.

|Programming Model| GNU | NVIDIA | LLVM |
| --- | --- | --- | --- |
| OpenMP | -fopenmp | -mp | -fopenmp |
| OpenACC | -- | -acc=multicore | -- | 

[//]: # (ToDo: Fill-in and test Cray compiler flags)
[//]: # (ToDo: This assumes we'll have LLVM available on day 1; distinguish from Cray)
[//]: # (ToDo: Need entry for SYCL; hipSYCL & oneAPI)

## GPU Programming Models

A summary of available GPU programming models and relevant compiler flags is shown below for compilers that generate offloadable code. Users are encouraged to review the corresponding man pages and documentation.

|Programming Model| GNU | NVIDIA | LLVM |
| --- | --- | --- | --- |
| CUDA | -- | -cuda [-gpu=cuda8.0,cc11.0] | -- |
| OpenACC | -- | -acc | -- |
| OpenCL* | -- | -- | -- |
| OpenMP | --| -mp=gpu | -fopenmp-targets=nvptx64 |

[//]: # (ToDo: Add links to programming model pages)

OpenCL is supported, but does not require specific compiler flags per-se as the offloaded kernels are just-in-time compiled. Abstraction programming models, such as Kokkos, can be built on top of some of these programming models (see below). 


[//]: # (ToDo: Fill-in and test Cray compiler flags)
[//]: # (ToDo: This assumes we'll have LLVM available on day 1; distinguish from Cray)
[//]: # (ToDo: Need entry for SYCL; hipSYCL & oneAPI)
[//]: # (ToDo: Need entry for HIP; Cray? AMD?)

## Mapping Programming Models to Polaris Modules

The table below offers some suggestions for how to get started setting up your environment on Polaris depending on the programming language and model. Note, mixed C/C++ and Fortran applications should choose the programming environment for the Fortran compiler because of mpi.mod and similar incompatibilities between Fortran-generated files. Several simple examples for different programming models are available in the [ALCF GettingStart repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris).

[//]: # (ToDo: Link broken; finish first pass on Polaris examples)


|Programming Language| GPU Programming Model | Likely used Modules/Compilers | Notes |
| --- | --- | --- | --- |
| C/C++ | CUDA | PrgEnv-nvidia, PrgEnv-gnu, llvm | NVIDIA compilers (nvcc, nvc, nvc++) do GPU code generation |
| C/C++ | HIP | PrgEnv-amd ?? | need to validate |
| C/C++ | Kokkos | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | OpenACC | PrgEnv-nvidia |  |
| C/C++ | OpenCL | PrgEnv-nvidia, PrgEnv-gnu, llvm | JIT GPU code generation |
| C/C++ | OpenMP | PrgEnv-nvidia, llvm, oneAPI |  |
| C/C++ | RAJA | See CUDA | HIP, OpenMP, and SYCL/DPC++ also candidates |
| C/C++ | SYCL/DPC++ | hipSYCL, oneAPI | NVIDIA compilers (nvcc, nvc, nvc++) do GPU code generation |
| | | |  |
| Fortran | CUDA | PrgEnv-nvidia, PrgEnv-gnu, llvm | NVIDIA compilers (nvfortran) do GPU code generation; should use PrgEnv-gnu if need `gfortran` |
| Fortran | HIP | PrgEnv-amd ?? | need to validate |
| Fortran | OpenACC | PrgEnv-nvidia |  |
| Fortran | OpenCL | PrgEnv-nvidia, PrgEnv-gnu | JIT GPU code generation |
| Fortran | OpenMP | PrgEnv-nvidia, oneAPI |  |

