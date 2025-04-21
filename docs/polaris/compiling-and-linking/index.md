# Compiling and Linking Overview on Polaris

## Compiling on Polaris Login and Compute Nodes

Most build systems for GPU applications do not require the use a GPU to compile and link the binary. In such cases, the compilation of GPU-accelerated codes is generally expected to work well on the Polaris login nodes. If your build system _does_ require usable GPUs, you cannot yet compile on the Polaris login nodes. They each currently have a single A100 installed on each, but user processes are forbidden from executing on these GPUs. In this case, you may compile your applications on the Polaris compute nodes. Do this by submitting an [interactive single-node job](../running-jobs/index.md#interactive-jobs-on-compute-nodes) or running your build system in a batch job.

### Home File System

It is helpful to realize that there is a single `HOME` filesystem for users that can be accessed from the login and compute nodes of each production resource at ALCF. Thus, users should be mindful of modifications to their environments (e.g., `.bashrc`) that may cause issues to arise due to differences between the systems.

## Cray Programming Environment

The Cray Programming Environment (PE) uses three compiler wrappers for building software. These compiler wrappers should be used when building MPI-enabled applications.

- `cc` - C compiler
- `CC` - C++ compiler
- `ftn` - Fortran compiler

Each of these wrappers can select a specific vendor compiler based on the PrgEnv module loaded in the environment. The following are some helpful options to understand what the compiler wrapper is invoking.

- `--craype-verbose`: Print the command which is forwarded to the compiler invocation
- `--cray-print-opts=libs`: Print library information
- `--cray-print-opts=cflags`: Print include information

The output from these commands may be useful in build scripts where a compiler other than that invoked by a compiler wrapper is desired. Defining some variables as such may prove useful in those situations.

```bash linenums="1"
CRAY_CFLAGS=$(cc --cray-print-opts=cflags)
CRAY_LIB=$(cc --cray-print-opts=libs)
```

Further documentation and options are available via `man cc` and similar.

## Compilers Provided by Cray Programming Environments

The default programming environment on Polaris is currently `NVHPC`. The `GNU` compilers are available via another programming environment. The following sequence of `module` commands can be used to switch to the `GNU` programming environment (gcc, g++, gfortran) and also have `NVIDIA` compilers available in your path.

```bash linenums="1"
module swap PrgEnv-nvhpc PrgEnv-gnu
module load nvhpc-mixed
```

The compilers invoked by the Cray MPI wrappers are listed for each programming environment in the following table.

| Module | C   | C++   | Fortran   |
|--------|-----|-------|-----------|
| MPI Compiler Wrapper | cc  | CC    | ftn       |
| PrgEnv-nvhpc         | nvc | nvc++ | nvfortran |
| PrgEnv-gnu           | gcc | g++   | gfortran  |

Note, while gcc and g++ may be available in the default environment, the `PrgEnv-gnu` module is needed to provide gfortran.

## Additional Compilers Provided by ALCF

The ALCF additionally provides compilers to enable the OpenMP and SYCL programming models for GPUs via `LLVM` as documented [here](llvm-compilers-polaris.md).

Additional documentation for using compilers is available on the respective programming model pages: [OpenMP](../programming-models/openmp-polaris.md) and [SYCL](../programming-models/sycl-polaris.md).

## Linking

Dynamic linking of libraries is currently the default on Polaris. The Cray MPI wrappers will handle this automatically.

## Notes on Default Modules

* `craype-accel-nvidia80`: This module adds compiler flags to enable GPU acceleration for `NVHPC` compilers along with GPU-aware MPI libraries, as it is assumed that the majority of applications to be compiled on Polaris will target the GPUs for acceleration. Users building CPU-only applications may find it useful to unload this module to silence "GPU code generation" warnings.

* `xalt`: This module enables library tracking; it helps ALCF identify software important to our users. More information can be found on the [XALT](../applications-and-libraries/libraries/xalt.md) page.

## Mixed C/C++ & Fortran Applications

For applications consisting of a mix of C/C++ and Fortran that also use MPI, it is suggested that the programming environment chosen for Fortran be used to build the full application because of mpi.mod (and similar) incompatibilities.

## Compiling for GPUs

It is assumed the majority of applications to be built on Polaris will make use of the GPUs. As such, the `craype-accel-nvidia80` module is in the default environment. This has the effect of the Cray compiler wrappers adding `-gpu` to the compiler invocation along with additional include paths and libraries. Additional compiler flags may be needed depending on the compiler and GPU programming model used (e.g., `-cuda`, `-acc`, or `-mp=gpu`).

This module also adds GPU Transport Layer (GTL) libraries to the link-line to support GPU-aware MPI applications.

## Man Pages

For additional information on the Cray wrappers, please refer to the man pages.

```bash linenums="1"
man cc
man CC
man ftn
```
