# Quantum ESPRESSO on Theta
## What Is Quantum ESPRESSO?
Quantum ESPRESSO (QE) is a suite of codes for electronic structure calculations and materials research. This code uses Density Functional Theory calculations with periodic boundary conditions to estimate energies, forces, and other properties of atomic scale systems. QE runs in parallel (MPI and OpenMP) and is based on plane waves basis functions and pseudopotentials. QE is an open-source project at [http://www.quantum-espresso.org/](http://www.quantum-espresso.org/).

## How to access Quantum ESPRESSO
Prebuilt QE binaries are provided on Theta under /soft/applications/quantum_espresso. The binaries for v5.3.0 and v6.2.1 are dynamically linked and require certain version of libraries to function. Please read the README file and load necessary modules. The binaries for v6.3 and beyond are statically linked and there is no dependance on loaded modules.

## Using Quantum ESPRESSO at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](support@alcf.anl.gov).

If building your own QE is necessary, follow the instructions below.

## Building on Theta
This recipe is verified on Oct 12th 2018 with QE-6.3.

QE uses autotools to create a make.inc file required to compile the code. First, obtain a copy of source code on the Theta login nodes.

We suggest compiling the code with Intel Fortran compiler (under Cray wrapper) and using Intel MKL for both linear algebra and FFT. Due to the conflict of MKL and libsci, libsci needs to be unloaded. Enabling wavefunction IO via hdf5 library is optional.

```
module unload cray-libsci
module load cray-hdf5-parallel 
export CRAYPE_LINK_TYPE=dynamic
```
Inside the source code directory, QE can be configured with the follow command in a shell terminal:
```
./configure \
 --prefix=/home/myhome/mypath \
 MPIF90=ftn CC=cc --enable-openmp --with-scalapack=intel --with-hdf5=/opt/cray/pe/hdf5-parallel/1.10.1.1/INTEL/16.0 \
 --with-elpa-include=/soft/applications/elpa/elpa-2017.11.001/include/elpa-2017.11.001/modules \
 --with-elpa-lib=/soft/applications/elpa/elpa-2017.11.001/lib/libelpa.a
```
 Now proceed to build the code. Use "make" and install it with "make install".

If statically linked binaries are needed, do the following before hitting "make":

```
export CRAYPE_LINK_TYPE=static
```
Edit make.inc by replacing
```
-lmkl_scalapack_lp64 -lmkl_blacs_intelmpi_lp64
```
in SCALAPACK_LIBS with
```
${MKLROOT}/lib/intel64/libmkl_scalapack_lp64.a -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_blacs_intelmpi_lp64.a -Wl,--end-group
```

## Running QE Jobs on Theta
Below is an example of a submission script of running the binary pw.x of QE in Theta:

```
#!/bin/bash
#COBALT -n 8
#COBALT -t 10
#COBALT -q default
#COBALT -A my_project
#COBALT -O my_test

PROCS_PERNODE=16
HT=1
PROCS=$((COBALT_PARTSIZE * PROCS_PERNODE))
NTHREADS=$((64 * HT / PROCS_PERNODE))

pw=/soft/applications/quantum_espresso/6.3/bin/pw.x

echo "Running Cobalt Job $COBALT_JOBID."

aprun -n $PROCS -N $PROCS_PERNODE -cc depth -d $NTHREADS -j $HT $pw -in ./test.in &> ./out
```

This script file can be submitted as ‘qsub script.sh’, assuming you have a ‘test.in’ file in place.

## References
[QE User Manual](http://www.quantum-espresso.org/resources/users-manual)

