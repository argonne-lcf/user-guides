# SYCL 

>SYCL (pronounced ‘sickle’) is a royalty-free, cross-platform abstraction layer that enables code for heterogeneous processors to be written using standard ISO C++ with the host and kernel code for an application contained in the same source file. 

- Specification: https://www.khronos.org/sycl/
- Source code of the compiler: https://github.com/intel/llvm
- ALCF Tutorial: https://github.com/argonne-lcf/sycltrain

```
module use /soft/compilers
module load mpiwrappers/cray-mpich-llvm
module load llvm-sycl/2022-06 
module load nvhpc/21.9
export MPICH_GPU_SUPPORT_ENABLED=1

```

# Example (using GPU-aware MPI)

```
$ cat main.cpp
int main(){

}

module use /soft/compilers
module load mpiwrappers/cray-mpich-llvm
module load llvm-sycl/2022-06
module load llvm-sycl/2022-06 
module load nvhpc/21.9
export MPICH_GPU_SUPPORT_ENABLED=1

LIBS=/opt/cray/pe/mpich/8.1.16/gtl/lib

$ mpicxx -L$(LIBS) -lmpi_gtl_cuda -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend '--cuda-gpu-arch=sm_80' main.cpp
$ mpiexec -n 2 --ppn 2 --depth=1 --cpu-bind depth ./set_aff_gpu_polaris.sh ./a.out
```
