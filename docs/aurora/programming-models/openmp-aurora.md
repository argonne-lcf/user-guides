# OpenMP on Aurora

## Overview

The OpenMP API is an open standard for parallel programming. The specification document can be found here: [OpenMP Specification](https://www.openmp.org). The specification describes directives, runtime routines, and environment variables that allow an application developer to express parallelism (e.g., shared memory multiprocessing and device offloading). Many compiler vendors provide implementations of the OpenMP specification ([OpenMP Specifications](https://www.openmp.org/specifications)).

## Setting the environment to use OpenMP on Aurora

The Intel oneAPI Programming Environment is the main environment on Aurora to maximally use the hardware. oneAPI has OpenMP support for both CPU threads and GPU devices. The oneAPI module is loaded by default in your environment:

```bash
> module list

Currently Loaded Modules:
  1) gcc/11.2.0                    3) intel_compute_runtime/release/agama-devel-551   5) libfabric/1.15.2.0   7) cray-libpals/1.3.3
  2) mpich/51.2/icc-all-pmix-gpu   4) *oneapi/eng-compiler/2022.12.30.003*              6) cray-pals/1.3.3
```

However, additional versions of oneAPI with newer compiler versions can be found by adding additional modules to your path:

```bash
> module use /soft/modulefiles/
> module avail oneapi

-------------------------------------------------------------------------------- /soft/modulefiles ---------------------------------------------------------------------------------
   oneapi/eng-compiler/2023.05.15.003    oneapi/eng-compiler/2023.10.15.002        oneapi/release/2023.10.15.001        spack-pe-oneapi/0.5-rc1 (D)
   oneapi/eng-compiler/2023.05.15.006    oneapi/eng-compiler/2023.12.15.002 (D)    oneapi/release/2023.12.15.001 (D)
   oneapi/eng-compiler/2023.05.15.007    oneapi/release/2023.05.15.001             spack-pe-oneapi/0.4-rc1

------------------------------------------------------------------------- /opt/aurora/23.073.0/modulefiles -------------------------------------------------------------------------
   oneapi/eng-compiler/2022.12.30.003 (L)    oneapi/release/2022.12.30.001
```

The additional oneAPI modules can be loaded with `module load oneapi/eng-compiler/2023.10.15.002`, for example.

## Building on Aurora

The following table shows the compiler and flags:

| Language | MPI Wrapper Compiler (Underlying Compiler) | Flag to Turn on OpenMP Support and Target CPU Threads | Additional Flags to Target GPU Devices |
| --- | --- | --- | --- |
| Fortran | mpifort (ifx) | `-fiopenmp` | `-fopenmp-targets=spir64_gen -Xopenmp-target-backend "-device pvc"` |
| C | mpicc (icx) | `-fiopenmp` | `-fopenmp-targets=spir64_gen -Xopenmp-target-backend "-device pvc"` |
| C++ | mpicxx (icpx) | `-fiopenmp` | `-fopenmp-targets=spir64_gen -Xopenmp-target-backend "-device pvc"` |

## Running on Aurora

To run, you can execute the produced executable or use `mpiexec` in a job script, and then submit the script to an Aurora queue, like:

```bash
$ cat submit.sh
#!/bin/bash -l
#PBS -l select=1
#PBS -l walltime=0:30:00
#PBS -l filesystems=<fs1:fs2>
#PBS -q <queue> 
#PBS -A <ProjectName>

cd ${PBS_O_WORKDIR}
mpiexec -n 1 ./executable
$ # submit to the queue:
$ qsub -l select=1 -l walltime=0:30:00 -q EarlyAppAccess -A Project ./submit.sh
```

In the above, having the PBS options in the script and on the command line is redundant, but we put it there to show both ways of launching. This submits the script to one node in the `EarlyAppAccess` queue on Aurora, requesting 30 minutes. It will charge project `Project` for the time. You should replace it with your project name.

More details for setting up the job script are in the [Job Scheduling and Execution section](../../running-jobs/index.md).

## Example

```cpp
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
```

```fortran
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
```

### To compile

```bash
$ mpicxx -fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend "-device pvc" hello.cpp -o c_test
$ mpifort -fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend "-device pvc" hello.F90 -o f_test
```

### To run 

```bash
$ mpiexec -n 1 ./c_test
Number of devices: 6
Hello world from accelerator.
$ mpiexec -n 1 ./f_test
Number of devices:            6
Hello world from accelerator
```