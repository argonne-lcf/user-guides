# VASP

## What is VASP?
The Vienna Ab initio Simulation Package (VASP) is a software package for performing electronic structure calculations with periodic boundary conditions. It is most commonly used to perform density functional theory (DFT) calculations in a plane wave basis using the projector augmented wave (PAW) method. A more complete description of VASP can be found here: [https://www.vasp.at](https://www.vasp.at)

## Using VASP at ALCF
VASP is commercial software. Access to binaries compiled by ALCF can only be granted after the user requesting access has been verified to be on the VASP license by an official VASP license distributor.

To access the VASP binary at ALCF, please email the details listed directly below to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). It can take up to 5–10 business days to verify a VASP license.

Information to provide:

- User’s full name:
- User’s ALCF username:
- Name of the organization that purchased the VASP license:
- VASP license purchased from (University of Vienna or MaterialsDesign):
- Principal investigator who is the POC for the VASP license:
- VASP license number:
- Version of VASP requested (VASP 6.x):

## VASP support policy
ALCF compiles the latest release of VASP on a per-request basis. We do not offer support for compiling customized versions of VASP with plugins. Support for scientific runs that encounter performance or numerical issues should be directed to the official VASP support mailing list or the VASP user forum. Limited support is available for fatal errors encountered at runtime.

Once the user licence is validated, they will be added to the UNIX groups: `vasp6` or `vasp65` , and get access to the subdirectories in `/soft/applications/vasp`.

## How to obtain the code
The VASP source can only be obtained from an official license reseller of VASP. This is either the University of Vienna or Material Designs, Inc.

## VASP 6.6.x in Aurora (oneAPI + OpenMP Offload + MPI)

### General compiling/installing instructions provided by VASP support 
Instructions and samples of `makefile.include` can be found on the [`vasp.at` wiki page](https://www.vasp.at/wiki/index.php/Makefile.include#Intel_Composer_suite_and_oneAPI_HPC_toolkit_for_CPU_and_GPU).

The following `makefile.include` was tailored for Polaris, originally taken from [here](https://www.vasp.at/wiki/Makefile.include.oneapi_omp_off).

```makefile title="makefile.include" linenums="1"
# Precompiler options
CPP_OPTIONS = -DHOST=\"LinuxIFC\" \
              -DMPI -DMPI_BLOCK=8000 -Duse_collective \
              -DscaLAPACK \
              -DCACHE_SIZE=4000 \
              -Davoidalloc \
              -Dvasp6 \
              -Dtbdyn \
              -Dfock_dblbuf \
              -D_OPENMP \
              -DINTELMKL

CPP         = fpp -f_com=no -free -w0  $*$(FUFFIX) $*$(SUFFIX) $(CPP_OPTIONS)

FC          = mpif90 -fc=ifx -fiopenmp
FCL         = mpif90 -fc=ifx
FREE        = -free -names lowercase

FFLAGS      = -assume byterecl -w

OFLAG       = -O2
OFLAG_IN    = $(OFLAG)
DEBUG       = -O0

# For what used to be vasp.5.lib
CPP_LIB     = $(CPP)
FC_LIB      = $(FC)
CC_LIB      = icx
CFLAGS_LIB  = -O
FFLAGS_LIB  = -O1
FREE_LIB    = $(FREE)

OBJECTS_LIB = linpack_double.o

# For the parser library
CXX_PARS    = icpx
LLIBS       = -lstdc++

LIB        += oneapi
LLIB       += -Loneapi -lZeInterface

# OMP offloading
CPP_OPTIONS+= -DOMP_OFFLOAD
# this enables offloading plus AoT compiling of target device code
FFLAGS     += -fopenmp-targets=spir64_gen -Xs "-device pvc"


# Workarounds for problems with fpp and ifx
FFLAGS     += -switch no-use-host-usm-for-implicit-reduction-map
IFX_BUILD_DATE = $(shell ifx --version | grep ifx | awk '{print $$4}')
CPP_OPTIONS+= -D__INTEL_COMPILER_BUILD_DATE=$(IFX_BUILD_DATE)

##
## Customize as of this point! Of course you may change the preceding
## part of this file as well if you like, but it should rarely be
## necessary ...
##

# When compiling on the target machine itself, change this to the
# relevant target when cross-compiling for another architecture
VASP_TARGET_CPU ?= -xHOST
FFLAGS     += $(VASP_TARGET_CPU)

# Intel MKL (FFTW, BLAS, LAPACK, and scaLAPACK)
FCL        += -qmkl -fiopenmp -fopenmp-targets=spir64_gen -Xs "-device pvc" -fopenmp-device-code-split=per_kernel -fopenmp-max-parallel-link-jobs=8 -lze_loader
MKLROOT    ?= /path/to/your/mkl/installation
LLIBS      += -L$(MKLROOT)/lib/intel64 -lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64 -lmkl_sycl
INCS        =-I$(MKLROOT)/include -I$(MKLROOT)/include/fftw

# HDF5-support (optional but strongly recommended, and mandatory for some features)
#CPP_OPTIONS+= -DVASP_HDF5
#HDF5_ROOT  ?= /path/to/your/hdf5/installation
#LLIBS      += -L$(HDF5_ROOT)/lib -lhdf5_fortran
#INCS       += -I$(HDF5_ROOT)/include

# For the VASP-2-Wannier90 interface (optional)
#CPP_OPTIONS    += -DVASP2WANNIER90
#WANNIER90_ROOT ?= /path/to/your/wannier90/installation
#LLIBS          += -L$(WANNIER90_ROOT)/lib -lwannier

# For the fftlib library (hardly any benefit in combination with MKL's FFTs)
#FCL         = mpiifort fftlib.o -qmkl
#CXX_FFTLIB  = icpc -qopenmp -std=c++11 -DFFTLIB_USE_MKL -DFFTLIB_THREADSAFE
#INCS_FFTLIB = -I./include -I$(MKLROOT)/include/fftw
#LIBS       += fftlib

# For machine learning library VASPml (experimental)
#CPP_OPTIONS += -Dlibvaspml
#CXX_ML       = mpiicpc -cxx=icpx -qopenmp
#CXXFLAGS_ML  = -O3 -std=c++17 -Wall
#INCLUDE_ML   =
## This may be required for the C++17 filesystem library if the underlying
## system compiler is older than GNU 9.1. For newer versions this can be removed.
#LLIBS       += -lstdc++fs


```

#### Setting up compiler and libraries with `module`

The following modules will update the include and library paths used by the Cray compiler wrapper `ftn` to load additional math libraries for the CPU.

```bash linenums="1"
module restore
module load oneapi/release/2025.3.1

```

### Compiling VASP
Once the `modules` are loaded and a `makefile.include` is in the `vasp` folder, compiling all the object files and binaries is done with:

``` 
make -j1
```

### Running VASP in Polaris

An example of a submission script can be found here `/soft/applications/vasp/example.script.sh`, which would look something similar to:

```bash linenums="1" title="script.sh"
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -l filesystems=<fs1:fs2>
#PBS -q debug-scaling
#PBS -A <MYPROJECT>

export TZ='/usr/share/zoneinfo/US/Central'
cd ${PBS_O_WORKDIR}

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=12 # Number of MPI ranks to spawn per node
NDEPTH=4 # Number of hardware threads per rank (i.e. spacing between MPI ranks)
NTHREADS=4 # Number of software threads per rank to launch (i.e. OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS} THREADS_PER_RANK= ${NTHREADS}"

MPIR_CVAR_ENABLE_GPU=1 #enables GPU-aware MPI support 

bin=/soft/applications/vasp/vasp.6.6.0/bin/vasp_std

mpiexec -n ${NTOTRANKS} -ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth -env OMP_NUM_THREADS=${NTHREADS} --env OMP_PLACES=cores /soft/applications/vasp/gpu_tile_compact.sh $bin
```

Submission scripts should have executable attributes to be used with `qsub` script mode.

```bash
chmod +x script.sh
qsub script.sh
```
