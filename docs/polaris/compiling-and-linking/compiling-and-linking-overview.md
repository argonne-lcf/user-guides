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

## Man Pages
For additional information on the Cray wrappers, please refer to the man pages.
```
man cc
man CC
man ftn
```