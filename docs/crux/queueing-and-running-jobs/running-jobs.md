---
description: "Run jobs on Crux: queues, interactive jobs, MPI+OpenMP examples, and ensemble launches."
---

# Running Jobs on Crux

## Queues: debug, workq-route, preemptable, demand {#queues}

There are four production queues you can target in your qsub (`-q <queue>`):

| Queue Name | Node Min | Node Max | Time Min | Time Max | Notes |
| --- | --- | --- | --- | --- | --- |
| debug | 1 | 4 | 5 min | 2 hr | Max 1 job running and 1 job queued per user; at most 8 nodes in use across all debug jobs |
| workq-route | 1 | 184 | 5 min | 24 hrs | Routing queue; 100 jobs max per project; see below for its execution queue |
| *preemptable* | 1 | 10 | 5 min | 72 hrs | ***Jobs in the preemptable queue can be killed at any time if jobs are submitted to the demand queue.*** Max 20 jobs queued and 10 running per project; see **Note** below |
| *demand* | 1 | 64 | 5 min | 1 hr | ***By request only (email support@alcf.anl.gov)***; max 100 jobs queued per project |

**Note:** Jobs in the preemptable queue can be killed without warning when demand jobs arrive. To view queue limits, run `qstat -Qf <queue>`.

To make your job rerunnable after preemption, add the PBS directive `#PBS -r y`. The scheduler will restart the job once the demand job completes.

`workq-route` is a routing queue that forwards jobs to the following execution queue:

| Queue Name | Node Min | Node Max | Time Min | Time Max | Notes |
| --- | --- | --- | --- | --- | --- |
| workq | 1 | 176 | 5 min | 24 hrs | 20 jobs queued or running, 10 jobs running per project |

Run `qstat -Qf workq` and `qstat -Qf workq-route` for the current limits; `qstat -Q` lists all queues.

## Interactive Jobs on Compute Nodes

To build or test on a compute node, request an interactive shell. For example, to get one node for an hour in the debug queue:

```bash
qsub -I -l select=1 -l walltime=01:00:00 -l filesystems=home:eagle -q debug -A <project>
```

After the scheduler assigns a node, your shell prompt will be on the compute node where you can compile and test. See [Compiling and Linking](../compiling-and-linking/compiling-and-linking-overview.md) for compiler details.

## Running MPI+OpenMP Applications

/// warning | Set `OMP_NUM_THREADS` explicitly
    attrs: {class: inline end}

The default `OMP_NUM_THREADS` on Crux compute nodes is 256 (one per hardware thread). Always set it to the value your application intends, or you may oversubscribe the node.
///

Once a job is running, launch MPI applications with `mpiexec`. Documentation is available via `man mpiexec`; common options are:

* `-n` total number of MPI ranks
* `--ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of CPUs per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script is below for a 4-node job with 64 MPI ranks per node and 2 OpenMP threads per rank. Each of the two 64-core CPUs hosts 32 ranks, and each thread is bound to a single core.

You can download and compile `hello_affinity` from the [GettingStarted repository](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/affinity_omp) (`make` in that directory).

```bash linenums="1" title="affinity_job.sh"
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4:system=crux
#PBS -l place=scatter
#PBS -l walltime=00:10:00
#PBS -q debug
#PBS -A <project>
#PBS -l filesystems=home:eagle

# MPI+OpenMP example: 64 MPI ranks per node, 2 threads per rank
# Two 64-core CPUs per node → 32 ranks per CPU, each bound to a core

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=64 # Number of MPI ranks to spawn per node
NDEPTH=2 # Number of hardware threads per rank (spacing between MPI ranks)
NTHREADS=2 # Number of OpenMP threads per rank (OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

cd $PBS_O_WORKDIR

MPI_ARGS="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth "
OMP_ARGS="--env OMP_NUM_THREADS=${NTHREADS} --env OMP_PROC_BIND=true --env OMP_PLACES=cores "

mpiexec ${MPI_ARGS} ${OMP_ARGS} ./hello_affinity
```

The [`hello_affinity`](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/affinity_omp) program is a compiled C++ code built via `make` in the linked directory after cloning the [GettingStarted](https://github.com/argonne-lcf/GettingStarted) repository.

## Running Multiple MPI Applications on a Single Node

Multiple applications can be run simultaneously on a node by launching several `mpiexec` commands and backgrounding them. For performance, ensure each application runs on a distinct set of CPU resources. Provide a list of CPUs via `--cpu-bind` to assign resources. Output from `numactl --hardware` (see [Machine Overview](../index.md)) is useful for localizing applications within NUMA domains.

In the example below, eight instances of the application run on a single node, each localized to a single NUMA domain (16 cores). Each instance spawns 16 MPI ranks on distinct cores without hyperthreads:

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

When running across many nodes, do not rely on the default hostfile (`$PBS_NODEFILE`) for all invocations, or every `mpiexec` will use the same nodes. Instead, split the hostfile into per-application host lists.

The following example splits `$PBS_NODEFILE` into one-node hostfiles and launches eight NUMA-local instances on each node:

```bash linenums="1" title="ensemble.sh"
# MPI example with multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 8 runs, each with 16 MPI ranks per node
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=16
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*; do
  echo "Launching mpiexec with ${lh}"
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

Ensemble [examples](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/ensemble) for several cases are provided to help with crafting job submission scripts.

## Compute Node Access to the Internet

Compute nodes access the internet via a proxy. Set the following in your job environment if your application needs outbound connectivity:

```bash
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export HTTP_PROXY="http://proxy.alcf.anl.gov:3128"
export HTTPS_PROXY="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
export no_proxy="admin,localhost,*.cm.crux.alcf.anl.gov,*.alcf.anl.gov"
```

See [Getting Started](../getting-started.md#proxy) for the `~/.bash_profile` snippet.

