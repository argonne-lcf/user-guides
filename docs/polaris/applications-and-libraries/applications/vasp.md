
## VASP 6.x.x in Polaris (NVHPC+OpenACC+OpenMP+CUDA math+CrayMPI)

VASP is a commercial code for materials and solid state simulations. Users must have a license to use this code in ALCF systems. More information on how to get access to VASP binaries can be found [here](https://www.alcf.anl.gov/support-center/theta/vasp).

### General compiling/installing instructions provided by VASP support 
Instructions and samples of `makefile.include` could be found in [`vasp.at` wiki page](https://www.vasp.at/wiki/index.php/Makefile.include#NVIDIA_HPC-SDK_for_CPU_and_GPU)

The follow `makefile.include` was tailored for Polaris, originally taken [from here](https://www.vasp.at/wiki/index.php/Makefile.include.nvhpc_omp_acc)

```makefile
# makefile.inclide
# Precompiler options

CPP_OPTIONS = -DHOST=\"LinuxNV\" \
              -DMPI -DMPI_BLOCK=8000 -Duse_collective \
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
              -DUSENCCL -DUSENCCLP2P

CPP        = nvfortran -Mpreprocess -Mfree -Mextend -E $(CPP_OPTIONS) $*$(FUFFIX)  > $*$(SUFFIX)

FC         = ftn  -acc -target-accel=nvidia80 -mp
FCL        = ftn   -acc -target-accel=nvidia80 -mp    -c++libs

FREE       = -Mfree

FFLAGS     = -Mbackslash -Mlarge_arrays

OFLAG      = -fast

DEBUG      = -Mfree -O0 -traceback


# Use NV HPC-SDK provided BLAS and LAPACK libraries
BLAS       = -lblas
LAPACK     = -llapack

# provided by cray-scilib
BLACS      =
#SCALAPACK  = -Mscalapack

CUDA       = -cudalib=cublas,cusolver,cufft,nccl -cuda


LLIBS      = $(SCALAPACK) $(LAPACK) $(BLAS) $(CUDA)

# Software emulation of quadruple precsion
#NVROOT = /opt/nvidia/hpc_sdk/Linux_x86_64/21.
NVROOT = /opt/nvidia/hpc_sdk/Linux_x86_64/22.3
QD         ?= $(NVROOT)/compilers/extras/qd
LLIBS      += -L$(QD)/lib -lqdmod -lqd
INCS       += -I$(QD)/include/qd


# Use the FFTs from fftw
# provided by cray-fftw
#FFTW       ?= 
#LLIBS      += 
#INCS       += 


OBJECTS    = fftmpiw.o fftmpi_map.o fftw3d.o fft3dlib.o

# Redefine the standard list of O1 and O2 objects
SOURCE_O1  := pade_fit.o
SOURCE_O2  := pead.o

# For what used to be vasp.5.lib
CPP_LIB    = $(CPP)
FC_LIB     = nvfortran
CC_LIB     = nvc
CFLAGS_LIB = -O
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

The follow modules would integrate into `ftn` compiler the libraries and path to headers provided by Cray.
```
module purge
module add PrgEnv-nvhpc
module add cray-libsci/21.08.1.2
module add cray-fftw/3.3.8.13

```

### Compiling vasp
Once the `modules` are loaded and a `makefile.include` is in `vasp` folder, compiling all the object files and binaries is done with:
```
make -j1
```

### Running VASP in Polaris


`example-script.sh`

```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:15:00

module purge
module add PrgEnv-nvhpc cray-fftw cray-libsci

export MPICH_GPU_SUPPORT_ENABLED=1
NNODES=1
NRANKS=4
NDEPTH=8
NTHREADS=1
NGPUS=4
NTOTRANKS=$(( NNODES * NRANKS ))

aprin -n ${NTOTRANKS} -N ${NRANKS} -d ${NDEPTH} -e OMP_NUM_THREADS=${NTHREADS} /bin/vasp_std
```

Submission script should have executable attibutes to be used with `qsub` script mode.
```
chmod +x example-script.sh
qsub  example-script.sh
```

### Known issues
---

* Undefined `MPIX_Query_cuda_support` function at linking binary* 
This function is called in `src/openacc.F`. The  `MPIX_Query_cuda_support` is not included in`cray-mpich`. One turn around to this
issue is to comment this function call.
See the follow suggested changes marked by `!!!!!CHANGE HERE ` in the `file:src/openacc.F`

```fortran
!!!!!CHANGE HERE 
!      INTERFACE
!        INTEGER(c_int) FUNCTION MPIX_Query_cuda_support() BIND(C, name="MPIX_Query_cuda_support")
!        END FUNCTION
!      END INTERFACE

       CHARACTER(LEN=1) :: ENVVAR_VALUE
       INTEGER :: ENVVAR_STAT

       ! This should tell us if MPI is CUDA-aware
!!!!!CHANGE HERE 
       !CUDA_AWARE_SUPPORT = MPIX_Query_cuda_support() == 1
       CUDA_AWARE_SUPPORT = .TRUE.
       ! However, for OpenMPI some env variables can still deactivate it even though the previous
       ! check was positive
       CALL GET_ENVIRONMENT_VARIABLE("OMPI_MCA_mpi_cuda_support", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0 .AND. ENVVAR_VALUE=='0') CUDA_AWARE_SUPPORT = .FALSE.
       CALL GET_ENVIRONMENT_VARIABLE("OMPI_MCA_opal_cuda_support", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0 .AND. ENVVAR_VALUE=='0') CUDA_AWARE_SUPPORT = .FALSE.
       ! Just in case we might be non-OpenMPI, and their MPIX_Query_cuda_support behaves similarly
       CALL GET_ENVIRONMENT_VARIABLE("MV2_USE_CUDA", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0 .AND. ENVVAR_VALUE=='0') CUDA_AWARE_SUPPORT = .FALSE.
       CALL GET_ENVIRONMENT_VARIABLE("MPICH_RDMA_ENABLED_CUDA", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0 .AND. ENVVAR_VALUE=='0') CUDA_AWARE_SUPPORT = .FALSE.
       CALL GET_ENVIRONMENT_VARIABLE("PMPI_GPU_AWARE", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0) CUDA_AWARE_SUPPORT =(ENVVAR_VALUE == '1')
!!!!!CHANGE HERE 
       CALL GET_ENVIRONMENT_VARIABLE("MPICH_GPU_SUPPORT_ENABLED", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
       IF (ENVVAR_STAT==0) CUDA_AWARE_SUPPORT =(ENVVAR_VALUE == '1')
```

