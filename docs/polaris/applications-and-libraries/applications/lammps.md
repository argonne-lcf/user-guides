# LAMMPS

## Overview

LAMMPS is a general-purpose molecular dynamics software package for massively parallel computers. It is written in an exceptionally clean style that makes it one of the more popular codes for users to extend and it currently has dozens of user-developed extensions.

For details bout the code and its usage, see the [LAMMPS](http://lammps.sandia.gov/) home page. This page provides information specific to running on Polaris at the ALCF.

## Using LAMMPS at ALCF

ALCF provides assistanc with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). A collection of Makefiles and submission scripts are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). For questions, contact us at <support@alcf.anl.gov>.

## How to Obtain the Code

LAMMPS is an open-source code, which can be downloaded from the LAMMPS [website](http://lammps.sandia.gov/download.html).

## Building on Polaris using KOKKOS package

After LAMMPS has been downloaded and unpacked an ALCF filesystem, users should see a directory whose name is of the form `lammps-<version>`. One should then see the Makefile `lammps-<version>/src/MAKE/MACHINES/Makefile.polaris` in recent versions that can be used for compilation on Polaris. A copy of the Makefile is also available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). For older versions of LAMMPS, you may need to take an existing Makefile (e.g. Makefile.mpi) for your specific version of LAMMPS used and edit the top portion appropratiately to create a new Makefile.polaris files.

The top portion of `Makefile.polaris_kokkos_nvidia` used to build LAMMPS with the KOKKOS package using the NVIDIA compilers is shown as an example.

```
# polaris_nvidia = Flags for NVIDIA A100, NVIDIA Compiler, Cray MPICH, CUDA
# module load craype-accel-nvidia80
# make polaris_kokkos_nvidia -j 16

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler

KOKKOS_DEVICES = Cuda,OpenMP
KOKKOS_ARCH = Ampere80
KOKKOS_ABSOLUTE_PATH = $(shell cd $(KOKKOS_PATH); pwd)
export NVCC_WRAPPER_DEFAULT_COMPILER = nvc++

CRAY_INC = $(shell CC --cray-print-opts=cflags)
CRAY_LIB = $(shell CC --cray-print-opts=libs)

CC =        $(KOKKOS_ABSOLUTE_PATH)/bin/nvcc_wrapper
CCFLAGS =  -g -O3 -mp -DLAMMPS_MEMALIGN=64 -DLAMMPS_BIGBIG
CCFLAGS += $(CRAY_INC)
SHFLAGS =   -fPIC
DEPFLAGS =  -M

LINK =      $(CC)
LINKFLAGS = $(CCFLAGS)
LIB = $(CRAY_LIB)
SIZE =      size
```

With the appropriate LAMMPS Makefile in place an executable can be compiled as in the following example, which uses the NVIDIA compilers.

```
module load craype-accel-nvidia80
cd lammps-<version>/src
make yes-KOKKOS
make polaris_kokkos_nvidia -j 16
``` 

[//]: # (ToDo: get all LAMMPS Makefiles into repos and update links)

## Running Jobs on Polaris

An example submission script for running a KOKKOS-enabled LAMMPS executable is below as an example. Additional information on LAMMPS application flags and options is described on the LAMMPS website.

```
#!/bin/sh
#PBS -l select=64:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:15:00

export MPICH_GPU_SUPPORT_ENABLED=1

NNODES=`wc -l < $PBS_NODEFILE`

# per-node settings
NRANKS=4
NRANKSSOCKET=2
NDEPTH=8
NTHREADS=1
NGPUS=4

NTOTRANKS=$(( NNODES * NRANKS ))

EXE=/home/knight/bin/lammps_polaris_kokkos_nvidia
EXE_ARG="-in in.reaxc.hns -k on g ${NGPUS} -sf kk -pk kokkos neigh half neigh/qeq full newton on "

# OMP settings mostly to quiet Kokkos messages

MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores"

COMMAND="mpiexec ${MPI_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

## Performance Notes

Some useful information on accelerator packages and expectations can be found on the LAMMPS website [here](https://docs.lammps.org/Speed_packages.html).

[//]: # (ToDo: drop in some additional tips)
