# Compiling and Linking Overview

## Compiling on Aurora Login and Compute Nodes

If your build system does not require GPUs for the build process, compilation of GPU-accelerated codes is generally expected to work well on the Aurora login nodes. If your build system _does_ require GPUs, then currently that must be done on the compute nodes either via an [interactive](../running-jobs.md#Interactive-Jobs-on-Compute-Nodes) or [batch](../running-jobs.md#Batch-Jobs-on-Compute-Nodes) job submission. Doing this interactively in a single-node job may be the preferred route as it also provides opportunity to quickly test the executable.

## Filesystem

It is helpful to realize that currently there is a single _temporary_ filesystem `gecko` mounted on the Aurora login and compute nodes available to users, where both `home` and `project` spaces reside. It is important current to realize that this filesystem is not backed up and users should take care to retain copies of important files (e.g. local resources or ALCF's `grand` and `eagle` filesystems).

## OneAPI Programming Environment

The oneAPI programming environment is currently the single environment for building and running software to maximally use the available hardware resources. The oneAPI environment is loaded by default for users and is principally defined by the following set of modules and related variants.

- oneapi
- intel_compute_runtime
- mpich

Additional modules loading `GNU` CPU compilers and parallel application launch support (e.g. libfabric and cray-pals) are also provided in the default environment. The oneAPI environment provides C, C++, and Fortran compilers and associated MPICH MPI wrappers for building applications targeting CPUs and GPUs based on the OpenMP, SYCL, and OpenCL programming models. 

- `mpicc` - C Compiler
- `mpicxx` - C++ Compiler (a.k.a `mpic++`)
- `mpifort` - Fortran Compiler (a.k.a `mpif70` & `mpif90`)

## Compilers provided by Cray Programming Environments

The `PrgEnv-gnu` and `PrgEnv-cray` Cray programming environments are currently available as modules for users that neede them. The compilers provided by these environments do not currently support Intel GPUs and should only be used, if at all, for CPU-only code.

## Mixed C/C++ & Fortran Applications

For applications consisting of a mix of programming languages that use MPI, it is important to use the same Fortran compiler for building the application as was used to build MPI because of mpi.mod (and similar) incompatibilities. 

## Additional software and build tools

Additional software is available on Aurora as modules that are not currently available in the default environment to reduce load on the filesystem. Once a shell has opened on the login or compute nodes (i.e. inside of a job script), users can access additional software in `/soft/modulefiles`. 

```
$ module use /soft/modulefiles
```

This will make available builds of the frameworks, Kokkos, additional MPICH builds, and also some spack modules, which provide access to additional tools. The `module use /soft/modulefiles` command only needs to be executed once per session, but is repeated below for clarity.

```
$ module use /soft/modulefiles
$ module load spack-pe-gcc
```

Looking at the available modules (`module avail`) now in this expanded list one can find additional build tools, such as `cmake`.

```
$ module use /soft/modulefiles
$ module load spack-pe-gcc cmake
$ cmake --version
cmake version 3.26.4
```
