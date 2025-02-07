#!/bin/sh
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -q workq
#PBS -A Catalyst
#PBS -l filesystems=home

#cd ${PBS_O_WORKDIR}

# MPI example w/ MPI ranks and OpenMP threads spread evenly across cores (one process per physical core)
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# 6 MPI ranks per socket, spread evenly across cores skipping 1 core on each socket
# Each MPI rank has access to 8 cores and corresponding hyperthreads (i.e. up to 16 OpenMP threads)
# The cpu list repeats for additional MPI ranks (i.e. a 13th MPI rank would bind to cpus 1-8,105-112)
SOCKET1="1-8,105-112:9-16,113-120:17-24,121-128:25-32,129-136:33-40,137-144:41-48,145-152"
SOCKET2="53-60,157-164:61-68,165-172:69-76,173-180:77-84,181-188:85-92,189-196:93-100,197-204"

MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} "
MPI_ARG+="--cpu-bind=list:${SOCKET1}:${SOCKET2}"

OMP_ARG=""
OMP_ARG="--env OMP_NUM_THREADS=${NTHREADS} --env OMP_PLACES=cores"

COMMAND="mpiexec ${MPI_ARG} ${OMP_ARG} ./hello_affinity"
echo "COMMAND= ${COMMAND}"
${COMMAND}
