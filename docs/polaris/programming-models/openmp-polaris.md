# OpenMP

## Overview

The OpenMP API is an open standard for parallel programming. The specification document can be found here: [OpenMP Specification](https://www.openmp.org). The specification describes directives, runtime routines, and environment variables that allow an application developer to express parallelism (e.g., shared memory multiprocessing and device offloading). Many compiler vendors provide implementations of the OpenMP specification ([OpenMP Specifications](https://www.openmp.org/specifications)).

## Setting the environment to use OpenMP on Polaris

Many of the programming environments available on Polaris have OpenMP support.

| Module | OpenMP CPU Support? | OpenMP GPU Support? |
| --- | --- | --- |
| PrgEnv-nvhpc | Yes | Yes | 
| llvm | Yes | Yes |
| PrgEnv-gnu | Yes | No |
| PrgEnv-cray | Yes | Yes* |

*Currently, PrgEnv-cray is not recommended for OpenMP offload.

By default, the PrgEnv-nvhpc module is loaded. To switch to other modules, you can use `module switch`.

### Using PrgEnv-nvhpc

This is loaded by default, so there's no need to load additional modules. You can confirm that it is loaded by running `module list` to check that PrgEnv-nvhpc is in the list.

### Using LLVM

To use the LLVM module, load the following:

```
module use /soft/modulefiles
module load mpiwrappers/cray-mpich-llvm
module load cudatoolkit-standalone
```

See the LLVM compiling page [here](../compiling-and-linking/llvm-compilers-polaris.md) for more information.

### Using PrgEnv-gnu

To switch from PrgEnv-nvhpc to PrgEnv-gnu, you can run:

```
module switch PrgEnv-nvhpc PrgEnv-gnu
```

The gcc/gfortran on Polaris was not built with GPU support.

### Using PrgEnv-cray

To switch from PrgEnv-nvhpc to PrgEnv-cray, you can run:

```
module switch PrgEnv-nvhpc PrgEnv-cray
```

To use OpenMP on the GPU, load `cudatoolkit-standalone`, although this is not recommended at the moment.

```
module load cudatoolkit-standalone
```

## Building on Polaris

The following table shows what compiler and flags to use with which PrgEnv:

| Module | Compiler | Flags |
| --- | --- | --- |
| PrgEnv-nvhpc | cc/CC/ftn (nvc/nvc++/nvfortran) | -mp=gpu -gpu=cc80 | 
| llvm | mpicc/mpicxx (clang/clang++) | -fopenmp --offload-arch=sm_80 | 
| PrgEnv-gnu | cc/CC/ftn (gcc/g++/gfortran) | -fopenmp |
| PrgEnv-cray | cc/CC/ftn | -fopenmp |

For example, to compile a simple code hello.cpp:

### For PrgEnv-nvhpc, after loading the modules as discussed above, we would use:

```
CC -mp=gpu -gpu=cc80 hello.cpp
ftn -mp=gpu -gpu=cc80 hello.F90
```

### For LLVM, after loading the modules as discussed above:

```
mpicxx -fopenmp --offload-arch=sm_80 hello.cpp 
```

Note that if you want to force the code to error out if it cannot run on the GPU (instead of falling back to run on the host, which is the default), you can additionally compile with `-fopenmp-offload-mandatory`.

### For PrgEnv-gnu, after loading the modules as discussed above, we would use:

```
CC -fopenmp hello.cpp
ftn -fopenmp hello.F90
```

### For PrgEnv-cray, after loading the modules as discussed above, we would use:

```
CC -fopenmp hello.cpp
ftn -fopenmp hello.F90
```

## Running on Polaris

To run, you can execute the produced executable or use mpiexec in a job script, and then submit the script to the Polaris queue, like:

```
$ cat submit.sh
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:eagle

cd ${PBS_O_WORKDIR}
mpiexec -n 1 ./executable
$ # submit to the queue:
$ qsub -l select=1:system=polaris -l walltime=0:30:00 -l filesystems=home:eagle -q debug -A Catalyst ./submit.sh
```

In the above, having the PBS options in the script and on the command line is redundant, but we put it there to show both ways of launching. This submits the script to one node in the debug queue on Polaris, requesting 30 minutes and the eagle and home filesystems. It will charge project Catalyst for the time.

More details for setting up the job script are in the [Job Scheduling and Execution section](../../running-jobs/index.md).

## Example

```
$ cat hello.cpp
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {

  printf("Number of devices: %d\n", omp_get_num_devices());

  #pragma omp target
  {
    if (!omp_is_initial_device())
      printf("Hello world from accelerator.\n");
    else
      printf("Hello world from host.\n");
  }
  return 0;
}

$ cat hello.F90
program main
  use omp_lib
  implicit none
  integer flag
  
  write(*,*) "Number of devices:", omp_get_num_devices()

  !$omp target map(from:flag)
    if (.not. omp_is_initial_device()) then
      flag = 1
    else
      flag = 0
    endif
  !$omp end target

  if (flag == 1) then
    print *, "Hello world from accelerator"
  else
    print *, "Hello world from host"
  endif

end program main

$ # To compile
$ CC -mp=gpu -gpu=cc80 hello.cpp -o c_test
$ ftn -mp=gpu -gpu=cc80 hello.F90 -o f_test

$ # To run 
$ mpiexec -n 1 ./c_test
Number of devices: 4
Hello world from accelerator.
$ mpiexec -n 1 ./f_test
 Number of devices:            4
 Hello world from accelerator
```