# NWChem
NWChem is a parallel quantum chemistry mainly written in Fortran77 and that uses MPI and OpenMP for distributed and multicore computing. NWChem was designed to solve large-scale  electronic structure calculations with Hartree-Fock, Density Functional Theory, and other wavefunction correlated methods. See full features at [http://www.nwchem-sw.org](http://www.nwchem-sw.org).

## Using NWChem at ALCF
ALCF provides binaries and compiling instructions for NWChem. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to Obtain the Code
NWChem is an open-source code the official web site is [http://www.nwchem-sw.org](http://www.nwchem-sw.org). Change log and release announcements could be found here [http://www.nwchem-sw.org/index.php/Download](http://www.nwchem-sw.org/index.php/Download).

Official NWChem source could be downloaded from the project’s github.com repository [https://github.com/nwchemgit/nwchem/releases](https://github.com/nwchemgit/nwchem/releases). For detailed compiling options, please visit the wiki page of NWchem, [https://github.com/nwchemgit/nwchem/wiki/Compiling-NWChem](https://github.com/nwchemgit/nwchem/wiki/Compiling-NWChem). The follow instructions were tested in for the version 6.8.1.

```
cat nwchem_envs_681.sh

export NWCHEM_TOP=/projects/nwchem/nwchem-6.8.1

export NWCHEM_TARGET=LINUX64

export USE_MPI=y

export USE_CPPRESERVE=y

export NWCHEM_MODULES="all"

export USE_MPI=y

export USE_MPIF=y

export USE_MPIF4=y

export USE_OPENMP=1

export USE_KNL=1

export USE_OPENMP=y

export  INTEL_64ALIGN=1

export USE_NOIO=1

export USE_GAGITHUB=1

export CRAYPE_LINK_TYPE=dynamic

export export ARMCI_NETWORK=MPI_TS

export BLAS_SIZE=4

export BLASOPT="  -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -lpthread -lm -ldl"

export SCALAPACK=" -lmkl_scalapack_lp64 -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -lmkl_blacs_intelmpi_lp64 -lpthread -liomp5  -ldl"

export BLAS_LIB=$BLASOPT

export LAPACK_LIB=$BLASOPT

export SCALAPACK_SIZE=4

export USE_KNL=1

export USE_64TO32=1

cd  $NWCHEM_TOP/src

make nwchem_config

make 64_to_32

make
```

Alternatively, binaries can be found in the folders under /soft/applications/nwchem

## Running Jobs on Theta

The following script “submit.sh” is an example to run Nwchem in Theta a job of 8 nodes with 64 MPI ranks per node. The job can be submitted with command "qsub submit.sh"

```
cat submit.sh

#!/bin/bash

#COBALT -n 8

#COBALT -t 30

#COBALT -A myproject

module load atp

bin=/soft/applications/nwchem/6.8/bin/nwchem
echo "Running Cobalt Job $COBALT_JOBID."

rpn=64

df -k

export MPICH_GNI_MAX_EAGER_MSG_SIZE=16384 

export MPICH_GNI_MAX_VSHORT_MSG_SIZE=10000 

export MPICH_GNI_MAX_EAGER_MSG_SIZE=131072 

export MPICH_GNI_NUM_BUFS=300 

export MPICH_GNI_NDREG_MAXSIZE=16777216 

export MPICH_GNI_MBOX_PLACEMENT=nic 

export MPICH_GNI_LMT_PATH=disabled 

export COMEX_MAX_NB_OUTSTANDING=6

export LD_LIBRARY_PATH=/opt/intel/compilers_and_libraries_2018.0.128/linux/compiler/lib/intel64_lin

aprun  -n $((COBALT_JOBSIZE*rpn)) -cc depth -d1 -j1  $bin test.nw

qsub submit.sh
```
