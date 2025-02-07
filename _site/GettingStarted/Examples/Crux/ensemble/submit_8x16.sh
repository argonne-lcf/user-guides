#!/bin/bash -l
#PBS -l select=1:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# Launch 8 executables separately. Each runs with 16 MPI ranks across a specified subset of cpu cores consisting of a single NUMA domain.
# Note, this script only works for single-node jobs
#NNODES=`wc -l < $PBS_NODEFILE`
NNODES=1
NRANKS_PER_NODE=16
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

OMP_ARGS="--env OMP_NUM_THREADS=${NTHREADS} "

# Socket 0
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:16:17:18:19:20:21:22:23:24:25:26:27:28:29:30:31 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:32:33:34:35:36:37:38:39:40:41:42:43:44:45:46:47 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:48:49:50:51:52:53:54:55:56:57:58:59:60:61:62:63 ${OMP_ARGS} ./hello_affinity & 
wait

# Socket 1
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:64:65:66:67:68:69:70:71:72:73:74:75:76:77:78:79 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:80:81:82:83:84:85:86:87:88:89:90:91:92:93:94:95 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:96:97:98:99:100:101:102:103:104:105:106:107:108:109:110:111 ${OMP_ARGS} ./hello_affinity & 
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:112:113:114:115:116:117:118:119:120:121:122:123:124:125:126:127 ${OMP_ARGS} ./hello_affinity & 

wait
