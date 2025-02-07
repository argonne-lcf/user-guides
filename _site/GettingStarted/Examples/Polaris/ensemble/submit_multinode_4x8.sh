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

# Settings for each run: 4 runs each with 8 MPI ranks per node spread evenly across cores mapped to closest GPU
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=8
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --hostfile ${lh} "

  mpiexec ${MPI_ARG} --cpu-bind list:24:25:26:27:28:29:30:31 -env CUDA_VISIBLE_DEVICES=0 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:16:17:18:19:20:21:22:23 -env CUDA_VISIBLE_DEVICES=1 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:8:9:10:11:12:13:14:15 -env CUDA_VISIBLE_DEVICES=2 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:0:1:2:3:4:5:6:7 -env CUDA_VISIBLE_DEVICES=3 ./hello_affinity &

  sleep 1s
done

wait
