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