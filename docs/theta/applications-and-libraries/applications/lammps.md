# LAMMPS on Theta
## Overview
LAMMPS is a general-purpose molecular dynamics software package for massively parallel computers. It is written in an exceptionally clean style that makes it one of the most popular codes for users to extend and it currently has dozens of user-developed extensions.

For details about the code and its usage, see the [LAMMPS](http://lammps.sandia.gov/) home page. This page is dedicated to information pertaining to Theta/ThetaGPU at the ALCF.

## Using LAMMPS at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to Obtain the Code
LAMMPS is an open-source code, which can be downloaded at [http://lammps.sandia.gov/download.html](http://lammps.sandia.gov/download.html).

## Building on Theta
After [LAMMPS](http://lammps.sandia.gov/) has been downloaded and unpacked, you should see a directory whose name is of the form lammps-<version>. One should see lammps-<version>/src/MAKE/MACHINES/Makefile.theta in recent versions that can be used for compilation on Theta. The top portion of that Makefile is provided below with suggested compiler settings. For older versions of LAMMPS, you will need to take an existing Makefile (e.g. Makefile.mpi) for the specific version used and edit the top portion appropriately to create a Makefile.theta.
  
```
# theta = Flags for Knights Landing Xeon Phi Processor, Intel compiler, Cray MPI, MKL FFT
# module unload libsci
# make theta -j 8

SHELL = /bin/sh

# ---------------------------------------------------------------------
# compiler/linker settings
# specify flags and libraries needed for your compiler

KOKKOS_DEVICES = OpenMP
KOKKOS_ARCH = KNL

CC       = CC -mkl
OPTFLAGS = -xMIC-AVX512 -O3 -fp-model fast=2 -no-prec-div -qoverride-limits
CCFLAGS  = -g -qopenmp -qno-offload -ansi-alias -restrict 
CCFLAGS += -DLMP_INTEL_USELRT -DLMP_USE_MKL_RNG $(OPTFLAGS)
CCFLAGS += -std=c++11
CCFLAGS += -DLAMMPS_MEMALIGN=64
SHFLAGS  = -fPIC
DEPFLAGS = -M

LINK      = $(CC)
LINKFLAGS = -g -qopenmp $(OPTFLAGS) -dynamic
#LIB       = -ltbbmalloc
LIB       = -L$(TBBROOT)/lib/intel64/gcc4.8 -ltbbmalloc -Wl,-rpath=$(TBBROOT)/lib/intel64/gcc4.8
SIZE      = size

ARCHIVE    = ar
ARFLAGS    = -rc
SHLIBFLAGS = -shared

# ---------------------------------------------------------------------
# LAMMPS-specific settings, all OPTIONAL
# specify settings for LAMMPS features you will use
# if you change any -D setting, do full re-compile after "make clean"

# LAMMPS ifdef settings
# see possible settings in Section 2.2 (step 4) of manual

LMP_INC =

# MPI library
# see discussion in Section 2.2 (step 5) of manual
# MPI wrapper compiler/linker can provide this info
# can point to dummy MPI library in src/STUBS as in Makefile.serial
# use -D MPICH and OMPI settings in INC to avoid C++ lib conflicts
# INC = path for mpi.h, MPI compiler settings
# PATH = path for MPI library
# LIB = name of MPI library

MPI_INC  = -DMPICH_SKIP_MPICXX -DOMPI_SKIP_MPICXX=1
MPI_PATH =
MPI_LIB  =

# FFT library
# see discussion in Section 2.2 (step 6) of manaul
# can be left blank to use provided KISS FFT library
# INC = -DFFT setting, e.g. -DFFT_FFTW, FFT compiler settings
# PATH = path for FFT library
# LIB = name of FFT library

FFT_INC  = -DFFT_MKL -DFFT_SINGLE
FFT_PATH =
FFT_LIB  = -L$(MKLROOT)/lib/intel64 -Wl,--start-group -lmkl_intel_lp64 \
           -lmkl_core -lmkl_intel_thread -Wl,--end-group

...
```
As newer versions of LAMMPS are distributed and changes made to the Makefile, the example Makefile above can be used to generate an updated Makefile using one of the Intel examples packaged with LAMMPS. With the Makefile in place, LAMMPS can be compiled from the lammps-<version>/src directory using the following command.

```
cd lammps-<version>/src
make theta -j 8
```
  
## Running LAMMPS Jobs on Theta
Following is an example executable script “run_lammps.csh” to run LAMMPS on two nodes of Theta with 64 MPI ranks per node. The job can be submitted with command “qsub run_lammps.csh”, where <project_name> is replaced with an active project allocation.
  
```
#!/bin/csh
#COBALT -n 2 -t 10 -q debug-cache-quad -A <project_name> -O LAMMPS

aprun -n 128 -N 64 -d 1 --cc depth -e OMP_NUM_THREADS=1 -j 1 ./lmp_theta -in lmp.in
```
## Performance Notes
When possible, users will want to build LAMMPS executables with the USER-OMP and USER-INTEL packages for best performance on Theta. Following is an example script “run_lammps_intel.csh” to run LAMMPS on two nodes of Theta with 64 MPI ranks per node and two OpenMP threads per rank with the USER-INTEL and USER-OMP packages. The job can be submitted with command “qsub run_lammps_intel.csh.”
```
#!/bin/csh
#COBALT -n 2 -t 10 -q debug-cache-quad -A <project_name> -O LAMMPS

aprun -n 128 -N 64 -d 2 --cc depth -e OMP_NUM_THREADS=2 -j 2 ./lmp_theta -in lmp.in -sf hybrid intel omp
```
Not all available forcefields in LAMMPS are supported in one or both of these packages. For the latest information, please check LAMMPS website and documentation.

## Building on ThetaGPU
There are two key packages available in LAMMPS for running on the GPUs available in ThetaGPU: GPU and KOKKOS. Example Makefiles based on recent version of LAMMPS are available for [download](https://github.com/argonne-lcf/GettingStarted/tree/master/Applications/ThetaGPU/LAMMPS) from ALCF GitHub.

LAMMPS can be built on the ThetaGPU compute nodes with the default software environment and support for the GPU package using the following commands once the Makefiles at the above link are placed appropriately based on instructions in the [README](https://github.com/argonne-lcf/GettingStarted/blob/master/Applications/ThetaGPU/LAMMPS/README.md).
```
cp Makefile.gpu_thetagpu lammps-<version>/lib/gpu
cp Makefile.thetagpu lammps-<version>/src/MAKE/MACHINES
 
cd lammps-<version>/lib/gpu
make -f Makefile.gpu_thetagpu -j 8
cd ../../src
make yes-GPU
make thetagpu -j 8
```
  
LAMMPS can be built with the default software environment and support for the KOKKOS package using the following commands and the Makefile at the above link.
  
```
cp Makefile.thetagpu_kokkos lammps-<version>/src/MAKE/MACHINES
 
cd lammps-<version>/src
make yes-KOKKOS
make thetagpu_kokkos -j 8
```
## Running LAMMPS jobs on ThetaGPU
Following is an example executable script [submit_full-node.sh](https://github.com/argonne-lcf/GettingStarted/blob/master/Applications/ThetaGPU/LAMMPS/submit_full-node.sh) to run LAMMPS a ThetaGPU node using all GPUs for both the GPU and KOKKOS packages. This example is based on the Rhodoposin benchmark using lammps-<version>/bench/in.rhodo.

After the appropriate command is uncommented, the job can be submitted with “qsub ./submit_full-node.sh”, where "-A Catalyst" in the script is replaced with an appropriate active project allocation.

**Note:** The preceding 'qsub' command should be executed 1) on the ThetaGPU login nodes or 2) from the Theta login node after executing 'module load cobalt/cobalt-gpu'.
  
```
#!/bin/sh
#COBALT -n 1 -t 15 -q full-node -A Catalyst
 
# submit job to run on 8 GPUs w/ 8 MPI ranks per GPU and 2 OpenMP threads per rank
#env OMP_NUM_THREADS=2 mpirun -np 64 ~/bin/lammps/lammps-git/src/lmp_thetagpu -in in.rhodo -pk gpu 8 -pk omp 0 -sf hybrid gpu omp
 
# KOKKOS package: submit job to run on 1 GPU w/ 1 MPI ranks per GPU and 2 OpenMP threads per rank
```
#!/bin/sh
#COBALT -n 1 -t 15 -q full-node -A Catalyst
 
# submit job to run on 8 GPUs w/ 8 MPI ranks per GPU and 2 OpenMP threads per rank
#env OMP_NUM_THREADS=2 mpirun -np 64 ~/bin/lammps/lammps-git/src/lmp_thetagpu -in in.rhodo -pk gpu 8 -pk omp 0 -sf hybrid gpu omp
 
# KOKKOS package: submit job to run on 1 GPU w/ 1 MPI ranks per GPU and 2 OpenMP threads per rank
mpirun -np 8 ~/bin/lammps/lammps-git/src/lmp_thetagpu -in in.rhodo -k on g 8 -sf kk -pk kokkos neigh half
```
Additional details on the specific usage of the GPU and KOKKOS packages and how best to use multiple GPUs per node is available in the LAMMPS documentation.
