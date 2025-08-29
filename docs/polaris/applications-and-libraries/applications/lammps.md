# LAMMPS

## Overview

LAMMPS is a general-purpose molecular dynamics software package for massively parallel computers. It is written in an exceptionally clean style that makes it one of the more popular codes for users to extend, and it currently has dozens of user-developed extensions.

For details about the code and its usage, see the [LAMMPS](http://lammps.sandia.gov/) home page. This page provides information specific to running on Polaris at the ALCF.

## Using LAMMPS at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). A collection of Makefiles and submission scripts are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Polaris/LAMMPS). For questions, contact us at <support@alcf.anl.gov>.

## How to Obtain the Code

LAMMPS is an open-source code, which can be downloaded from the LAMMPS [website](http://lammps.sandia.gov/download.html).

## Setting up GNU environment

The following modules are useful for building LAMMPS. The initial `module restore` is just setting the default environment as the starting point. Users may find it useful to copy these module commands into a small helper script to assist with compiling and running LAMMPS (e.g., `setup_lammps_gnu.sh`). 

```
module restore

# use -target-accel=nvidia90 in compiler/link flags
#module load craype-accel-nvidia90

module swap PrgEnv-nvidia PrgEnv-gnu
module swap gcc-native/14.2 gcc-native/12.3

module use /soft/modulefiles
module load cudatoolkit-standalone

module load spack-pe-base cmake
module load cray-fftw
```

## Building with KOKKOS package on Polaris 

Users are encouraged to use cmake to build LAMMPS using the GNU host compilers. An example helper script is provided where `cmake` is used to build LAMMPS using the KOKKOS package for GPU acceleration.

```
BASE=/path_to_lammps/

cd ${BASE}
mkdir build
cd build

. /home/knight/public/lammps/polaris/setup_lammps_gnu.sh 

export NVCC_WRAPPER_DEFAULT_COMPILER=CC

cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=$(pwd) \
    -D CMAKE_CXX_STANDARD=17 \
    -D CMAKE_CXX_STANDARD_REQUIRED=ON \
    -D BUILD_MPI=ON \
    -D BUILD_SHARED_LIBS=ON \
    -D PKG_KOKKOS=ON \
    -D PKG_MOLECULE=ON \
    -D PKG_ML-SNAP=ON \
    -D PKG_KSPACE=ON \
    -D Kokkos_ENABLE_CUDA=ON \
    -D FFT_KOKKOS=CUFFT \
    -D FFT_SINGLE=yes \
    -D Kokkos_ENABLE_OPENMP=ON \
    -D CMAKE_CXX_COMPILER=$(pwd)/../lib/kokkos/bin/nvcc_wrapper \
    -D CMAKE_EXE_LINKER_FLAGS="-target-accel=nvidia90" \
    -D Kokkos_ARCH_AMDAVX=ON \
    ../cmake

make -j 4
```

If building LAMMPS in an interactive job on a compute node, then `make -j 32` could be used to speedup up compilation. The `${BASE}/build/lmp` executable should be present when compilation succeeds.

## Building with GPU package on Polaris 

Users are encouraged to use cmake to build LAMMPS using the GNU host compilers. An example helper script is provided where `cmake` is used to build LAMMPS using the KOKKOS package for GPU acceleration.

```
BASE=/path_to_lammps/

cd ${BASE}
mkdir build
cd build

. /home/knight/public/lammps/polaris/setup_lammps_gnu.sh 

cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=$(pwd) \
    -D CMAKE_CXX_STANDARD=17 \
    -D CMAKE_CXX_STANDARD_REQUIRED=ON \
    -D BUILD_MPI=ON \
    -D BUILD_SHARED_LIBS=ON \
    -D PKG_OPENMP=ON \
    -D PKG_GPU=ON \
    -D GPU_API=CUDA \
    -D GPU_ARCH=sm_90 \
    -D CUDA_MPS_SUPPORT=yes \
    -D PKG_MOLECULE=ON \
    -D PKG_KSPACE=ON \
    -D FFT_SINGLE=yes \
    -D CMAKE_CXX_COMPILER=CC \
    -D CMAKE_EXE_LINKER_FLAGS="-target-accel=nvidia90" \
    ../cmake

make -j 4
```

If building LAMMPS in an interactive job on a compute node, then `make -j 32` could be used to speedup up compilation. The `${BASE}/build/lmp` executable should be present when compilation succeeds.

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

. /home/knight/public/lammps/polaris/setup_lammps_gnu.sh

EXE=/path_to_lammps/build/lmp
EXE_ARG="-in in.lj "

# Kokkos package settings
EXE_ARG+=" -k on g ${NGPUS} -sf kk -pk kokkos neigh half neigh/qeq full newton on "

# GPU package settings
#EXE_ARG+=" -pk gpu ${NGPUS} -sf hybrid gpu omp "

# MPI+OMP settings
MPI_ARG=" -n ${NTOTRANKS} --ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARG=" --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores "

COMMAND="mpiexec ${MPI_ARG} ${OMP_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

## Building LAMMPS with Makefiles

Support for building LAMMPS via Makefiles will be deprecated soon in favor of cmake. Recent Makefiles are available, if needed, for building LAMMPS in the following directory.

```
knight@x3004c0s7b1n0:~/public/lammps/polaris> pwd
/home/knight/public/lammps/polaris

knight@x3004c0s7b1n0:~/public/lammps/polaris> ls Makefile.polaris_gnu*
Makefile.polaris_gnu  Makefile.polaris_gnu_kokkos
```

Please contact us at <support@alcf.anl.gov> for assistance if you require building LAMMPS with Makefiles.

## Performance Notes

Some useful information on accelerator packages and expectations can be found on the LAMMPS website [here](https://docs.lammps.org/Speed_packages.html).