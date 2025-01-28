# LAMMPS

## Overview

LAMMPS is a general-purpose molecular dynamics software package for massively parallel computers. It is written in an exceptionally clean style that makes it one of the more popular codes for users to extend, and it currently has dozens of user-developed extensions.

For details about the code and its usage, see the [LAMMPS](http://lammps.sandia.gov/) home page. This page provides information specific to running on Polaris at the ALCF.

## Using LAMMPS at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). A collection of Makefiles and submission scripts are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). For questions, contact us at <support@alcf.anl.gov>.

## How to Obtain the Code

LAMMPS is an open-source code, which can be downloaded from the LAMMPS [website](http://lammps.sandia.gov/download.html).

## Building on Polaris 

After LAMMPS has been downloaded and unpacked on an ALCF filesystem, users should see a directory whose name is of the form `lammps-<version>`. One should then see the Makefile `lammps-<version>/src/MAKE/MACHINES/Makefile.polaris` in recent versions that can be used as a starting point for compilation on Polaris. Copies of Makefiles for building with the GPU/KOKKOS package using CUDA for GPU support with the GNU/NVHPC compiler are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). For older versions of LAMMPS, you may need to take an existing Makefile (e.g., Makefile.mpi) for your specific version of LAMMPS and edit the top portion appropriately to create a new Makefile.polaris.

### KOKKOS package and GNU compilers

The following modules are useful for this particular build. Note, the `cmake` module is not required if using the GNU Makefile procedure to build LAMMPS. The initial `module restore` is just setting the default environment as the starting point. Users may find it useful to copy these module commands into a small helper script to assist with compiling and running LAMMPS (e.g., `setup_lammps_gnu.sh`). 

```
module restore
module load craype-accel-nvidia80
module swap PrgEnv-nvhpc PrgEnv-gnu

module use /soft/modulefiles
module load cudatoolkit-standalone

module load spack-pe-base cmake
```

The top portion of `Makefile.polaris_gnu_kokkos` used to build LAMMPS with the KOKKOS package using GNU as the host-side compiler is shown as an example.

```
# polaris_nvhpc_kokkos = Flags for NVIDIA A100, GNU Compiler, MPICH, CUDA
# module load craype-accel-nvidia80
# module swap PrgEnv-nvhpc PrgEnv-gnu
# module use /soft/modulefiles
# module load cudatoolkit-standalone
# make polaris_gnu_kokkos -j 32

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler

KOKKOS_DEVICES = Cuda,OpenMP
KOKKOS_ARCH = Ampere80
KOKKOS_ABSOLUTE_PATH = $(shell cd $(KOKKOS_PATH); pwd)
KOKKOS_CUDA_OPTIONS = "enable_lambda,disable_malloc_async"
export NVCC_WRAPPER_DEFAULT_COMPILER = nvc++

CC =        $(KOKKOS_ABSOLUTE_PATH)/bin/nvcc_wrapper
CCFLAGS =  -g -O3 -mp -DLAMMPS_MEMALIGN=64
#CCFLAGS += -DLAMMPS_BIGBIG
SHFLAGS =   -fPIC
DEPFLAGS =  -M

LINK =      $(CC)
LINKFLAGS = $(CCFLAGS)
LIB =
SIZE =      size
```

With the appropriate LAMMPS Makefile in place, an executable can be compiled as in the following example. Note, per-user limits on the login nodes will reduce the maximum parallelism for compilation. Users are encouraged to compile on a compute node in an interactive session if necessary.

```
cd lammps-<version>/src
make yes-KOKKOS
make polaris_gnu_kokkos -j 32
``` 

### KOKKOS package and NVHPC compilers

The following modules are useful for this particular build. Note, the `cmake` module is not required if using the GNU Makefile procedure to build LAMMPS. The initial `module restore` is just setting the default environment as the starting point. Users may find it useful to copy these module commands into a small helper script to assist with compiling and running LAMMPS (e.g., `setup_lammps_nvhpc.sh`). 

```
module restore
module load craype-accel-nvidia80

module use /soft/modulefiles
module load spack-pe-base cmake
```

The top portion of `Makefile.polaris_nvhpc_kokkos` used to build LAMMPS with the KOKKOS package using the NVHPC compilers is shown as an example.

```
# polaris_nvhpc_kokkos = Flags for NVIDIA A100, NVIDIA Compiler, MPICH, CUDA
# make polaris_nvhpc_kokkos -j 16

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler

KOKKOS_DEVICES = Cuda,OpenMP
KOKKOS_ARCH = Ampere80
KOKKOS_ABSOLUTE_PATH = $(shell cd $(KOKKOS_PATH); pwd)
KOKKOS_CUDA_OPTIONS = "enable_lambda,disable_malloc_async"
export NVCC_WRAPPER_DEFAULT_COMPILER = nvc++

CRAY_INC = $(shell CC --cray-print-opts=cflags)
CRAY_LIB = $(shell CC --cray-print-opts=libs)

#$(info CRAY_INC = ${CRAY_INC})
#$(info CRAY_LIB = ${CRAY_LIB})

CC =        $(KOKKOS_ABSOLUTE_PATH)/bin/nvcc_wrapper
CCFLAGS =  -g -O3 -mp -DLAMMPS_MEMALIGN=64
CCFLAGS += $(CRAY_INC)
SHFLAGS =   -fPIC
DEPFLAGS =  -M

LINK =      $(CC)
LIB =
LIB += $(CRAY_LIB)
SIZE =      size
```

With the appropriate LAMMPS Makefile in place, an executable can be compiled as in the following example. Note, per-user limits on the login nodes will reduce the maximum parallelism for compilation. Users are encouraged to compile on a compute node in an interactive session if necessary.

```
cd lammps-<version>/src
make yes-KOKKOS
make polaris_nvhpc_kokkos -j 32
``` 

## Running Jobs on Polaris

An example submission script for running a 64-node KOKKOS-enabled LAMMPS executable is below as an example. Additional information on LAMMPS application flags and options is described on the LAMMPS website.

```
#!/bin/sh
#PBS -l select=64:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -l filesystems=home:eagle
#PBS -q prod
#PBS -A Catalyst

export MPICH_GPU_SUPPORT_ENABLED=1

NNODES=`wc -l < $PBS_NODEFILE`

# per-node settings
NRANKS=4
NDEPTH=8
NTHREADS=1
NGPUS=4

NTOTRANKS=$(( NNODES * NRANKS ))

. ./setup_lammps_gnu.sh

EXE=/home/knight/bin/lmp_polaris_gnu_kokkos
EXE_ARG="-in in.reaxc.hns -k on g ${NGPUS} -sf kk -pk kokkos neigh half neigh/qeq full newton on "

# OMP settings mostly to quiet Kokkos messages

MPI_ARG=" -n ${NTOTRANKS} --ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARG=" --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores "

COMMAND="mpiexec ${MPI_ARG} ${OMP_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

## GPU package

The module environments above can be used to build LAMMPS with the GPU package as well. Copies of Makefiles for building with the GPU package using CUDA for GPU support with the GNU and NVHPC compilers are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). 

## Performance Notes

Some useful information on accelerator packages and expectations can be found on the LAMMPS website [here](https://docs.lammps.org/Speed_packages.html).