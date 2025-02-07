#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 8 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NUM_RANKS_PER_EXE=2
NUM_THREADS_PER_EXE=4

OMP_ARGS="--env OMP_NUM_THREADS=${NUM_THREADS_PER_EXE} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores"

EXE=./hello_affinity
#EXE=../affinity_omp/hello_affinity

nvidia-smi topo -m

# In this example, we should really have MPS enabled because multiple MPI ranks targeting same GPU.

# Enable MPS on each node allocated to job
#mpiexec -n ${NNODES} --ppn 1 ../mps/enable_mps_polaris.sh

export CUDA_VISIBLE_DEVICES=0
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:24-27:28-31 ${OMP_ARGS} ${EXE} &

export CUDA_VISIBLE_DEVICES=1
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:16-19:20-23 ${OMP_ARGS} ${EXE} &

export CUDA_VISIBLE_DEVICES=2
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:8-11:12-15 ${OMP_ARGS} ${EXE} &

export CUDA_VISIBLE_DEVICES=3
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:0-3:4-7 ${OMP_ARGS} ${EXE} &

wait

# Disable MPS on each node allocated to job
#mpiexec -n ${NNODES} --ppn 1 ../mps/disable_mps_polaris.sh
