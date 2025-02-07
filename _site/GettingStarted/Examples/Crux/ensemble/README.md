# Compilation w/ Cray Compiler Wrappers

Users are able to build applications on the Crux login nodes, but may find it convenient to build and test applications on the Crux compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly submission scripts.

```
$ qsub -I -l select=1,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make clean
$ make

./submit.sh
```

## Multiple MPI Applications on Single Node

Providing instances of `mpiexec` with separate lists of CPUs is one way to launch multiple applications concurrently on a node with each using a distinct set of CPUs. The example script `submit_4x32.sh` launches four separate instances of an application each using one NUMA domain.

```
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

```

Note, this example backgrounds each `mpiexec` and waits for them to complete before exiting the script mimicking what one would typically desire in a production scenario. Output from the different executions will likely be mixed when written to stdout. In this simple example, adding additional `wait` commands between `mpiexec` calls can help to make sense of the output.

A similar example for OpenMP-enabled applications is available in `submit_4x32_omp.sh`.

## Multiple MPI Applications on Different Nodes

Users can use multiple hostfiles to run different applications across different nodes within a single job. In the example below, the `split` command is used to create several hostfiles containing subsets of nodes allocated to the job. In this example, each application is running on a separate set of nodes, but that is not currently a requirement. In the 4-node example below, each application will run on two nodes. 

```
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

# Settings for each run: 2 nodes, 4 MPI ranks per node spread evenly across cores (underutilized on purpose in simple example)
# User must ensure there are enough nodes in job to support all concurrent runs
NUM_NODES_PER_MPI=2
NRANKS_PER_NODE=4
NDEPTH=32
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARG="--env OMP_NUM_THREADS=${NTHREADS} "

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  mpiexec ${MPI_ARG} ${OMP_ARG} --hostfile ${lh} ./hello_affinity &
  sleep 1s
done

wait

rm -f local_hostfile.*
```

## Multiple-Instance Multiple-Node Example

The two example submission scripts above can be combined if a user would like to run multiple applications within and across nodes. The following example script runs eight applications in each node binding processes to NUMA domains.

```
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
```