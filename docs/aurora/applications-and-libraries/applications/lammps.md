# LAMMPS

## Overview

LAMMPS is a general-purpose molecular dynamics software package for massively parallel computers. It is written in an exceptionally clean style that makes it one of the more popular codes for users to extend, and it currently has dozens of user-developed extensions.

For details about the code and its usage, see the [LAMMPS](http://lammps.sandia.gov/) home page. This page provides information specific to running on Aurora at the ALCF.

## Using LAMMPS at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). A collection of Makefiles, CMake build scripts, and submission scripts are available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Aurora/LAMMPS). For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to Obtain the Code

LAMMPS is an open-source code, which can be downloaded from the LAMMPS [website](http://lammps.sandia.gov/download.html).

## Building with KOKKOS package on Aurora

Users are encouraged to use CMake to build LAMMPS using the default compilers and the `kokkos-sycl-intel.cmake` configuration file included in the LAMMPS repo. An example helper script is provided where CMake is used to build LAMMPS using the KOKKOS package for GPU acceleration.

```
BASE=/path_to_lammps/

module load cmake

mkdir -p ${BASE}/build
cd ${BASE}/build

cmake \
    -C ../cmake/presets/kokkos-sycl-intel.cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=${PWD} \
    -D PKG_MOLECULE=ON \
    -D PKG_RIGID=ON \
    -D PKG_KSPACE=ON \
    -D PKG_MANYBODY=ON \
    -D PKG_ML-SNAP=ON \
    ../cmake

make -j 8
```

Compilation on the Aurora login nodes will work, but should be done with few processes as they are a shared resource. If building LAMMPS in an interactive job on a compute node, then `make -j 32` could be used to speedup up compilation. The `${BASE}/build/lmp` executable should be present when compilation succeeds.

## Running Jobs on Aurora with Kokkos package

An example submission script for a KOKKOS-enabled LAMMPS executable is below as an example. Additional information on LAMMPS application flags and options is described on the LAMMPS website.

A GPU affinity script, such as `gpu_tile_compact.sh` must be used to properly bind MPI ranks to GPU resources. As these affinity scripts only make a single device visible to each MPI rank, the LAMMPS command-line option requesting a number of GPUs should always be set to 1. In this example, each MPI rank is bound to a single tile of an Aurora GPU (12 per node). This is likely the optimal configuration when running with the Kokkos package on Aurora.

```
#!/bin/sh
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -l filesystems=home:flare
#PBS -q debug
#PBS -A Catalyst

NNODES=`wc -l < $PBS_NODEFILE`

# per-node settings
NRANKS_PER_NODE=12
NTHREADS=1

export MPICH_GPU_SUPPORT_ENABLED=1
export OMP_NUM_THREADS=${NTHREADS}
export OMP_PROC_BIND=spread
export OMP_PLACES=cores

NTOTRANKS=$(( NNODES * NRANKS ))

cd /path_to_work/

EXE=/path_to_lammps/build/lmp
EXE_ARG="-in in.lj "

# Kokkos package settings
EXE_ARG+=" -k on g 1 -sf kk -pk kokkos neigh half neigh/qeq full newton on "

# MPI+OMP settings
MPI_ARG=" -n ${NTOTRANKS} --ppn ${NRANKS} "
MPI_ARG+=" --cpu-bind list:1:2:3:4:5:6:53:54:55:56:57:58 "
MPI_ARG+=" --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores "

AFFINITY="gpu_tile_compact.sh"

COMMAND="mpiexec ${MPI_ARG} ${AFFINITY} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

## Building with GPU package on Aurora

Users are encouraged to use CMake to build LAMMPS using the default compilers. An example CMake config script is provided below and available in the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/Aurora/LAMMPS/cmake/presets). This file can be copied to your `/path_to_lammps/cmake/presets/` directory.

```
$ cat aurora.cmake
# preset that enables GPU package and selects OpenCL compilation with OpenMP
# enabled as well. Also sets some performance related compiler flags.

set(PKG_OPENMP ON CACHE BOOL "" FORCE)
set(PKG_GPU ON CACHE BOOL "" FORCE)
set(GPU_API OPENCL CACHE STRING "" FORCE)
set(USE_STATIC_OPENCL_LOADER NO CACHE BOOL "" FORCE)

set(FFT "MKL" CACHE STRING "" FORCE)
set(FFT_SINGLE ON CACHE BOOL "" FORCE)

unset(USE_INTERNAL_LINALG)
unset(USE_INTERNAL_LINALG CACHE)
set(BLAS_VENDOR "Intel10_64_dyn")

set(CMAKE_CXX_COMPILER icpx CACHE STRING "" FORCE)
set(CMAKE_C_COMPILER icx CACHE STRING "" FORCE)
set(CMAKE_Fortran_COMPILER ifx CACHE STRING "" FORCE)
set(MPI_CXX_COMPILER "mpicxx" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -xSAPPHIRERAPIDS -O2 -ffp-model=fast -qoverride-limits -qopt-zmm-usage=high " CACHE STRING "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -xSAPPHIRERAPIDS -O2 -ffp-model=fast -qoverride-limits -qopt-zmm-usage=high " CACHE STRING "" FORCE)

```

An example helper script is provided where CMake is used to build LAMMPS using the GPU package for GPU acceleration.

```
BASE=/path_to_lammps/

module load cmake

mkdir -p ${BASE}/build
cd ${BASE}/build

cmake \
    -C ../cmake/presets/aurora.cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=${PWD} \
    -D PKG_MOLECULE=ON \
    -D PKG_RIGID=ON \
    -D PKG_KSPACE=ON \
    ../cmake

make -j 4
```

Compilation on the Aurora login nodes will work, but should be done with few processes as they are a shared resource. If building LAMMPS in an interactive job on a compute node, then `make -j 32` could be used to speedup up compilation. The `${BASE}/build/lmp` executable should be present when compilation succeeds.

## Running Jobs on Aurora with GPU package

An example submission script for a GPU-enabled LAMMPS executable is below as an example. Additional information on LAMMPS application flags and options is described on the LAMMPS website.

A GPU affinity script, such as `gpu_tile_compact.sh` must be used to properly bind MPI ranks to GPU resources. As these affinity scripts only make a single device visible to each MPI rank, the LAMMPS command-line option requesting a number of GPUs should always be set to 1. In this example, each MPI rank is bound to a single tile of an Aurora GPU (12 per node). 

When running with the GPU package, it will likely be beneficial to run with multiple MPI ranks bound to an individual GPU tile (i.e. 24 or 48 MPI ranks per node). Binding MPI ranks to individual compute-command-streamers can help to improve performance. This is likely the optimal configuration when running with the GPU package on Aurora. Example affinity scripts are available at the paths in example below and the ALCF GettingStarted repo [here](https://github.com/argonne-lcf/GettingStarted/tree/master/HelperScripts/Aurora). Additionally, 2 or 4 OpenMP threads per MPI rank (and appropriately updating the `cpu-bind` list) might further improve performance but this very much depends on specific workload.

```
#!/bin/sh
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -l filesystems=home:flare
#PBS -q debug
#PBS -A Catalyst

export MPICH_GPU_SUPPORT_ENABLED=0
export OMP_NUM_THREADS=${NTHREADS}
export OMP_PROC_BIND=spread
export OMP_PLACES=cores

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=12
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS ))

cd /path_to_work/

EXE=/path_to_lammps/build/lmp

EXE_ARG="-in in.lj "

# GPU package settings
EXE_ARG+=" -pk gpu 1 -pk omp ${NTHREADS} -sf hybrid gpu omp "

# MPI+OMP settings
MPI_ARG=" -n ${NTOTRANKS} --ppn ${NRANKS} "
OMP_ARG=" --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores "

if (( NRANKS_PER_NODE <= 12 )); then
MPI_ARG+="--cpu-bind list:1:2:3:4:5:6:53:54:55:56:57:58"
AFFINITY="gpu_tile_compact.sh"
elif (( NRANKS_PER_NODE <= 24)); then
MPI_ARG+="--cpu-bind list:1:2:3:4:5:6:7:8:9:10:11:12:53:54:55:56:57:58:59:60:61:62:63:64"
AFFINITY="/home/knight/scripts/set_affinity_gpu_2ccs.sh"
else
MPI_ARG+="--cpu-bind list:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15:16:17:18:19:20:21:22:23:24:53:54:55:56:57:58:59:60:61:62:63:64:65:66:67:68:69:70:71:72:73:74:75:76"
AFFINITY="/home/knight/scripts/set_affinity_gpu_4ccs.sh"
fi

COMMAND="mpiexec ${MPI_ARG} ${AFFINITY} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

## Building LAMMPS with Makefiles

Support for building LAMMPS via Makefiles will be deprecated soon in favor of CMake. Recent Makefiles are available, if needed, for building LAMMPS in the following directory. Recent versions of Kokkos are required to run on Aurora, so older versions of LAMMPS will likely need updating to work correctly.

```
knight@aurora-uan-0009:~/public/lammps/old> ls *
Makefile.aurora  Makefile.aurora_kokkos

gpu:
Makefile.aurora
```

Please contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov) for assistance if you require building older versions of LAMMPS with Makefiles (or CMake).

## Performance Notes

Some useful information on accelerator packages and expectations can be found on the LAMMPS website [here](https://docs.lammps.org/Speed_packages.html).