# Overview

Cooley is an accelerated system with Nvidia Kepler GPUs.
OpenMP offload to GPUs is supported via community compilers.

LLVM Clang compiler is installed on Cooley to support OpenMP 4.5+ offload features.
The compiler is in rapid development and we build it frequently from master branch instead of releases.
This page provides the status of offload features https://clang.llvm.org/docs/OpenMPSupport.html.
Considering that a lot of features are newly developed, the compiler may contain bugs and please report to https://bugs.llvm.org.

Disclaimer: this compiler is **NOT** for production.

# Cooley

On Cooley, the offload compiler is installed in two places:
one for mainline LLVM and one for clang-ykt (no longer maintained).

## Mainline LLVM
On Cooley, the offload compiler is installed at
/soft/compilers/llvm/

master-latest is the latest pull from the master branch of LLVM
(https://github.com/llvm/llvm-project).
master-20200414 is a pull from the master branch on 4/14/2020.

```
export PATH=/soft/compilers/llvm/master-latest/bin:$PATH
export LD_LIBRARY_PATH=/soft/compilers/llvm/master-latest/lib:$LD_LIBRARY_PATH
```
OpenMP offload feature needs the following softkey
```
+cuda-10.0
+gcc-7.1.0
```
Remember to resoft

## Clang-ykt
There are also clang-ykt offload versions here:
20190319, 20190320, 20190425 are not built from the mainline llvm+clang+openmp.
Instead this is a develop version from https://github.com/clang-ykt.
It is no more maintained
.
mirror-20200209 is built from mainline llvm-project.
New builds will continue being added in the future.

OpenMP offload feature needs the following softkey
```
+cuda-10.0
+gcc-6.4.0
```
Remember to resoft

Set paths.
```
export PATH=/soft/compilers/clang-ykt/latest/bin:$PATH
export LD_LIBRARY_PATH=/soft/compilers/clang-ykt/latest/lib:$LD_LIBRARY_PATH
```

# Compiling

The following argument is always needed for offload compilation
`-fopenmp -fopenmp-targets=nvptx64-nvidia-cuda`

find test_simple2.cpp in /soft/compilers/clang-ykt/example

`clang++ -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda test_simple2.cpp`

# Running

Using the example above, ./a.out prints on compute nodes (with K80)
"Running target region on device!"
or on login nodes (no GPU)
"Running target region on host!"

Use nvprof to verify your run. Ignore the warning.
```
$ nvprof ./a.out > out
==122598== NVPROF is profiling process 122598, command: ./a.out
==122598== Profiling application: ./a.out
==122598== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   49.48%  190.14us         1  190.14us  190.14us  190.14us  __omp_offloading_2e_20895b0f__ZN7maptestIdEC1Em_l18
                   46.48%  178.62us         1  178.62us  178.62us  178.62us  __omp_offloading_2e_20895b0f__ZN7maptestIdE3runEv_l51
                    2.75%  10.560us         5  2.1120us  1.8560us  2.3360us  [CUDA memcpy DtoH]
                    1.30%  4.9920us         3  1.6640us  1.3440us  2.3040us  [CUDA memcpy HtoD]
      API calls:   81.17%  381.81ms         1  381.81ms  381.81ms  381.81ms  cuCtxCreate
                   17.03%  80.118ms         1  80.118ms  80.118ms  80.118ms  cuCtxDestroy
                    0.93%  4.3728ms         1  4.3728ms  4.3728ms  4.3728ms  cuModuleLoadDataEx
                    0.58%  2.7387ms         1  2.7387ms  2.7387ms  2.7387ms  cuModuleUnload
                    0.10%  468.85us         2  234.42us  226.92us  241.93us  cuCtxSynchronize
                    0.06%  298.09us         5  59.618us  5.9500us  266.05us  cuMemAlloc
                    0.06%  276.94us         5  55.388us  7.3200us  235.78us  cuMemFree
                    0.04%  185.39us         5  37.077us  27.568us  50.892us  cuMemcpyDtoH
                    0.01%  58.870us         2  29.435us  24.049us  34.821us  cuLaunchKernel
```

You can also look at the examples in the offload-101 directory.