#!/bin/bash -l
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -q EarlyAppAccess
#PBS -A Aurora_deployment

cd ${PBS_O_WORKDIR}

# MPI example w/ 12 MPI ranks per node each with access to single GPU tile
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12
NDEPTH=1
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

AFFINITY=""
AFFINITY="../../../../HelperScripts/Aurora/set_affinity_gpu_aurora.sh"

COMMAND="mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ${AFFINITY} ./hello_affinity"
echo "COMMAND= ${COMMAND}"
${COMMAND}
