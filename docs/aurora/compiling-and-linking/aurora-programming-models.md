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

# Aurora Programming Environment

## Overview

The Aurora Programming Environment (Aurora PE) is Aurora's default software environment and consists of the OneAPI SDK, MPICH, and the Spack PE. The Aurora PE is loaded in the user environment through a default module set. Alternative versions of the Aurora PE, as well as Aurora PE components not loaded by default, are available through the module interface. Each version of the PE may have multiple compilers, runtimes, and/or MPICH installations; these are interchangeable within a particular PE version. In addition to the OneAPI and MPICH installations, the Aurora PE contains the Spack PE, which contains a myriad of general and scientific computing software. See the [Spack PE page](./applications-and-libraries/libraries/spack-pe.md) for more details.

## Switching PE or SDK versions

Aurora PE and OneAPI SDK versions can be viewed with `module avail` and switched with `module load` commands. The Aurora PE modules utilize Lmod's hierarchical module system to allow seamless switching between versions of the Aurora PE and its components. For example, if the user loads a different version of the `oneapi` SDK, the other modules in the Aurora PE, such as `intel_compute_runtime`, `mpich`, and `spack-pe-gcc`, will be reloaded to guarantee compatibility. In short, the hierarchical modulefile system ensures that the module environment is self-consistent with minimal user input.

## Aurora Programming Environment and /soft

The Aurora PE is installed in `/opt/aurora` and is mounted as a read-only squashfs. `/soft` is also available to provide software not present in the Aurora PE. Modules in `/soft/modulefiles` are not part of the default environment in order to avoid filesystem metadata overhead, which can have significant performance impacts. Users wishing to use software in `/soft` will need to first run `module use /soft/modulefiles` to access the modules.

The Aurora PE has a longer-term upgrade cadence (on the order of months), so ad-hoc software requests will be fulfilled through software installations in `/soft`. Pre-release previews of Aurora PE components may also be made available in `/soft` for testing. `/soft` installations will be considered for incorporation into the Aurora PE during upgrade cycles.

Modules in `/soft` may conflict with modules in the Aurora PE. Lmod has some limitations in automatically handling module conflicts, so the user may need to manually resolve conflicts arising from modules outside of the Aurora PE. Users are also advised to sanitize module paths that they add from other non-standard locations, such as `/home`, to avoid conflicts. Additionally, packages targeted for future Aurora PE updates are staged for testing under a separate module path  `/soft/preview-modules/<PE_VERSION>` to prevent accidental module clashes.

[//]: # (Do we need a "Mapping Programming Models" table? It's just oneAPI compilers today...)



