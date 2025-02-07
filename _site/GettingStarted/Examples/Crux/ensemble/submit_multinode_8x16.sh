#!/bin/bash -l
#PBS -l select=4:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 8 runs each with 16 MPI ranks per node spread evenly across specified subset of cores
# actual # of ranks run is 2 to reduce output in this simple example
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=2
#NRANKS_PER_NODE=16
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --hostfile ${lh} "
  OMP_ARG="--env OMP_NUM_THREADS=${NTHREADS} "

  # Socket 0
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:16:17:18:19:20:21:22:23:24:25:26:27:28:29:30:31 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:32:33:34:35:36:37:38:39:40:41:42:43:44:45:46:47 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:48:49:50:51:52:53:54:55:56:57:58:59:60:61:62:63 ./hello_affinity &

  # Socket 1
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:64:65:66:67:68:69:70:71:72:73:74:75:76:77:78:79 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:80:81:82:83:84:85:86:87:88:89:90:91:92:93:94:95 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:96:97:98:99:100:101:102:103:104:105:106:107:108:109:110:111 ./hello_affinity &
  mpiexec ${MPI_ARG} ${OMP_ARG} --cpu-bind list:112:113:114:115:116:117:118:119:120:121:122:123:124:125:126:127 ./hello_affinity &

  sleep 1s
done

wait

rm -f local_hostfile.*
