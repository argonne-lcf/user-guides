# Compiling and Linking Overview on Polaris

## Polaris Nodes

### Login Nodes

The login nodes do not currently have GPUs installed. It is still possible to compile GPU-enabled applications on the login nodes depending on the requirements of your applications build system. If a GPU is required for compilation, then users are encouraged for the time being to build their applications on a Polaris compute node. This can be readily accomplished by submitting an interactive single-node job. Compilation of non-GPU codes is expected to work well on the current Polaris login nodes.

### Interactive Jobs on Compute Nodes

Submitting a single-node interactive job to, for example, build and test applications on a Polaris compute node can be accomplished using the `qsub` command.
```
qsub -I -l select=1 -l walltime=1:00:00
```
This command requests 1 node for a period of 1 hour. After waiting in the queue for a node to become available, a shell prompt on a compute node will become available. Users can then proceed to start building applications and testing job submission scripts.

## Cray Programming Environment

The Cray Programming Environment (PE) uses three compiler wrappers for building software. These compiler wrappers should be used when building MPI-enabled applications.

- `cc` - C compiler
- `CC` - C++ compiler
- `ftn` - Fortran compiler

Each of these wrappers can select a specific vendor compiler based on the PrgEnv module loaded in the environment. The following are some helpful options to understand what the compiler wrapper is invoking.

- `--craype-verbose` : Print the command which is forwarded to the compiler invocation
- `--cray-print-opts=libs` : Print library information
- `--cray-print-opts=cflags` : Print include information

The output from these commands may be useful in build scripts where a compiler other than that invoked by a compiler wrapper is desired. Defining some variables as such may prove useful in those situations.
```
CRAY_CFLAGS=$(cc --cray-print-opts=cflags)
CRAY_LIB=$(cc --cray-print-opts=libs)
```
Further documentation and options are available via `man cc` and similar. 

## Compilers

The default programming environment on Polaris is currently `NVHPC`. The `GNU` compilers are available. In the current early user environment, the following sequence of `module` commands can be used to switch to the `GNU` programming environment (gcc, g++, gfortran) and also have `NVIDIA` compilers available in your path.

```
module swap PrgEnv-nvhpc PrgEnv-gnu
module load nvhpc
module swap PrgEnv-nvhpc PrgEnv-gnu
```
This process of switching to the `GNU` programming environment is expected to be streamlined during the early user phase. 

The compilers invoked by the Cray MPI wrappers are listed for each programming environment in the following table.

|module| C | C++ | Fortran |
| --- | --- | --- | --- |
| MPI Compiler Wrapper | cc | CC | ftn |
| PrgEnv-nvhpc | nvc | nvc++ | nvfortran |
| PrgEnv-gnu | gcc | g++ | gfortran |

Note, while gcc and g++ may be available in the default environment, the `PrgEnv-gnu` module is needed to provide gfortran.

[//]: # (ToDo: do the gnu compilers loaded by PrgEnv-gnu have non-zero support for GPUs?)

## Linking

Dynamic linking of libraries is currently the default on Polaris. The Cray MPI wrappers will handle this automatically.

[//]: # (ToDo: first attempt at using CRAYPE_LINK_TYPE=static didn't go well; couldn't find libpthread and lrt)

## Mixed C/C++ & Fortran Applications

For applications consisting of a mix of C/C++ and Fortran that also uses MPI, it is suggested that the programming environment chosen for Fortran be used to build the full application because of mpi.mod (and similar) incompatibilities. 

## Compiling for GPUs

It is assumed the majority of applications to be built on Polaris will make use of the GPUs. As such, the `craype-accel-nvidia80` module is in the default environment. This has the effect of adding `-gpu` to the compiler invocation along with additional include paths and libraries. Additional compilers flags may be needed depending on the compiler and GPU programming model used (e.g. `-cuda`, `-acc`, or `-mp=gpu`). 

This module also adds GPU Transport Layer (GTL) libraries to the link-line to support GPU-aware MPI applications. Note, there is currently an issue in the early Polaris software environment that may prevent applications from using GPU-enabled MPI.

## Man Pages
For additional information on the Cray wrappers, please refer to the man pages.
```
man cc
man CC
man ftn
```