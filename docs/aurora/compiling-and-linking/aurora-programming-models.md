# Aurora Programming Models

The software environment on Aurora supports several parallel programming models targeting the CPUs and GPUs. 

## CPU Parallel Programming Models

The Aurora MPICH compiler wrappers `mpicc`, `mpicxx`, and `mpifort` are recommended for MPI applications to be built using the oneAPI compilers. A summary of available CPU parallel programming models and relevant compiler flags is show below. Users are encouraged to review the corresponding man pages and documentation.

[//]: # (Table too much? There are GNU compilers as well I guess...)

|Programming Model| oneAPI |
| --- | --- |
| OpenMP | -fopenmp |

Higher-level programming models such as Kokkos and Raja may also be used for CPU programming on Aurora.

## GPU Programming Models

A summary of available GPU programming models and relevant compiler flags is shown below for compilers that generate offloadable code. Two modes of compilation are currently available with the oneAPI compilers: Just-in-Time (JIT) and Ahead-of-Time (AoT). With AOT compilation, flags for specifying the backend are only needed when linking the application. Users are encouraged to review the corresponding man pages and documentation.

|Programming Model| oneAPI (JIT) | oneAPI (AoT) |
| --- | --- | --- |
| OpenCL | -- | N/A |
| OpenMP | -fiopenmp -fopenmp-targets=spir64 | -fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend=spir64_gen "-device 12.60.7" |
| SYCL | --intel -fsycl -fsycl-targets=spir64 | --intel -fsycl -fsycl-targets=spir64_gen -Xsycl-target-backend "-device 12.60.7" |

The oneAPI compiler flags For some build systems (e.g. `cmake`), it may be necessary to use the backslash character to escape the double quotes when specifying the device in AoT builds.

```
-Xopenmp-target-backend=spir64_gen \"-device 12.60.7\"
```

OpenCL is supported, but does not require specific compiler flags per-se as the offloaded kernels are JIT-compiled. One does need to link against the OpenCL library `-lOpenCL`. Abstraction programming models, such as Kokkos, can be built on top of these programming models.

[//]: # (Do we need a "Mapping Programming Models" table? It's just oneAPI compilers today...)



