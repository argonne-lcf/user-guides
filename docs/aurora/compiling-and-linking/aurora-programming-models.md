# Aurora Programming Models

The software environment on Aurora supports several parallel programming models targeting the CPUs and GPUs.

## CPU Parallel Programming Models

The Aurora MPICH compiler wrappers `mpicc`, `mpicxx`, and `mpifort` are recommended for MPI applications to be built using the oneAPI compilers. A summary of available CPU parallel programming models and relevant compiler flags is shown below. Users are encouraged to review the corresponding man pages and documentation.

| Programming Model | oneAPI |
| --- | --- |
| OpenMP | -fiopenmp or -qopenmp |

Note: `-fopenmp` uses OpenMP as it is implemented by the LLVM community.

Higher-level programming models such as Kokkos and Raja may also be used for CPU programming on Aurora.

## GPU Programming Models

A summary of available GPU programming models and relevant compiler flags is shown below for compilers that generate offloadable code. Two modes of compilation are currently available with the oneAPI compilers: Just-in-Time (JIT) and Ahead-of-Time (AoT). With AoT compilation, flags for specifying the backend are only needed when linking the application. Users are encouraged to review the corresponding man pages and documentation.

| Programming Model | oneAPI (JIT) | oneAPI (AoT) |
| --- | --- | --- |
| OpenCL | -- | N/A |
| OpenMP | -fiopenmp -fopenmp-targets=spir64 | -fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend=spir64_gen "-device pvc" |
| SYCL | -fsycl -fsycl-targets=spir64 | -fsycl -fsycl-targets=intel_gpu_pvc |

/// tip
One can add `--intel` if the base compiler isn't `icpx` or `icx`
///

For SYCL, `-fsycl-targets=intel_gpu_pvc` is the recommended spelling of the AoT target. It is equivalent to `-fsycl-targets=spir64_gen -Xsycl-target-backend "-device pvc"`.

OpenMP has no equivalent alias. One should use the `-fopenmp-targets=spir64_gen -Xopenmp-target-backend=spir64_gen "-device pvc"` command. Please note that some build systems (e.g., `cmake`) may require escaping the double quotes when specifying the device in AoT builds.

```bash
-Xopenmp-target-backend=spir64_gen \"-device pvc\"
```

/// tip
On a large SYCL application, producing the device image during the link can dominate the build. See [Reducing link time with `-fno-sycl-rdc`](../programming-models/sycl-aurora.md#reducing-link-time-with-fno-sycl-rdc) for moving that work into the per-file compiles, where it can run in parallel.
///

OpenCL is supported, but does not require specific compiler flags per-se as the offloaded kernels are JIT-compiled. One does need to link against the OpenCL library `-lOpenCL`. Abstraction programming models, such as Kokkos, can be built on top of these programming models.

[//]: # (Do we need a "Mapping Programming Models" table? It's just oneAPI compilers today...)



