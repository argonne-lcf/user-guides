# NVIDIA Compilers on Polaris

The NVIDIA compilers (`nvc`, `nvc++`, `nvcc`, and `nvfortran`) are available on Polaris via the `PrgEnv-nvidia` module. 

The Cray compiler wrappers map to NVIDIA compilers as follows.

```
cc -> nvc
CC -> nvc++
ftn -> nvfortran
```

Users are encouraged to look through (NVIDIA's documentation)[https://developer.nvidia.com/hpc-sdk] for the NVHPC SDK and specific information on the compilers, tools, and libraries.

## Notes on NVIDIA Compilers
### PGI compilers

The NVIDIA programming environments makes available compilers from the [NVIDIA HPC SDK](https://developer.nvidia.com/hpc-sdk). While the PGI compilers are available in this programming environment, it should be noted they are actually symlinks to the `nv#` compilers.
```
pgcc -> nvc
pgc++ -> nvc++
pgf90 -> nvfortran
pgfortran -> nvfortran
```
While `nvcc` is the traditional CUDA C and CUDA C++ compiler for NVIDIA GPUs, the `nvc`, `nvc++`, and `nvfortran` compilers additionally target CPUs.

### NVHPC SDK Directory Structure

Users migrating from CUDA toolkits to the NVHPC SDK may find it beneficial to review the directory structure of the hpc-sdk directory to find the location of commonly used libraries (including math libraries for the CPU). One a `PrgEnv-nvidia` module is loaded, the `NVHPC_ROOT` environment variable can be used to find discover NVIDIA tools, libraries, and examples.

- compiler/bin - cuda-gdb, ncu, nsys, ...
- examples - CUDA-Fortran, OpenMP, ...
- comm_libs - nccl, nvshmem, ...
- compiler/libs - blas, lapack, ...
- cuda/lib64 - cudart, OpenCL, ...
- math_libs/lib64 - cublas, cufft, ...

[//]: # (ToDo: confirm environment variable for hpc-sdk root in Polaris module)

### Differences between nvcc and nvc/nvc++

For users that want to continue using `nvcc` it is important to be mindful of differences with the newer `nvc` and `nvc++` compilers. For example, the `-cuda` flag instructs `nvcc` to compile `.cu` input files to `.cu.cpp.ii` output files which are to be separately compiled, whereas the same flag instructs `nvc` and `nvc++` to enable CUDA C++ or CUDA Fortran code generation. The resulting output file in each case is different (text vs. object) and one may see `unrecognized format error` when `-cuda` is incorrectly passed to `nvcc`.


[//]: # (ToDo: repeat here some of the nvidia-specific items from general compiling page??)
[//]: # (ToDo: do we want separate pages for each compiler or a single compiler page with brief info on each of them with links to further info??)