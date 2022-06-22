# Compiling and Linking Overview on Polaris

## Polaris Nodes

### Login Nodes

The login nodes do not currently have GPUs installed and thus compiling GPU-enabled applications may not be straightforward depending on build system and dependencies. Users are encourged to build their applications on Polaris compute nodes for the time being. Compilation of non-GPU codes will work well on the current Polaris login nodes. 

### Interactive Jobs on Compute Nodes

The present configuration of the Polaris login nodes do not contain GPUs. As such, users are more likely to run into fewer issues while building GPU-enabled applications if they instead use one of the Polaris compute nodes. This can easily be accomplished by submitting a short interactive job.
```
qsub -I -l select=1 -l walltime=1:00:00
```
A shell prompt on a compute node will be available once the interactive job starts for you to start building your application and running test jobs.

## Cray Programming Environment

The Cray Programming Environment (PE) has 3 main compiler wrappers for building software. These compiler wrappers should be used 

- `cc` - C compiler
- `CC` - C++ compiler
- `ftn` - Fortran compiler

Each of these wrappers can select a specific vendor compiler based on the PrgEnv module loaded in your environment. The following are some helpful options to understand what the compiler wrapper is invoking.

- `--craype-verbose` : Print the command which is forwarded to the compiler invocation
- `--cray-print-opts=libs` : Print library information
- `--cray-print-opts=cflags` : Print include information

The output from these commands may be useful in build scripts where a compiler other than that invoked by a compiler wrapper is desired. An example that might be found in a Makefile using `nvcc` follows.
```
CRAY_CFLAGS=$(cc --cray-print-opts=cflags)
nvcc -x cu $(CRAY_CFLAGS) main.cpp
```

## Compilers

The default programming environment on Polaris is currently NVIDIA. The GNU compilers are available and the following `module` command, for example, could be used to switch from the NVIDIA programming environment.

```
module swap PrgEnv-nvidia PrgEnv-gnu
```
The compilers invoked by the Cray MPI wrappers are listed for each programming environment in the following table.

|module| C | C++ | Fortran |
| --- | --- | --- | --- |
| MPI Compiler Wrapper | cc | CC | ftn |
| PrgEnv-nvidia | nvc | nvc++ | nvfortran |
| PrgEnv-gnu | gcc | g++ | gfortran |

Note, while gcc and g++ may be available in the default environment, the `PrgEnv-gnu` module is needed to provide gfortran.

[//]: # (ToDo: do the gnu compilers loaded by PrgEnv-gnu have non-zero support for GPUs?)

## Linking

Dynamic linking of libraries is currently the default on Polaris. The Cray MPI wrappers will handle this automatically.

[//]: # (ToDo: confirm that CRAYPE_LINK_TYPE=static works as intended)
[//]: # (ToDo: confirm the compiler wrappers accept -static and -dynamic and work as intended)

## Mixed C/C++ & Fortran Applications

For applications consisting of a mix of C/C++ and Fortran that also uses MPI, it is suggested that the programming environment chosen for Fortran be used to build the full application because of mpi.mod (and similar) incompatibilities. 

## Compiling for GPUs

Loading the `craype-accel-nvidia80` module adds `-gpu` to the compiler invocation along with additional include paths and libraries. Additional compilers flags may be needed depending on the compiler and programming model used (e.g. `-cuda`, `-acc`, or `-mp=gpu`). This module also adds GPU Transport Layer (GTL) libraries to the link-line to support GPU-aware MPI applications.

## Notes on NVIDIA Compilers
### PGI compilers

The NVIDIA programming environments makes available compilers from the [NVIDIA HPC SDK](https://developer.nvidia.com/hpc-sdk). While the PGI compilers are available in this programming environment, it should be noted they are actually symlinks.
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

## Man Pages
For additional information on the Cray wrappers, please refer to the man pages.
```
man cc
man CC
man ftn
```