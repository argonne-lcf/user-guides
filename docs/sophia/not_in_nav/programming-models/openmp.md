# OpenMP on ThetaGPU

## OpenMP threading on CPU

All the compilers available on ThetaGPU support OpenMP threading.

## OpenMP offload on A100 GPU

A few compilers that support OpenMP offload are accessible on ThetaGPU. They are made available via modules.

```
$ module avail llvm

--------- /lus/theta-fs0/software/environment/thetagpu/lmod/modulefiles ----------
   llvm/main-20210112    llvm/release-12.0.0 (D)

$ module avail nvhpc

--------- /lus/theta-fs0/software/environment/thetagpu/lmod/modulefiles ----------
   nvhpc-byo-compiler/20.9 (D)    nvhpc-nompi/20.9 (D)    nvhpc/20.9 (D)
   nvhpc-byo-compiler/21.2        nvhpc-nompi/21.2        nvhpc/21.2
   nvhpc-byo-compiler/21.3        nvhpc-nompi/21.3        nvhpc/21.3
```

## LLVM Clang for C/C++

- [Clang OpenMP offload features](https://clang.llvm.org/docs/OpenMPSupport.html#basic-support-for-cuda-devices)
- [More details about the OpenMP runtime](https://openmp.llvm.org/docs/)

If there is an issue with the compiler, feel free to contact [openmp-dev@lists.llvm.org](mailto:openmp-dev@lists.llvm.org)

### Warning message

```
clang-12 warning: Unknown CUDA version. version.txt: 11.0.205. Assuming the latest supported version 10.1 [-Wunknown-cuda-version]
```

This means CUDA 11 language features are not supported. As long as these features are not used, the full CUDA 11.x toolchain works. This warning can be ignored or suppressed by the `-Wno-unknown-cuda-version` compiler option.

### Compiling example

```bash
module load llvm/release-12.0.0
clang++ -fopenmp -fopenmp-targets=nvptx64 your_source.cpp
```

### NVIDIA HPC SDK for C/C++/Fortran

[OpenMP documentation](https://docs.nvidia.com/hpc-sdk/compilers/hpc-compilers-user-guide/index.html#openmp-use)

For compiler bugs, please file bug reports at [NVIDIA Developer](https://developer.nvidia.com) after logging in.

### Compiling example

```bash
module load nvhpc-sdk/nvhpc/21.3
nvfortran -mp=gpu -gpu=cc80 your_source.f90
```