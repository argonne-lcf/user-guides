# Kokkos on Cooley
## Overview
Kokkos implements a programming model in C++ for writing performance portable applications targeting all major HPC platforms. It provides abstractions for both parallel execution of code and data management. Kokkos is designed to target complex node architectures with N-level memory hierarchies and multiple types of execution resources. It can use OpenMP, etc as backend programming model. For more information please visit [https://github.com/kokkos/kokkos](https://github.com/kokkos/kokkos)

The Kokkos shared memory programming model is a C++ library, that provides the necessary architecture specific backends (e.g. OpenMP, CUDA, …). To begin with, though, it is important to note that the Kokkos programming model is usable only in C/C++ codes. Hence, for those with Fortran codes, Kokkos must first be encapsulated within C/C++ functions and called from the main Fortran code.

The purpose of this document is to provide guidance on using Kokkos on Cooley. Please see the following pages for tutorial and more information on Kokkos: [Kokkos GitHub](https://github.com/kokkos) and [Kokkos Tutorials](https://extremecomputingtraining.anl.gov/files/2018/08/ATPESC_2018_Track-2_4_8-2_1130am_Ellingwood-Kokkos.pdf).

## Using Kokkos at ALCF
ALCFprovides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Building Kokkos on Cooley
Users should look at the ```/soft/ProgrammingModels/KOKKOS-JULY-29-2019/``` directory. The project directory consists of the following directories that users should feel free to copy over to their own directories.
```
[bramesh@cooleylogin2 KOKKOS-JULY-29-2019]$ ls
kokkos/  kokkos-tools/  kokkos-tutorials/
[bramesh@cooleylogin2 KOKKOS-JULY-29-2019]$
```

In order to compile and use Kokkos on Cooley, users will need the following settings in the .soft.cooley file:
```
+mvapich2-2.2
+cuda-10.0
+gcc-7.1.0
@default
```

The idea behind these settings is to use the CUDA-10 wrapper, “nvcc,” together with the gcc-7.1.0 compiler, and the mvapich 2-2.2 build with the gcc-7.1.0 compiler. In order to compile/run Kokkos, users are encouraged to look at the tutorial example(s) in the following directories:
```
/soft/ProgrammingModels/KOKKOS-JULY-29-2019/kokkos-tutorials/Intro-Full/Exercises/<01,02, …>/Begin
```

The Makefile(s), and the settings therein, should indicate the specifications for the KOKKOS_DEVICE variable that one would need for either the OpenMP, or CUDA, or OpenMP+CUDA backends in Kokkos.
