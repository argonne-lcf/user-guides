# Level-Zero on Aurora

## Overview

The objective of the ‘[oneAPI](https://oneapi-spec.uxlfoundation.org/)’
Level-Zero Application Programming Interface (API) is to provide
direct-to-metal interfaces to offload accelerator devices. Its programming
interface can be tailored to any device's needs and can be adapted to support a
broader set of language features such as function pointers, virtual functions,
unified memory, and I/O capabilities.

## Setting the environment to use Level-Zero on Aurora

The Intel Programming Environment is the main environment on Aurora. The Intel
Compute Runtime is part of this environment and grants access to Level-Zero.
The Intel Compute Runtime is loaded by default in your environment.

```
> module list

Currently Loaded Modules:
  1) gcc/11.2.0                    3) intel_compute_runtime/release/agama-devel-551   5) libfabric/1.15.2.0   7) cray-libpals/1.3.3
  2) mpich/51.2/icc-all-pmix-gpu   4) oneapi/eng-compiler/2022.12.30.003              6) cray-pals/1.3.3

```

## Building on Aurora

Level-Zero is a C API that can be used in your application by including the
`ze_api.h` file:

```c
#include <ze_api.h>
```

Applications that use the Level-Zero API need to be linked to the Level-Zero
loader library by using the `-lze_loader` linker flag.

## Level-Zero Documentation

The Level-Zero documentation can be found here:
[Level-Zero Specification](https://oneapi-src.github.io/level-zero-spec).