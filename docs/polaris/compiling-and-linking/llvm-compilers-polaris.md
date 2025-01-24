# LLVM Compilers on Polaris

This page is not about LLVM-based Cray Compiling Environment (CCE) compilers from `PrgEnv-cray` but about open-source LLVM compilers.

If LLVM compilers are needed without MPI support, simply load the `llvm` module.

The Cray Programming Environment does not offer LLVM compiler support. Thus, cc/CC/ftn compiler wrappers using LLVM compilers currently are not available. To use Clang with MPI, one can load the `mpiwrappers/cray-mpich-llvm` module, which loads the following modules:

* `llvm`, upstream LLVM compilers
* `cray-mpich`, MPI compiler wrappers mpicc/mpicxx/mpif90. mpif90 uses gfortran because flang is not ready for production use.
* `cray-pals`, MPI launchers mpiexec/aprun/mpirun

**Limitation**: There is no GPU-aware MPI library linking support by default. If needed, users should manually add the GTL (GPU Transport Layer) library to the application link line.

**Update 04/25/2024**: To access LLVM modules, `module use /soft/modulefiles` is required.

[//]: # (ToDo: Lack of support for PrgEnv-cray is based on man page; should confirm)

## OpenMP offload

When targeting the OpenMP or CUDA programming models for GPUs, the `cudatoolkit-standalone` module should also be loaded.

[//]: # (ToDo: create examples and document)