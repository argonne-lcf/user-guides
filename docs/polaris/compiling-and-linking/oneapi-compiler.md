# oneAPI Compilers and Support

The Intel oneAPI compiler and Codeplay plugins for Nvidia GPUs are available on Polaris.
The oneAPI compilers are not enabled under the Cray Programming Environment system but can be used separately.
Two oneAPI variants are provided, the first being a "release" version based on Intel's officially released oneAPI toolkit.
[Intel Release Notes](https://www.intel.com/content/www/us/en/developer/articles/release-notes/intel-oneapi-toolkit-release-notes.html)

!!! note

    The 2023.2.0 release of oneAPI Toolkit does not support oneMKL or oneDPL on Nvidia.

The other variant being a build of main from the open source. This variant will be more up-to-date at the risk of bugs and breakages based on code that has not undergone a full release cycle.
The documentation is located on the [SYCL](../programming-models/sycl-polaris.md) page.


## Compile and Link
oneAPI uses the clang (or icx/icpx wrapper) for compiling and linking for the Nvidia A100 SM80 architecture.


```
module load PrgEnv-nvhpc
module use /soft/compilers/oneapi/release/modulefiles
module load compiler
icpx -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 test.cpp
```

```
harms@polaris-login-04:~/working/polaris/oneapi> icpx --version
Intel(R) oneAPI DPC++/C++ Compiler 2023.2.0 (2023.2.0.20230721)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /soft/compilers/oneapi/release/2023.2/compiler/2023.2.1/linux/bin-llvm
Configuration file: /soft/compilers/oneapi/release/2023.2/compiler/2023.2.1/linux/bin-llvm/../bin/icpx.cfg
```

## Running
The library should select the GPU by default, but selection of the GPUs can be forced via the ONEAPI_DEVICE_SELECTOR
```
$ ONEAPI_DEVICE_SELECTOR=ext_oneapi_cuda:gpu ./a.out
```
or a specific GPU.
```
$ ONEAPI_DEVICE_SELECTOR=ext_oneapi_cuda:gpu:3 ./a.out
```

## sycl-ls
Expected output of sycl-ls and which platforms are available.

```
harms@x3004c0s7b0n0:~> which sycl-ls
/soft/compilers/oneapi/release/2023.2/compiler/2023.2.1/linux/bin/sycl-ls

harms@x3004c0s7b0n0:~> sycl-ls
[opencl:acc:0] Intel(R) FPGA Emulation Platform for OpenCL(TM), Intel(R) FPGA Emulation Device 1.2 [2023.16.7.0.21_160000]
[opencl:cpu:1] Intel(R) OpenCL, AMD EPYC 7543P 32-Core Processor                3.0 [2023.16.7.0.21_160000]
[ext_oneapi_cuda:gpu:0] NVIDIA CUDA BACKEND, NVIDIA A100-SXM4-40GB 8.8 [CUDA 11.4]
[ext_oneapi_cuda:gpu:1] NVIDIA CUDA BACKEND, NVIDIA A100-SXM4-40GB 8.8 [CUDA 11.4]
[ext_oneapi_cuda:gpu:2] NVIDIA CUDA BACKEND, NVIDIA A100-SXM4-40GB 8.8 [CUDA 11.4]
[ext_oneapi_cuda:gpu:3] NVIDIA CUDA BACKEND, NVIDIA A100-SXM4-40GB 8.8 [CUDA 11.4]
```
