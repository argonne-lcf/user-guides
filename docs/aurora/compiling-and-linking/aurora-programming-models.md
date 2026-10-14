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
| SYCL | --intel -fsycl -fsycl-targets=spir64 | --intel -fsycl -fsycl-targets=intel_gpu_pvc |

/// tip
`--intel` is only needed if the base compiler isn't `icpx` or `icx`
///

For SYCL, `-fsycl-targets=intel_gpu_pvc` is the recommended spelling of the AoT target. It is equivalent to `-fsycl-targets=spir64_gen -Xsycl-target-backend "-device pvc"`, which is still accepted, and it takes no quoted backend argument, so build systems do not need to escape anything. There is no equivalent alias for OpenMP: `-fopenmp-targets` accepts only `spir64_gen` with `-device pvc`.

For some build systems (e.g., `cmake`), it may be necessary to use the backslash character to escape the double quotes when specifying the device in AoT builds.

```bash
-Xopenmp-target-backend=spir64_gen \"-device pvc\"
```

### Reducing link time in AoT SYCL builds

By default the device code in each object file stays as an intermediate form, and GPU code for every kernel is generated during the link. On a large application this makes the link the slowest part of the build, and it is repeated in full whenever any one source file changes. Compiling a source file with `-fno-sycl-rdc` instead generates its GPU code at that point and stores it in the `.o`, which moves that work off the link and into the per-file compiles, where `make -j` can spread it across cores.

The option applies per source file, so an application can mix the two. Files that use `SYCL_EXTERNAL` must be compiled without it; the rest can use it, and all of the objects link together:

```makefile
SELF = $(patsubst %.cpp,%.o,$(wildcard self*.cpp))
RDC  = lib.o uses_lib.o main.o

$(SELF): %.o: %.cpp
	icpx -fsycl -fno-sycl-rdc -fsycl-targets=intel_gpu_pvc -O2 -c $< -o $@
$(RDC):  %.o: %.cpp
	icpx -fsycl -fsycl-targets=intel_gpu_pvc -O2 -c $< -o $@

app: $(SELF) $(RDC)
	icpx -fsycl -fsycl-targets=intel_gpu_pvc $^ -o $@
```

How much this helps depends on how many files can avoid `SYCL_EXTERNAL`, so measure it on the application rather than assuming. For a small test of eleven files, eight of which were self-contained, the split cut link time from 2.7 s to 0.6 s and rebuild time after editing one self-contained file from 5.7 s to 4.2 s. The serial clean build was slightly slower, because each `-fno-sycl-rdc` compile does more work, but the parallel clean build was faster.

See [SYCL on Aurora](../programming-models/sycl-aurora.md#ahead-of-time-compilation) for the caveats, including why `-fsycl-targets=` has to stay on the link command.

OpenCL is supported, but does not require specific compiler flags per-se as the offloaded kernels are JIT-compiled. One does need to link against the OpenCL library `-lOpenCL`. Abstraction programming models, such as Kokkos, can be built on top of these programming models.

[//]: # (Do we need a "Mapping Programming Models" table? It's just oneAPI compilers today...)



