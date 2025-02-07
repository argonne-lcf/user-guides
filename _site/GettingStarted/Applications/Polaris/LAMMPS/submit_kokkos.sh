#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -q debug
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

. ./setup_lammps_gnu.sh

export MPICH_GPU_SUPPORT_ENABLED=1

NNODES=`wc -l < $PBS_NODEFILE`

# per-node settings
NRANKS=4
NRANKSSOCKET=2
NDEPTH=8
NTHREADS=1
NGPUS=4

NTOTRANKS=$(( NNODES * NRANKS ))

EXE=/home/knight/bin/lmp_polaris_gnu_kokkos
EXE_ARG="-in in.reaxc.hns -k on g ${NGPUS} -sf kk -pk kokkos neigh half neigh/qeq full newton on "

# OMP settings mostly to quiet Kokkos messages

MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores"

COMMAND="mpiexec ${MPI_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
