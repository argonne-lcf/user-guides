#!/bin/bash -l
#PBS -l select=4:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 1 nodes, 4 MPI ranks per node spread evenly across cores
# User must ensure there are enough nodes in job to support all concurrent runs
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --hostfile ${lh} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity & 
  sleep 1s
done

wait
