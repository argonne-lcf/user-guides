#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

. setup_env.sh

# MPI example w/ 4 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

EXE=julia

EXE_ARGS="--check-bounds=no --project main.jl"

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth "

COMMAND="mpiexec ${MPI_ARGS} ${EXE} ${EXE_ARGS}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
