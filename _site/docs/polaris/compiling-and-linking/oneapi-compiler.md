# oneAPI Compilers and Support

The Intel oneAPI compiler and Codeplay plugins for Nvidia GPUs are available on Polaris. The oneAPI compilers are not enabled under the Cray Programming Environment system but can be used separately. Two oneAPI variants are provided, the first being a "release" version based on Intel's officially released oneAPI toolkit. [Intel Release Notes](https://www.intel.com/content/www/us/en/developer/articles/release-notes/intel-oneapi-toolkit-release-notes.html)

!!! note

    The 2023.2.1 release of the oneAPI Toolkit does not yet support oneDPL on Nvidia devices, though oneMKL is now added from the 2023.2.1 release onwards.

## Components

- The following is a list of components associated with this module:

| User Application  | Component                                               |
|-------------------|---------------------------------------------------------|
| Compilers         | [DPC++](https://codeplay.com/portal/blogs/2023/07/13/oneapi-2023-2-release.html) |
| oneMKL Interfaces | [oneMKL](https://github.com/oneapi-src/oneMKL)          |

The other variant is a build from the open-source. This variant will be more up-to-date at the risk of bugs and breakages based on code that has not undergone a full release cycle. The documentation is located on the [SYCL](../programming-models/sycl-polaris.md) page. The most notable differences are that `icx/icpx` are the names of C/C++ compilers, respectively, when using the release version of the module, whereas `clang/clang++` are for the open-source variant.

## Compile and Link

oneAPI uses the clang (or icx/icpx wrapper) for compiling and linking for the Nvidia A100 SM80 architecture.

```
module load oneapi/release
icpx -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 test.cpp
```

```
harms@polaris-login-04:~/working/polaris/oneapi> icpx -v
Intel(R) oneAPI DPC++/C++ Compiler 2023.2.0 (2023.2.0.20230721)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /soft/compilers/oneapi/release/2023.2/compiler/2023.2.1/linux/bin-llvm
Configuration file: /soft/compilers/oneapi/release/2023.2/compiler/2023.2.1/linux/bin-llvm/../bin/icpx.cfg
Found candidate GCC installation: /usr/lib64/gcc/x86_64-suse-linux/7
Selected GCC installation: /usr/lib64/gcc/x86_64-suse-linux/7
Candidate multilib: .;@m64
Selected multilib: .;@m64
Found CUDA installation: /opt/nvidia/hpc_sdk/Linux_x86_64/21.9/cuda/11.4, version 11.4
```

## Running

The library should select the GPU by default, but the selection of the GPUs can be forced via the ONEAPI_DEVICE_SELECTOR.

```
$ ONEAPI_DEVICE_SELECTOR=cuda:gpu ./a.out
```

or a specific GPU.

```
$ ONEAPI_DEVICE_SELECTOR=cuda:gpu:3 ./a.out
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