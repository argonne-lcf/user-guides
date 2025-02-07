#!/bin/bash -l
#PBS -l select=8:system=sophia
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q workq 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

# Example using all 8 GPUs and 128 cores (1 thread per core)
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=1
NDEPTH=128
NTHREADS=128

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

export OMP_NUM_THREADS=${NTHREADS}
export OMP_PLACES=cores
./hello_affinity

#echo "Affinitying using cpu-bind depth"
#mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity
