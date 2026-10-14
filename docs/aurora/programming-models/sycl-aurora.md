---
tags:
  - SYCL
---

# SYCL on Aurora

## Overview

SYCL is an open, royalty-free, cross-platform abstraction layer that enables code for heterogeneous and offload processors to be written using modern ISO C++. It provides APIs and abstractions to find devices (CPUs, GPUs, FPGAs, etc.) on which code can be executed and to manage data resources and code execution on those devices.

The specification can be found here: [SYCL 2020 Specification](https://registry.khronos.org/SYCL/specs/sycl-2020/)

## Setting the environment to use SYCL on Aurora

The Intel oneAPI Programming Environment is the main environment on Aurora. oneAPI has SYCL support. The oneAPI module is loaded by default in your environment:

```bash
$ module list

Currently Loaded Modules:
  1) gcc-runtime/12.2.0-267awrk   5) gcc/12.2.0                             9) libfabric/1.20.1
  2) gmp/6.2.1-yctcuid            6) intel_compute_runtime/release/996.26  10) cray-pals/1.4.0
  3) mpfr/4.2.1-fhgnwe7           7) oneapi/eng-compiler/2024.07.30.002    11) cray-libpals/1.4.0
  4) mpc/1.3.1-ygprpb4            8) mpich/icc-all-pmix-gpu/20240717
```

## Building on Aurora

Simply use `-fsycl`. For CMake, use `find_package(IntelSYCL REQUIRED)`. See `cat $CMPLR_ROOT/lib/cmake/IntelSYCL/IntelSYCLConfig.cmake` for more details.

### Ahead-of-time compilation

With `-fsycl` alone, kernels ship as SPIR-V and are lower to the PVC ISA the first time the application runs. To compile them for Aurora's PVC GPUs at build time instead, one can use `-fsycl-targets=intel_gpu_pvc`:

```bash
icpx -fsycl -fsycl-targets=intel_gpu_pvc hello_sycl.cpp -o hello_sycl
```

`-fsycl-targets=intel_gpu_pvc` is the recommended option (it is equivalent to the older `-fsycl-targets=spir64_gen -Xsycl-target-backend "-device pvc"`). 

/// tip
When compiling and linking in separate steps, pass `-fsycl-targets=intel_gpu_pvc` to both. Passing it only to the link step still produces a working executable, but it will contain no GPU code: the kernels stay as SPIR-V and are compiled at first run, which is what AoT is meant to avoid.

Note that the backend argument of the older spelling, `-Xsycl-target-backend "-device pvc"`, is link-only; the compiler reports it as unused at compile time.
///

### Reducing link time with `-fno-sycl-rdc`

By default in an AoT build, device code in each object file is kept in a relocatable form, and GPU machine code for every kernel is generated during the link step. Because this step is serial and reruns completely whenever any single source file changes, it can easily dominate build times in large applications.

Compiling with `-fno-sycl-rdc` shifts code generation from the link step into the per-file compilation step, allowing parallelization of the GPU-code gen:

```bash
icpx -fsycl -fno-sycl-rdc -fsycl-targets=intel_gpu_pvc -O2 -c tu_a.cpp -o tu_a.o   # generates GPU code here
icpx -fsycl -fno-sycl-rdc -fsycl-targets=intel_gpu_pvc -O2 -c tu_b.cpp -o tu_b.o   # and here, in parallel
icpx -fsycl -fsycl-targets=intel_gpu_pvc tu_a.o tu_b.o -o app   # fast: no GPU code left to generate
```

/// warning | Caveats
- **`SYCL_EXTERNAL` is unsupported:** every kernel in a file compiled with `-fno-sycl-rdc` must be self-contained. Compilation will fail with `unknown type name 'SYCL_EXTERNAL'`.
- **Always retain `-fsycl-targets=` at link:** omitting the target flag during the link step is only safe if every object was built with `-fno-sycl-rdc`. If any object still relies on RDC, the link will succeed without warnings, but that object's kernels will be silently omitted from the final binary and fail when launched.
-  while `-fno-sycl-rdc` is accepted in JIT builds, it has no effect on the resulting object files; use it only with AoT.
///


Projects can mix both styles: files requiring cross-file device calls (`SYCL_EXTERNAL`) keep the default RDC mode, while self-contained files use `-fno-sycl-rdc`.


### Checking a binary or object for SPIR-V or native code

Inspect its offload targets with `clang-offload-bundler`:

```bash
clang-offload-bundler --type=o --input=tu_a.o --list
```

- `sycl-spir64_gen_image-...`: contains finished PVC device code (built with `-fno-sycl-rdc`).
- `sycl-spir64_gen-...`: contains relocatable device code; final GPU code will be generated at link time.
- `sycl-spir64-...`: no AoT target was given at compile time, so the object holds generic SPIR-V and its kernels will be compiled at first run.

The same command works on the final executable, where it reports which kinds of device code the binary carries:

- `sycl-spir64_gen` alone: every kernel is PVC device code, as intended for an AoT build.
- `sycl-spir64` alone: a JIT build.
- both: part of the application is still SPIR-V and will be compiled at first run, which usually means an AoT target was missing from a compile step.

## Example

```cpp
$ cat hello_sycl.cpp
#include <sycl/sycl.hpp>
int main(int argc, char **argv) {
  int global_range = 10;
  // Default Queue
  sycl::queue Q;
  // Queue introspection
  std::cout << "Running on " << Q.get_device().get_info<sycl::info::device::name>() << std::endl;

  // Allocate device memory
  int *A = sycl::malloc_device<int>(global_range, Q);
  // Blocking kernel that uses the memory
  Q.parallel_for(global_range, [=](auto id) { A[id] = id; }).wait();
  // Allocate Host Memory
  std::vector<int> A_host(global_range);
  // Blocking copy the device memory to the host
  Q.copy(A, A_host.data(), global_range).wait();
  // Free Device Memory
  sycl::free(A, Q);

  for (size_t i = 0; i < global_range; i++)
    std::cout << "A_host[ " << i << " ] = " << A_host[i] << std::endl;
  return 0;
}
$ icpx -fsycl hello_sycl.cpp
$ ./a.out
```

More examples can be found here: [SYCL Training Examples](https://github.com/argonne-lcf/sycltrain/tree/master/9_sycl_of_hell)
