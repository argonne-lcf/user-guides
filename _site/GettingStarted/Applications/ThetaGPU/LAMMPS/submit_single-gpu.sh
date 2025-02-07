#!/bin/bash -l
#COBALT -n 1 -t 15 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

# GPU package: submit job to run on 1 GPU w/ 8 MPI ranks per GPU and 2 OpenMP threads per rank
#env OMP_NUM_THREADS=2 mpirun -np 8 ~/bin/lammps/lammps-git/src/lmp_thetagpu -in in.rhodo -pk gpu 1 -pk omp 0 -sf hybrid gpu omp

# KOKKOS package: submit job to run on 1 GPU w/ 1 MPI ranks per GPU and 2 OpenMP threads per rank
mpirun -np 1 ~/bin/lammps/lammps-git/src/lmp_thetagpu -in in.rhodo -k on g 1 -sf kk -pk kokkos neigh half
