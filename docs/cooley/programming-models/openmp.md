# OpenMp on Cooley
## Overview
The OpenMP API is an open standard for parallel programming. The specification document can be found here: https://www.openmp.org/. The specification describes directives, runtime routines, and environment variables that allow an application developer to express parallelism (e.g. shared memory multiprocessing and device offloading). Many compiler vendors provide implementations of the OpenMP specification (https://www.openmp.org/specifications/).

## Using OpenMP at ALCF
OpenMP support for CPUs on Cooley is provided through the GNU, Intel, and LLVM Clang compilers available on Cooley. Guidance on updating your environment to use one of these compilers is available [here](/docs/cooley/compiling-and-linking/compiling-and-linking.md).

OpenMP offload support for GPUs on Cooley is provided via community compilers. The LLVM Clang compiler is installed on Cooley to support OpenMP 4.5+ offload features. The compiler is in rapid development and ALCF staff build it frequently from the master branch. 

The status of offload features in this compiler is available on the [LLVM Clang website](https://clang.llvm.org/docs/OpenMPSupport.html). Considering that the compiler is under active development, the compiler may contain bugs and those should be reported directly to the compiler team [here](https://bugs.llvm.org/).

## Building on Cooley
OpenMP parallelism for CPUs can be enabled for each supported compiler using the appropriate compiler flag: -fopenmp for GNU/Clang and -qopenmp for Intel compilers.
```
soft add +intel-composer-xe-2018

icpc -qopenmp main.cpp
```

OpenMP settings, such as number of threads and affinity, can be controlled via OpenMP environment variables.

The offload compiler is installed on Cooley at /soft/compilers/clang-ykt. To use this compiler, first update your software environment with the following CUDA and gcc softkeys and paths.
```
soft add +cuda-10.0
soft add +gcc-6.4.0
export PATH=/soft/compilers/clang-ykt/latest/bin:$PATH
export LD_LIBRARY_PATH=/soft/compilers/clang-ykt/latest/lib:$LD_LIBRARY_PATH
```

The following compiler flags are needed to enable offload compilation: ```-fopenmp -fopenmp-targets=nvptx64-nvidia-cuda```.
```
clang++ -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda main.cpp
```

## Running jobs on Cooley
An example ‘test.sh’ job submission script follows.
```
#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 1))
mpirun -f $COBALT_NODEFILE -n $PROCS ./a.out
```

To request a single node with 10 minutes of walltime, charging to the MyProject project, one can use the following command.
```
qsub -n 1 -t 10 -A MyProject ./test.sh
```

## Examples
There are a handful of simple examples available in the /soft/compilers/clang-ykt/example directory. To run an example, copy the source file to current working directory, compile, and submit to a compute node in an interactive job or as batch job using example script above.
```
cp /soft/compilers/clang-ykt/example/test_simple2.cpp ./
clang++ -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda test_simple2.cpp

./a.out
enter constructor 0x7ffe3f6bb648
host pointer 0x2125590
device pointer 0x620b840200
Running target region on device!
maptest constructor
check_size = 6
check_value = 1
```

The NVIDIA tools can be used to debug and profile offloaded kernels compiled with the OpenMP offload clang-ykt compiler. For example, nvprof can be used to profile and verify that your application offloaded kernels to the GPUs on Cooley.
```
nvprof ./a.out 
==2755== NVPROF is profiling process 2755, command: ./a.out
enter constructor 0x7ffea1bb91c8
host pointer 0x3fcc7a0
device pointer 0x620c240200
Running target region on device!
maptest constructor
check_size = 6
check_value = 1
==2755== Profiling application: ./a.out
==2755== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   48.51%  156.96us         1  156.96us  156.96us  156.96us  __omp_offloading_2d_e6fe0d__ZN7maptestIdEC1Em_l18
                   46.44%  150.27us         1  150.27us  150.27us  150.27us  __omp_offloading_2d_e6fe0d__ZN7maptestIdE3runEv_l51
                    3.56%  11.520us         5  2.3040us  1.9840us  2.7520us  [CUDA memcpy DtoH]
                    1.49%  4.8310us         3  1.6100us  1.3110us  2.2080us  [CUDA memcpy HtoD]
      API calls:   75.94%  292.38ms         1  292.38ms  292.38ms  292.38ms  cuCtxCreate
                   21.48%  82.678ms         1  82.678ms  82.678ms  82.678ms  cuCtxDestroy
                    0.98%  3.7905ms       256  14.806us  1.3690us  458.24us  cuStreamCreate
                    0.69%  2.6478ms         1  2.6478ms  2.6478ms  2.6478ms  cuModuleLoadDataEx
                    0.39%  1.4933ms         1  1.4933ms  1.4933ms  1.4933ms  cuModuleUnload
                    0.17%  638.23us       256  2.4930us  2.0450us  23.244us  cuStreamDestroy
…
```






