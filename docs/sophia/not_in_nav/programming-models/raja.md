# RAJA ThetaGPU
## Overview
RAJA is a collection of C++ software abstractions, being developed at Lawrence Livermore National Laboratory (LLNL), that enable architecture portability for HPC applications. The overarching goals of RAJA are to:
- Make existing (production) applications portable with minimal disruption
- Provide a model for new applications so that they are portable from inception.

RAJA targets portable, parallel loop execution by providing building blocks that extend the generally-accepted parallel for idiom.

Additional information can be found at [RAJA User Guide](https://raja.readthedocs.io/en/develop/sphinx/user_guide/index.html).

## Using RAJA
RAJA provides a project template for how to use RAJA in an application project that uses CMake or Make. This is located at [RAJA Project Template](https://github.com/LLNL/RAJA-project-template).

### How to get the source code
The RAJA source code lives at [RAJA github](https://github.com/LLNL/RAJA). 

It can be cloned with git clone ```--recursive https://github.com/llnl/raja.git```. The recursive clone will also clone RAJA's dependencies in the proper locations.

### Add a cmake configuration for ThetaGPU
```
raja/host-configs/alcf-builds/thetagpu.cmake
```

```
set(RAJA_COMPILER "RAJA_COMPILER_GNU" CACHE STRING "")

set(CMAKE_CXX_COMPILER "g++" CACHE PATH "")
set(CMAKE_C_COMPILER "gcc" CACHE PATH "")

set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "")

set(CUDA_COMMON_OPT_FLAGS -restrict; --gpu-architecture sm_80; -std c++11; --expt-extended-lambda)
set(CUDA_COMMON_DEBUG_FLAGS -restrict; --gpu-architecture sm_80; -std c++11; --expt-extended-lambda)

set(HOST_OPT_FLAGS -Xcompiler -O3 -Xcompiler -fopenmp)

if(CMAKE_BUILD_TYPE MATCHES Release)
  set(RAJA_NVCC_FLAGS -O3; ${CUDA_COMMON_OPT_FLAGS}; -ccbin; ${CMAKE_CXX_COMPILER} ; ${HOST_OPT_FLAGS} CACHE LIST "")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(RAJA_NVCC_FLAGS -g; -G; -O3; ${CUDA_COMMON_OPT_FLAGS}; -ccbin; ${CMAKE_CXX_COMPILER} ; ${HOST_OPT_FLAGS} CACHE LIST "")
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  set(RAJA_NVCC_FLAGS -g; -G; -O0; ${CUDA_COMMON_DEBUG_FLAGS}; -ccbin; ${CMAKE_CXX_COMPILER} ; -Xcompiler -fopenmp CACHE LIST "")
endif()

set(RAJA_RANGE_ALIGN 4 CACHE INT "")
set(RAJA_RANGE_MIN_LENGTH 32 CACHE INT "")
set(RAJA_DATA_ALIGN 64 CACHE INT "")

set(RAJA_HOST_CONFIG_LOADED On CACHE Bool "")
```

### Now build on the ThetaGPU compute node

```
git clone --recursive https://github.com/llnl/raja.git

mkdir build_alcf-thetagpu && cd build_alcf-thetagpu
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -C ../host-configs/alcf-builds/thetagpu.cmake \
  -DENABLE_OPENMP=On \
  -DENABLE_CUDA=On \
  -DCUDA_ARCH=sm_80 \
  -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda \
  -DCMAKE_INSTALL_PREFIX=../install_${BUILD_SUFFIX} \
  "$@" \

  ..
```
