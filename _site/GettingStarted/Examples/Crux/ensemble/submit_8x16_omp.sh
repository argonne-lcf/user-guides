#!/bin/bash -l
#PBS -l select=1:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# Launch 8 executables separately. Each runs with 4 MPI ranks across a specified subset of cpu cores consisting of a single NUMA domain. Each MPI rank spawns 4 OpenMP threads in the subset of cores.
# Note, this script only works for single-node jobs
#NNODES=`wc -l < $PBS_NODEFILE`
NNODES=1
NUM_RANKS_PER_EXE=4
NUM_THREADS_PER_EXE=4

OMP_ARGS="--env OMP_NUM_THREADS=${NUM_THREADS_PER_EXE} --env OMP_PROC_BIND=spread --env OMP_PLACES=cores"

EXE=./hello_affinity

# Socket 1
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:0-3:4-7:8-11:12-15 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:16-19:20-23:24-27:28-31 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:32-35:36-39:40-43:44-47 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:48-51:52-55:56-59:60-63 ${OMP_ARGS} ${EXE} &

# Socket 2
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:64-67:68-71:72-75:76-79 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:80-83:84-87:88-91:92-95 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:96-99:100-103:104-107:108-111 ${OMP_ARGS} ${EXE} &
mpiexec -n ${NUM_RANKS_PER_EXE} --ppn ${NUM_RANKS_PER_EXE} --cpu-bind list:112-115:116-119:120-123:124-127 ${OMP_ARGS} ${EXE} &

wait
