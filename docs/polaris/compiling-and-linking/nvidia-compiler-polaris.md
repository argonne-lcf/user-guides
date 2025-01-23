# NVIDIA Compilers on Polaris

The NVIDIA compilers (`nvc`, `nvc++`, `nvcc`, and `nvfortran`) are available on Polaris via the `PrgEnv-nvhpc` and `nvhpc` modules. There is currently a `PrgEnv-nvidia` module available, but that will soon be deprecated in Cray's PE, thus it is not recommended for use.

The Cray compiler wrappers map to NVIDIA compilers as follows:

```
cc -> nvc
CC -> nvc++
ftn -> nvfortran
```

Users are encouraged to look through [NVIDIA's documentation](https://developer.nvidia.com/hpc-sdk) for the NVHPC SDK and specific information on the compilers, tools, and libraries.

## Notes on NVIDIA Compilers
### PGI compilers

The NVIDIA programming environments make available compilers from the [NVIDIA HPC SDK](https://developer.nvidia.com/hpc-sdk). While the PGI compilers are available in this programming environment, it should be noted they are actually symlinks to the corresponding NVIDIA compilers.

```
pgcc -> nvc
pgc++ -> nvc++
pgf90 -> nvfortran
pgfortran -> nvfortran
```

While `nvcc` is the traditional CUDA C and CUDA C++ compiler for NVIDIA GPUs, the `nvc`, `nvc++`, and `nvfortran` compilers additionally target CPUs.

### NVHPC SDK Directory Structure

Users migrating from CUDA toolkits to the NVHPC SDK may find it beneficial to review the directory structure of the `hpc-sdk` directory to find the location of commonly used libraries (including math libraries for the CPU). With the `PrgEnv-nvhpc` module loaded, the `NVIDIA_PATH` environment variable can be used to locate the path to various NVIDIA tools, libraries, and examples.

- `compiler/bin` - cuda-gdb, ncu, nsys, ...
- `examples` - CUDA-Fortran, OpenMP, ...
- `comm_libs` - nccl, nvshmem, ...
- `compiler/libs` - blas, lapack, ...
- `cuda/lib64` - cudart, OpenCL, ...
- `math_libs/lib64` - cublas, cufft, ...

### Differences between nvcc and nvc/nvc++

For users that want to continue using `nvcc`, it is important to be mindful of differences with the newer `nvc` and `nvc++` compilers. For example, the `-cuda` flag instructs `nvcc` to compile `.cu` input files to `.cu.cpp.ii` output files, which are to be separately compiled, whereas the same `-cuda` flag instructs `nvc`, `nvc++`, and `nvfortran` to enable CUDA C/C++ or CUDA Fortran code generation. The resulting output file in each case is different (text vs. object), and one may see an `unrecognized format error` when `-cuda` is incorrectly passed to `nvcc`.

### Known Issues and Workarounds

If you are using `nvcc` to invoke `nvc++` and compiling C++17 code, and are seeing the following warning and unable to compile C++17 constructs:

```
polaris-login-01(~)> nvcc --std=c++17 -ccbin nvc++ ~/smalltests/bool_constant.cpp
nvcc warning : The -std=c++17 flag is not supported with the configured host compiler. Flag will be ignored.
"/home/zippy/smalltests/bool_constant.cpp", line 10: error: namespace "std" has no member class "bool_constant"
      : std::bool_constant<(UnaryPred<Ts>::value || ...)> {};
             ^

"/home/zippy/smalltests/bool_constant.cpp", line 10: error: class or struct definition is missing
      : std::bool_constant<(UnaryPred<Ts>::value || ...)> {};
                          ^

2 errors detected in the compilation of "/home/zippy/smalltests/bool_constant.cpp".
polaris-login-01(~)>
```

you will need to work around it by loading the latest cudatoolkit module atop PrgEnv-nvhpc:

```
module load cudatoolkit-standalone/11.6.2
```

[//]: # (ToDo: repeat here some of the nvidia-specific items from general compiling page??)
[//]: # (ToDo: do we want separate pages for each compiler or a single compiler page with brief info on each of them with links to further info??)