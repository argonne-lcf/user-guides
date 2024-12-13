# Kokkos on ThetaGPU

## Overview

Kokkos implements a programming model in C++ for writing performance-portable applications targeting all major HPC platforms. It provides abstractions for both parallel execution of code and data management. Kokkos is designed to target complex node architectures with N-level memory hierarchies and multiple types of execution resources. It can use OpenMP, etc., as a backend programming model. For more information, please visit [Kokkos GitHub](https://github.com/kokkos/kokkos).

The Kokkos shared memory programming model is a C++ library that provides the necessary architecture-specific backends (e.g., OpenMP, CUDA, etc.). To begin with, it is important to note that the Kokkos programming model is usable only in C/C++ codes. Hence, for those with Fortran codes, Kokkos must first be encapsulated within C/C++ functions and called from the main Fortran code.

The purpose of this document is to provide guidance on using Kokkos on ThetaGPU. Please see the following pages for tutorials and more information on Kokkos: [Kokkos GitHub](https://github.com/kokkos/kokkos) and [Kokkos Tutorials](https://github.com/kokkos/kokkos-tutorials).

## Using Kokkos at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Building Kokkos on ThetaGPU

Please follow the steps below to build Kokkos on ThetaGPU.

### Step 1: Clone the repository

```bash
git clone https://github.com/kokkos/kokkos.git 
cd kokkos 
export KOKKOS_PATH="${PWD}"
```

**Note:** The Kokkos Project strives to keep the master branch stable.

### Step 2: Make sure that a relatively new version of CMake is available

```bash
module load cmake/3.14.5
```

### Step 3: Create a build directory

```bash
mkdir build && cd build
```

### Step 4: Generate the Makefile

```bash
cmake ../kokkos \
    -DCMAKE_CXX_COMPILER=CC \
    -DCMAKE_INSTALL_PREFIX=${PWD}/kokkos-install \
    -DKokkos_ENABLE_OPENMP=On \
    -DKokkos_ENABLE_HWLOC=On \
    -DKokkos_HWLOC_DIR=/usr \
    -DHWLOC_LIBRARY=/usr/lib64/libhwloc.so
```

**Note:** The default compiler on Theta should be sufficient to build Kokkos.

### Step 5: Install Kokkos

```bash
make install
```

**Note:** This will end up installing Kokkos in `${KOKKOS_PATH}/build/kokkos-install`.

If you wish to install it in a different directory, change `CMAKE_INSTALL_PREFIX` in step 4.