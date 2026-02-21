# Level-Zero on Aurora

## Overview

The objective of the ‘[oneAPI](https://oneapi-spec.uxlfoundation.org/)’
Level-Zero Application Programming Interface (API) is to provide
direct-to-metal interfaces to offload accelerator devices. Its programming
interface can be tailored to any device's needs and can be adapted to support a
broader set of language features such as function pointers, virtual functions,
unified memory, and I/O capabilities.

## Using Level-Zero on Aurora

The Intel Compute Runtime is part of the node images on Aurora, and available under
standard system paths (`/usr/include/level_zero/` for example).

## Building on Aurora

Level-Zero is a C API that can be used in your application by including the
`ze_api.h` file:

```c
#include <level_zero/ze_api.h>
```

Applications that use the Level-Zero API need to be linked to the Level-Zero
loader library by using the `-lze_loader` linker flag.

## Level-Zero Documentation

The Level-Zero documentation can be found here:
[Level-Zero Specification](https://oneapi-src.github.io/level-zero-spec).
