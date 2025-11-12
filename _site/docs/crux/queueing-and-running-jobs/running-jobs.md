# Running Jobs on Crux

## Queues

*******

There are four production queues you can target in your qsub (`-q <queue name>`):

| Queue Name    | Node Min | Node Max | Time Min | Time Max | Notes                                                                                                |
|---------------|----------|----------|----------|----------|------------------------------------------------------------------------------------------------------|
| debug         | 1        | 8        | 5 min    | 2 hr     | 8 nodes are exclusive 
| workq-route   | 1        | 184      | 5 min    | 24 hrs   | Routing queue; 100 jobs max per project; See below for its execution queue
| *preemptable*   | 1        | 10       | 5 min    | 72 hrs   | ***Please be aware that jobs in the preemptable queue can be killed at any time if jobs are submitted to the demand queue.*** Max 20 jobs running/accruing/queued **per-project**; see **Note** below                              |
| *demand*        | 1        | 64       | 5 min    | 1 hr     | ***By request only (email support@alcf.anl.gov) to submit a request***              |

******
**Note:** Please be aware that jobs in the preemptable queue can be killed at any time if jobs are submitted to the demand queue.
Jobs in the demand queue take priority over jobs in the preemptable queue.
This means jobs in the preemptable queue may be preempted (killed without any warning) if there are jobs in the demand queue.
Unfortunately, there's always an inherent risk of jobs being killed when using the preemptable queue. 
Please use the following command to view details of a queue: `qstat -Qf <queuename>`

To make your job rerunnable, add the following PBS directive: `#PBS -r y`. This will ensure your job will restart once the demand job is complete. 

`workq-route` is a routing queue and routes your job to the following execution queue:

| Queue Name      | Node Min | Node Max | Time Min | Time Max | Notes                                  |
|-----------------|----------|----------|----------|----------|----------------------------------------|
| workq           | 1        | 184      | 5 min    | 24 hrs   | 20 jobs queue or running/10 jobs running per project |

## Running MPI+OpenMP Applications

**Note:** For OpenMP-enabled applications, it is extremely important to set the number of OpenMP threads to an appropriate value. As on most systems, the default value for `OMP_NUM_THREADS` is set to the maximum possible, which is 256 on the Crux compute nodes.

Once a submitted job is running, calculations can be launched on the compute nodes using `mpiexec` to start an MPI application. Documentation is accessible via `man mpiexec`, and some helpful options follow.

* `-n` total number of MPI ranks
* `-ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of CPUs per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script with directives is below for a 4-node job with 8 MPI ranks on each node and 8 OpenMP threads per rank. Each hardware thread runs a single OpenMP thread since there are 64 hardware threads on the CPU (2 per core).
You can download and compile `hello_affinity` from this [link](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/affinity).

```bash
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -q debug
#PBS -A Catalyst  # Replace with your project
#PBS -l filesystems=home:eagle

# MPI+OpenMP example w/ 64 MPI ranks per node and threads spread evenly across cores
# There are two 32-core CPUs on each node. This will run 32 MPI ranks per CPU, 2 OpenMP threads per rank, and each thread bound to a single core.

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=64 # Number of MPI ranks to spawn per node
NDEPTH=2 # Number of hardware threads per rank (i.e. spacing between MPI ranks)
NTHREADS=2 # Number of software threads per rank to launch (i.e. OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Change the directory to work directory, which is the directory you submit the job.
cd $PBS_O_WORKDIR

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARGS="--env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=true --env OMP_PLACES=cores "

mpiexec ${MPI_ARGS} ${OMP_ARGS} ./hello_affinity
```

The [`hello_affinity`](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/affinity_omp) program is a compiled C++ code, which is built via `make clean ; make` in the linked directory after cloning the [Getting Started](https://github.com/argonne-lcf/GettingStarted) repository.

## Running Multiple MPI Applications on a Single Node

Multiple applications can be run simultaneously on a node by launching several `mpiexec` commands and backgrounding them. For performance, it will likely be necessary to ensure that each application runs on a distinct set of CPU resources. One can provide a list of CPUs using the `--cpu-bind` option to explicitly assign CPU resources on a node to each application. Output from the `numactl --hardware` command is useful for understanding how to localize applications within NUMA domains on the two CPUs of each node.

In the example below, eight instances of the application are simultaneously running on a single node, with each application localized to a single NUMA domain. Each application here is bound to 16 CPU cores with a single process running on each core (i.e. no hyperthreads). In the first instance, the application is spawning 16 MPI ranks on cores 0-15 in the first CPU.

```bash
  MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE}"
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

wait
```

## Running Multiple MPI Applications on Multiple Nodes

An important detail missing from the prior example was specifying the hostfile. When not specified, the default hostfile ${PBS_NODEFILE} is used for all invocations of `mpiexec`, meaning all applications will include identical sets of nodes. This is fine for single-node jobs, but appropriate hostfiles need to be created and passed to `mpiexec` when running applications across subsets of nodes in a large job.

The following example first splits the hostfile ${PBS_NODEFILE} into separate hostfiles each containing the requested number of nodes (in this case just 1 per file). The separate hostfiles are then used in each batch of `mpiexec` calls to launch applications on different compute nodes.

```bash
# MPI example w/ multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 8 runs each with 16 MPI ranks per node spread evenly across specified subset of cores
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=16
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

Ensemble [`examples`](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/ensemble) for several cases are provided to help users with crafting job submission scripts.

## Compute Node Access to the Internet

Currently, the only access to the internet is via a proxy. Here are the proxy environment variables for Crux:

```bash
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
```
