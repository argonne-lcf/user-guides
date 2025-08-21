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
- Version of VASP requested (VASP 5, VASP 6.4, VASP 6.5):

## VASP support policy
ALCF compiles the latest release of VASP on a per-request basis. We do not offer support for compiling customized versions of VASP with plugins. We are able to provide Makefiles and step-by-step build instructions to users with a verified VASP license. Support for scientific runs that encounter performance or numerical issues should be directed to the official VASP support mailing list or the VASP user forum. Limited support is available for fatal errors encountered at runtime.

## How to obtain the code
The VASP source can only be obtained from an official license reseller of VASP. This is either the University of Vienna or Material Designs, Inc.

## VASP 6.5.x in Polaris (NVHPC+OpenACC+OpenMP+CUDA math+CrayMPI)

### General compiling/installing instructions provided by VASP support 
Instructions and samples of `makefile.include` can be found on the [`vasp.at` wiki page](https://www.vasp.at/wiki/index.php/Makefile.include#NVIDIA_HPC-SDK_for_CPU_and_GPU).

The following `makefile.include` was tailored for Polaris, originally taken from [here](https://www.vasp.at/wiki/index.php/Makefile.include.nvhpc_omp_acc).

```makefile
# Precompiler options
CPP_OPTIONS = -DHOST=\"LinuxNV\" \
              -DMPI \
              -DMPI_BLOCK=8000 -Duse_collective \
              -DscaLAPACK \
              -DCACHE_SIZE=4000 \
              -Davoidalloc \
              -Dvasp6 \
              -Duse_bse_te \
              -Dtbdyn \
              -Dqd_emulate \
              -Dfock_dblbuf \
              -D_OPENMP \
              -D_OPENACC \
              -DACC_OFFLOAD \
              -DNVCUDA \
              -DUSENCCL

CPP        = nvfortran -Mpreprocess -Mfree -Mextend -E $(CPP_OPTIONS) $*$(FUFFIX)  > $*$(SUFFIX)

FC         = ftn -acc -gpu=cc80 -mp -target-accel=nvidia80
FCL        = ftn -acc -gpu=cc80 -c++libs -target-accel=nvidia80

FREE       = -Mfree

FFLAGS     = -Mbackslash -Mlarge_arrays

OFLAG      = -fast

DEBUG      = -Mfree -O0 -traceback

# Specify your NV HPC-SDK installation, try to set NVROOT automatically
NVROOT     =$(shell which nvfortran | awk -F /compilers/bin/nvfortran '{ print $$1 }')
# ...or set NVROOT manually
NVHPC      ?= /opt/nvidia/hpc_sdk
NVVERSION  = 23.9
#/opt/nvidia/hpc_sdk/Linux_x86_64/24.11/
NVROOT     = $(NVHPC)/Linux_x86_64/$(NVVERSION)

# Use NV HPC-SDK provided BLAS and LAPACK libraries
LIBAOCL=/soft/libraries/aocl/3.2.0
BLAS       = /soft/applications/vasp/aol-libs/3.2/amd-blis/lib/LP64/libblis-mt.a
LAPACK     = /soft/applications/vasp/aol-libs/3.2/amd-libflame/lib/LP64/libflame.a

BLACS      =
SCALAPACK  =

CUDA       = -cudalib=cublas,cusolver,cufft,nccl -cuda

LLIBS      = $(SCALAPACK) $(LAPACK) $(BLAS) $(CUDA)

# Software emulation of quadruple precsion
QD         ?= $(NVROOT)/compilers/extras/qd
LLIBS      += -L$(QD)/lib -lqdmod -lqd
INCS       += -I$(QD)/include/qd


# Use the FFTs from fftw
FFTW       = /soft/applications/vasp/aol-libs/3.2/amd-fftw
LLIBS      += -L$(FFTW)/lib -lfftw3 -lfftw3_omp -lomp
INCS       += -I$(FFTW)/include

#OBJECTS    = fftmpiw.o fftmpi_map.o fftw3d.o fft3dlib.o

# Redefine the standard list of O1 and O2 objects
SOURCE_O1  := pade_fit.o
SOURCE_O2  := pead.o

# For what used to be vasp.5.lib
CPP_LIB    = $(CPP)
FC_LIB     = nvfortran
CC_LIB     = cc
CFLAGS_LIB = -O $(INCS) -c++libs -cuda
FFLAGS_LIB = -O1 -Mfixed
FREE_LIB   = $(FREE)

OBJECTS_LIB= linpack_double.o getshmem.o

# For the parser library
CXX_PARS   = nvc++ --no_warnings

# Normally no need to change this
SRCDIR     = ../../src
BINDIR     = ../../bin
```

#### Setting up compiler and libraries with `module`

The following modules will update the include and library paths used by the Cray compiler wrapper `ftn` to load additional math libraries for the CPU.

```
module restore
module load PrgEnv-nvhpc
module load cray-libsci
module load craype-accel-nvidia80
export NVROOT=${NVIDIA_PATH}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NVROOT/compilers/extras/qd/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-blis/lib/ILP64/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-libflame/lib/ILP64/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-fftw/lib

```

### Compiling VASP
Once the `modules` are loaded and a `makefile.include` is in the `vasp` folder, compiling all the object files and binaries is done with:

``` 
make -j1
```

### Running VASP in Polaris

An example of a submission script can be found here `/soft/applications/vasp/script.sh`, which would look something similar to:

``` example-script.sh
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:eagle
#PBS -q debug
#PBS -A MYPROJECT

module load PrgEnv-nvhpc
module load cray-libsci
module load craype-accel-nvidia80

NVROOT=${NVIDIA_PATH}

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NVROOT/compilers/extras/qd/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-blis/lib/ILP64/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-libflame/lib/ILP64/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/soft/applications/vasp/aol-libs/3.2/amd-fftw/lib

export MPICH_GPU_SUPPORT_ENABLED=1
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=2
NDEPTH=4
NTHREADS=4
NGPUS=2
NTOTRANKS=$(( NNODES * NRANKS ))
# Provide full path to VASP binary
bin=/soft/applications/vasp/vasp.6.4.3/bin/vasp_std

cd $PBS_O_WORKDIR

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS} --depth ${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} $bin
```

Submission scripts should have executable attributes to be used with `qsub` script mode.

```
chmod +x script.sh
qsub script.sh
```
