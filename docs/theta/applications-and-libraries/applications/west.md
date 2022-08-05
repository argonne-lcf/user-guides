# WEST on Theta
## What Is WEST?
WEST is a Fortran/MPI scalable parallel implementation of large-scale electronic structure calculations within many-body perturbation theory. WEST is currently interfaced with Quantum Espresso planewave DFT software. As described on the WEST website [http://west-code.org](http://west-code.org/), WEST is highly scalable and is used for calculations of solids, liquids, nanostructures, molecules, and interfaces, including samples with ~2000 electrons.

## Using WEST at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. Some provided executables on Theta are available here: /soft/applications/qe_west/qe_v6.1.0-west_3.1.0. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to Obtain the Code
WEST is an open-source code that can be downloaded at [http://west-code.org](http://west-code.org/). Similarly, the Quantum Espresso code can be downloaded at [https://www.quantum-espresso.org](https://www.quantum-espresso.org/). Check the WEST website for current information on supported algorithms.

## Building on Theta
WEST currently requires a working PW executable from Quantum Espresso. Current information on installation details for both WEST and PW can be found at [http://west-code.org/documentation.php](http://west-code.org/documentation.php). After Quantum Espresso and WEST codes have been downloaded and unpacked, the PW and WEST executables can be compiled using the following script in the qe-<version> directory.

  
```
cat build_theta.sh
#!/bin/bash

export BLAS_LIBS="-L$MKLROOT/intel64/lib -Wl,--start-group -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -Wl,--end-group"
export SCALAPACK_LIBS="-L$MKLROOT/intel64/lib -Wl,--start-group -lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64 -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -Wl,--end-group"
export FFT_LIBS=""
export MPIF90="ftn -g -mkl"
export CC="cc -g -mkl"
export F77="ftn -g -mkl"
export FFLAGS="-xMIC-AVX512 -align array64byte -fp-model fast=2 -no-prec-div -assume byterecl"

./install/configure --host=x86_64-build-linux-gnu --build=x86_64-target-linux-gnu --enable-parallel --with-scalapack --enable-openmp

make pw -j 16

cd West
make
```
  
As newer versions of Quantum Espresso and WEST are released, check the corresponding websites for current information.

  
```
> cat run_west.sh
#!/bin/bash
#COBALT -n 2 -t 10 -q debug-cache-quad -A <project_name> -O WEST

aprun -n 128 -N 64 -d 1 --cc depth -e OMP_NUM_THREADS=1 -j 1 ./wstat wstat.i
> qsub run_west.sh
```
  
## Running WEST Jobs on Theta
The following is an example executable script “run_west.sh” to run the wstat WEST executable on two nodes of Theta with 64 MPI ranks per node. The job can be submitted with command “qsub run_west.sh” where <project_name> is replaced with an active project allocation.  
