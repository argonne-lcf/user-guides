# Gromacs on Theta

## What is Gromacs?
GROMACS is a versatile package to perform molecular dynamics, i.e. simulate the Newtonian equations of motion for systems with hundreds to millions of particles. It is primarily designed for biochemical molecules like proteins, lipids, and nucleic acids that have a lot of complicated bonded interactions, but since GROMACS is extremely fast at calculating the nonbonded interactions (that usually dominate simulations) many groups are also using it for research on non-biological systems, e.g. polymers.

## Using GROMACS at ALCF
ALCF offers assistance with building binaries and compiling instructions for GROMACS. For questions, contact us at support@alcf.anl.gov.

## Building Gromacs

1. Download latest source code: http://manual.gromacs.org/documentation/2018.5/download.html
2. Tar xzf&amp;nbsp;gromacs-2018.5.tar.gz
3. cd gromacs-2018.5
4. Mkdir build
5. cd ../
6. Make sure use Intel compiler suite
7. ```CmakeCC=cc CXX=CC CFLAGS=” -xMIC-AVX512 -g -static-intel” 
   CXXFLAGS=” -xMIC-AVX512 -g -static-intel” -
   DBUILD_SHARED_LIBS=OFF -DGMX_FFT_LIBRARY=mkl
   DCMAKE_INSTALL_PREFIX=/PATH-to-Gromacs-Build-directory/ -
   DGMX_MPI=ON DGMX_OPENMP=ON -DGMX_CYCLE_SUBCOUNTERS=ON -DGMX_GPU=OFF 
   DGMX_BUILD_HELP=OFF -DGMX_HWLOC=OFF 
   -DGMX_SIMD=AVX_512_KNL 
   DGMX_OPENMP_MAX_THREADS=256```
8. Make –j 16
9. Make install
10. cd build
11. The Gromacs binary is exe/gmx_mpi

## Running Gromacs
A prebuilt Gromacs binary can be found in directory /soft/applications/gromacs

A sample qsub script:

```
#!/bin/bash
#COBALT -t 30 
#COBALT --project catalyst 
#COBALT --attrs mcdram=cache:numa=quad 
#COBALT -n 1& 
#COBALT -q debug-cache-quad 
#export  I_MPI_DEBUG=5 
#export I_MPI_FABRICS=shm 
#export I_MPI_PIN_MODE=lib 
#export KMP_AFFINITY=verbose,compact,1 
exportGMX_MAXBACKUP=-1 

aprun -n64-N64--envOMP_NUM_THREADS=2-ccdepth -d2-j2
/lus/theta-fs0/home/user_name/gromacs-2018.5/build/bin/gmx_mpi\
 mdrun -dlbyes-resethway-pinon -v-deffnmstep5_1 -gtest.log
 ```
 
We strongly suggest that users try combinations of different numbers of nodes, MPI ranks per node, and OMP threads per rank to find the optimal throughput for the application.

This is a benchmark for a 30,000 atoms system generated on 1 single Theta node with above qprun:

|   | Core time(sec)| Wall time(sec) |
| ----------- | ----------- | ----------- |
| Time | 8497.278(ns/day) | 12800.0(hour/ns) |
| Performance | 65.078 | 0.369	 |

 
 
