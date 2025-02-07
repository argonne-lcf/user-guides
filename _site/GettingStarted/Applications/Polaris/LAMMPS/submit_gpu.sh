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

EXE=/home/knight/bin/lmp_polaris_gnu
EXE_ARG="-in in.rhodo -pk gpu ${NGPUS} -pk omp 0 -sf hybrid gpu omp"

# more MPI ranks per GPU likely needed for performance

MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores"

COMMAND="mpiexec ${MPI_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
