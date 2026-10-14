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

With `-fsycl` alone, kernels ship as SPIR-V and are compiled for the GPU the first time the application runs. To compile them for Aurora's PVC GPUs at build time instead, and so avoid that startup cost, add `-fsycl-targets=intel_gpu_pvc`:

```bash
icpx -fsycl -fsycl-targets=intel_gpu_pvc hello_sycl.cpp
```

This is the recommended spelling. It is equivalent to the older `-fsycl-targets=spir64_gen -Xsycl-target-backend "-device pvc"`, which still works, but it takes no quoted backend argument, so build systems need no extra escaping. The flag is only needed when linking, so it can be added to a project's link flags alone.

By default the device code in each `.o` stays as an intermediate form, and the GPU code is generated once at link time. Adding `-fno-sycl-rdc` to the compile step makes each `.o` carry finished PVC code, so editing one source file regenerates GPU code for that file only:

```bash
icpx -fsycl -fno-sycl-rdc -fsycl-targets=intel_gpu_pvc -c tu_a.cpp -o tu_a.o
icpx -fsycl -fno-sycl-rdc -fsycl-targets=intel_gpu_pvc -c tu_b.cpp -o tu_b.o
icpx -fsycl -fsycl-targets=intel_gpu_pvc tu_a.o tu_b.o -o app
```

`-fno-sycl-rdc` applies per source file, so a project can mix the two. Files that use `SYCL_EXTERNAL` are compiled without it, the rest with it, and all of the resulting `.o` files link together as usual. To see how an object was built, run `clang-offload-bundler --type=o --input=tu_a.o --list`: a `spir64_gen_image` target means the object already holds GPU code, and `spir64_gen` means it will be generated at link time.

/// warning
`SYCL_EXTERNAL` is not available under `-fno-sycl-rdc`, so every kernel must be self-contained in its source file. The macro is left undefined rather than diagnosed, so a file that uses it fails with `unknown type name 'SYCL_EXTERNAL'`.

Keep `-fsycl-targets=` on the link command, as shown above. Dropping it is only safe when every object was compiled with `-fno-sycl-rdc`; if any was not, the link still succeeds and prints no warning, but that object's kernels are left out of the executable and fail when launched.
///

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