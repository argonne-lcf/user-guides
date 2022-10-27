# Gromacs on Theta

## What is Gromacs?
GROMACS is a versatile package to perform molecular dynamics, i.e. simulate the Newtonian equations of motion for systems with hundreds to millions of particles. It is primarily designed for biochemical molecules like proteins, lipids, and nucleic acids that have a lot of complicated bonded interactions, but since GROMACS is extremely fast at calculating the nonbonded interactions (that usually dominate simulations) many groups are also using it for research on non-biological systems, e.g. polymers.

## Using GROMACS at ALCF
ALCF offers assistance with building binaries and compiling instructions for GROMACS. For questions, contact us at support@alcf.anl.gov.

## Building Gromacs

1. Download latest source code: [http://manual.gromacs.org/documentation/2022.1/download.html](http://manual.gromacs.org/documentation/2022.1/download.html)
2. tar -xzf gromacs-2022.1.tar.gz
3. cd gromacs-2022.1
4. mkdir build
5. module load cmake
6. module swap PrgEnv-intel PrgEnv-gnu/6.0.10
7. 
```
cmake -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC \
      -DBUILD_SHARED_LIBS=OFF -DGMX_BUILD_OWN_FFTW=ON \
      -DCMAKE_INSTALL_PREFIX=/path-to/gromacs-2022.1/build \
      -DGMX_MPI=ON -DGMX_OPENMP=ON -DGMX_CYCLE_SUBCOUNTERS=ON -DGMX_GPU=OFF \
      -DGMX_BUILD_HELP=OFF -DGMX_HWLOC=OFF -DGMX_SIMD=AVX_512_KNL \
      -DGMX_OPENMP_MAX_THREADS=256
```
8. make –j 16
9. make install
10. The installed binary is `build/bin/gmx_mpi`.

## Running Gromacs on Theta
Prebuilt Gromacs binaries can be found in the directory `/soft/applications/gromacs/gromacs_theta`.

A sample qsub script follows.

```
#!/bin/bash
#COBALT -n 1
#COBALT -t 30 
#COBALT -q debug-cache-quad 
#COBALT -project catalyst 
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,theta-fs0

export GMX_MAXBACKUP=-1 

aprun -n64 -N64 --env OMP_NUM_THREADS=2 --cc depth -d 2 -j 2 \
      /soft/applications/gromacs/gromacs_theta/gmx_mpi.2022.1 mdrun \
      -dlb yes -resethway -pin on -v deffnm step5_1 -g test.log
```
 
We strongly suggest that users try combinations of different numbers of nodes, MPI ranks per node, and OMP threads per rank to find the optimal throughput for their particular workload.

The following is a representative benchmark for a system with 30,000 atoms generated on a single Theta node with above aprun command.

| | Core time(sec)| Wall time(sec) | (%) | 
| ----------- | ----------- | ----------- | -- |
| Time | 9016.068 | 70.441 | 12799.4 |

| | ns/day | hour/ns |
| -- | -- | -- |
| Performance | 61.330 | 0.391  |
