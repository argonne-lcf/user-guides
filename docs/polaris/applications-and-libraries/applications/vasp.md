
## VASP 6.x.x in Polaris (NVHPC+OpenACC+OpenMP+CUDA math+CrayMPI)

The Vienna Ab initio Simulation Package (VASP) is a software package for performing electronic structure calculations with periodic boundary conditions. It is most commonly used that to perform density functional theory (DFT) calculations in a planewave basis using the projector augemented wave (PAW) method. A more complete description of VASP can be found [here](https://www.vasp.at).

Users must have a license to use this code on ALCF systems. More information on how to get access to VASP binaries can be found [here](https://www.alcf.anl.gov/support-center/theta/vasp).

### General compiling/installing instructions provided by VASP support 
Instructions and samples of `makefile.include` could be found in the [`vasp.at` wiki page](https://www.vasp.at/wiki/index.php/Makefile.include#NVIDIA_HPC-SDK_for_CPU_and_GPU).

The follow `makefile.include` was tailored for Polaris, originally taken from [here](https://www.vasp.at/wiki/index.php/Makefile.include.nvhpc_omp_acc).

```makefile
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
              -DUSENCCL -DUSENCCLP2P\

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
NVVERSION  = 20.9
#NVROOT     = $(NVHPC)/Linux_x86_64/$(NVVERSION)

# Use NV HPC-SDK provided BLAS and LAPACK libraries
LIBAOCL=/soft/libraries/aocl/3.2.0
BLAS       = ${LIBAOCL}/lib/libblis-mt.a
LAPACK     = ${LIBAOCL}/lib/libflame.a

BLACS      =
SCALAPACK  =
#SCALAPACK  = -Mscalapack
#SCALAPACK  = ${LIBAOCL}/lib/libscalapack.a

CUDA       = -cudalib=cublas,cusolver,cufft,nccl -cuda

LLIBS      = $(SCALAPACK) $(LAPACK) $(BLAS) $(CUDA)

# Software emulation of quadruple precsion
QD         ?= $(NVROOT)/compilers/extras/qd
LLIBS      += -L$(QD)/lib -lqdmod -lqd
INCS       += -I$(QD)/include/qd

#INCS       += -I/usr/include/linux 
#INCS       += -I/usr/include/c++/7/tr1 
#INCS       += -I/usr/include/c++/7 
#INCS       += -I/usr/include/x86_64-linux-gnu/c++/7
#INCS       += -I/lus/theta-fs0/software/spack/spack-dev/opt/spack/linux-sles15-x86_64/gcc-9.3.0/gcc-10.2.0-r7v3naxd5xgzzaqxoe73jj2ytwuddamr/lib/gcc/x86_64-pc-linux-gnu/10.2.0/include/

# Use the FFTs from fftw
FFTW       ?= ${LIBAOCL}
LLIBS      += -L$(FFTW)/lib -lfftw3 -lfftw3_omp -lomp
#INCS       += -I/soft/libraries/aocl/3.2.0/include_LP64/
INCS       += -I$(FFTW)/include

OBJECTS    = fftmpiw.o fftmpi_map.o fftw3d.o fft3dlib.o

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
#CXX_PARS   = nvc++ --no_warnings -I/lus/theta-fs0/software/spack/spack-dev/opt/spack/linux-sles15-x86_64/gcc-9.3.0/gcc-10.2.0-r7v3naxd5xgzzaqxoe73jj2ytwuddamr/include/c++/10.2.0/ -I/lus/theta-fs0/software/spack/spack-dev/opt/spack/linux-s
les15-x86_64/gcc-9.3.0/gcc-10.2.0-r7v3naxd5xgzzaqxoe73jj2ytwuddamr/include/c++/10.2.0/x86_64-pc-linux-gnu -I/lus/theta-fs0/software/spack/spack-dev/opt/spack/linux-sles15-x86_64/gcc-9.3.0/gcc-10.2.0-r7v3naxd5xgzzaqxoe73jj2ytwuddamr/lib/gcc
/x86_64-pc-linux-gnu/10.2.0/include -I/lus/theta-fs0/software/spack/spack-dev/opt/spack/linux-sles15-x86_64/gcc-9.3.0/gcc-10.2.0-r7v3naxd5xgzzaqxoe73jj2ytwuddamr/lib/gcc/x86_64-pc-linux-gnu/10.2.0/include-fixed/
CXX_PARS   = nvc++ --no_warnings 

# Normally no need to change this
SRCDIR     = ../../src
BINDIR     = ../../bin
```

#### Setting up compiler and libraries with `module`

The follow modules will update the include and libraries paths used by the Cray compiler wrapper `ftn` to load additional math libraries for the CPU.

```
module purge
module load PrgEnv-nvhpc
module load cray-libsci
module load craype-accel-nvidia8

```

### Compiling VASP
Once the `modules` are loaded and a `makefile.include` is in the `vasp` folder, compiling all the object files and binaries is done with:

``` 
make -j1
```

### Running VASP in Polaris

An example of a submission script could be found here `/soft/applications/vasp/submit-polaris2023-2.sh` , which would looks something similar to:


``` example-script.sh
#!/bin/sh
#PBS -l select=1:system=polaris  
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:grand:eagle
#PBS -q debug
#PBS -A Catalyst

module load PrgEnv-nvhpc
module load cray-libsci

export MPICH_GPU_SUPPORT_ENABLED=1
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=2
NDEPTH=4
NTHREADS=4
NGPUS=2
NTOTRANKS=$(( NNODES * NRANKS ))

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS} --depth ${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} /path_to_vasp/bin/vasp_std
```

Submission scripts should have executable attibutes to be used with `qsub` script mode.

```
chmod +x example-script.sh
qsub  example-script.sh
```

### Known issues versions: >= 6.4.0 in Polaris 
---

* Undefined `MPIX_Query_cuda_support` function at linking binary: This function is called in `src/openacc.F`. The  `MPIX_Query_cuda_support` is not included in`cray-mpich`. One workaround to this
issue is to comment this function call.
See the follow suggested changes marked by `!!!!!CHANGE HERE ` in the `file:src/openacc.F`

```fortran
+!!!!!CHANGE HERE 
-      INTERFACE
-        INTEGER(c_int) FUNCTION MPIX_Query_cuda_support() BIND(C, name="MPIX_Query_cuda_support")
-        END FUNCTION
-      END INTERFACE

       CHARACTER(LEN=1) :: ENVVAR_VALUE
       INTEGER :: ENVVAR_STAT

       ! This should tell us if MPI is CUDA-aware
+!!!!!CHANGE HERE 
-       CUDA_AWARE_SUPPORT = MPIX_Query_cuda_support() == 1
+       CUDA_AWARE_SUPPORT = .TRUE.
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
+!!!!!CHANGE HERE 
+       CALL GET_ENVIRONMENT_VARIABLE("MPICH_GPU_SUPPORT_ENABLED", ENVVAR_VALUE, STATUS=ENVVAR_STAT)
+       IF (ENVVAR_STAT==0) CUDA_AWARE_SUPPORT =(ENVVAR_VALUE == '1')
```

