# Example Job Scripts

This page contains a small collection of example job scripts users may find useful for submitting their jobs on Theta. Additional information on Cobalt and how to submit these job scripts is available [here](./job-and-queue-scheduling.md). A simple example using a similar script on Polaris is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Theta/compilation/submit.sh).

## CPU MPI Example

The following `submit.sh` example submits a 2-node job to Theta with 64 MPI ranks per node and 1 MPI rank per core. 

```
#!/bin/bash
#COBALT -n 2 
#COBALT -t 30 
#COBALT -A Comp_Perf_Workshop 
#COBALT -q comp_perf_workshop 
#COBALT --attrs mcdram=cache:numa=quad 
#COBALT --attrs filesystems=home,theta-fs0

echo "COBALT_JOBID = " $COBALT_JOBID
echo "COBALT_JOBSIZE (nodes) =" $COBALT_JOBSIZE
echo "COBALT_PARTNAME = " $COBALT_PARTNAME

NNODES=${COBALT_JOBSIZE}
NRANKS_PER_NODE=64
NTHREADS_PER_CORE=1
NDEPTH=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

# option  long version            (explanation)
#
# -n                              "PEs" (ranks)
# -N      --pes-per-node          ranks per node
# -d      --cpus-per-pe           hyperthreads per rank
# -cc     --cpu-binding depth
# -j                              cpus (hyperthreads) per compute unit (core)


aprun -n ${NTOTRANKS} -N ${NRANKS_PER_NODE} -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth ./hellompi
status=$?


echo "Exit status of aprun is: $status"
exit $status
```

Each Theta node has 1 KNL CPU with a total of 64 cores and each core supports 4 threads. The process affinity in this example is setup to map each MPI rank to 1 core. In this example, a special ALCF training event queue was used to illustrate the need to specify `--attrs mcdram=cache:numa=quad` to select the memory mode. Outside of a training event, this small test job would need to be submitted to one of the debug queues (`-q debug-cache-quad` or `-q debug-flat-quad`), where the memory mode is implied. Applications must be launched with `aprun` for them to run on the KNL compute nodes. Information on the use of `aprun` is available via `man aprun`. Some notes on the specific options used in the above example follow.

* `-n ${NTOTRANKS}` : This is specifying the total number of MPI ranks to start as determined by the total number of nodes allocated to the job (`${COBALT_JOBSIZE}`) and number of MPI ranks to launch on each node (`NRANKS_PER_NODE`).
* `-N ${NRANKS_PER_NODE}` : This is specifying the number of MPI ranks to start on each node.
* `-d ${NDEPTH}` : This is specifying how many cores/threads to space MPI ranks apart on each node.
* `-j ${NTHREADS_PER_CORE}` : This is indicating the number of hardware threads that will be active on each core. This value can be 1, 2, or 4.
* `-cc depth` : This is specifying how to bind processes to cores/threads. The `depth` option will set affinity such that MPI ranks are spaced apart by the argument to `-d` iterating over hardware threads specified by `-j`.

## CPU MPI-OpenMP Example

Using the MPI-only job submission example above as a baseline, there are not many additional changes needed to use OpenMP parallelism with an application. In the following 2-node example, 64 MPI ranks will be started on each node. Each MPI rank will be spaced apart by 2 threads as determined by `-d 2` and achieves the same process affinity as in the earlier MPI-only example because of `-j 2` in this example. The number of OpenMP threads is specified by the environment variable `OMP_NUM_THREADS` and passed via the `-e` option to `aprun`. Some simple examples using a similar job submission script on Theta is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Theta/affinity/submit.sh).


```
#!/bin/bash
#COBALT -n 2 
#COBALT -t 30 
#COBALT -A Catalyst
#COBALT -q debug-cache-quad
#COBALT --attrs filesystems=home,theta-fs0

NNODES=${COBALT_JOBSIZE}
NRANKS_PER_NODE=64
NTHREADS_PER_CORE=2
NDEPTH=2

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

aprun -n ${NTOTRANKS} -N ${NRANKS_PER_NODE} -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out
```

Additional discussion and examples of how to specify process affinity on Theta is available in the [Affinity on Theta](./affinity-theta.md) page.

## Bundling Multiple Runs Within a Job

### Running many jobs one after another

The simplest way of bundling many apruns in a script is simply to list one after another. The apruns will run one at a time sequentially. Each aprun can use up to the number of nodes that were requested in the initial qsub. The following script is an example of launching multiple runs within a script, where each aprun requests the same number of nodes.

```
#!/bin/bash
#COBALT -n 2 
#COBALT -t 30 
#COBALT -A Catalyst
#COBALT -q debug-cache-quad
#COBALT --attrs filesystems=home,theta-fs0

NNODES=${COBALT_JOBSIZE}
NRANKS_PER_NODE=64
NTHREADS_PER_CORE=2
NDEPTH=2

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

aprun -n ${NTOTRANKS} -N ${NRANKS_PER_NODE} -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out
aprun -n ${NTOTRANKS} -N ${NRANKS_PER_NODE} -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out
aprun -n ${NTOTRANKS} -N ${NRANKS_PER_NODE} -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out
```
The aprun command blocks until task completion, at which point it exits, providing a convenient way to run multiple short jobs together. In addition, if a subset of nodes is requested, aprun will place jobs on nodes in the script’s reservation until the pool of inactive nodes is exhausted. If the number of nodes requested by an aprun exceeds the number of nodes reserved by the batch scheduler for the job (through the qsub command), that aprun will fail to execute and an error will be returned.

### Running many jobs at the same time

Multiple simultaneous apruns can be launched by backgrounding the aprun commands in the script and then waiting for completion. A short sleep between apruns is recommended to avoid a potential race condition during a large number of aprun starts. As an example, the following script will launch 4 simultaneous apruns, which execute on the compute nodes at the same time. The first aprun listed runs on 2 nodes and the others each run on 1 node. Since the apruns are backgrounded (as denoted by the &), the script must have a `wait` command at the end so that it does not exit before the apruns complete.

```
#!/bin/bash
#COBALT -n 4 
#COBALT -t 30 
#COBALT -A Catalyst
#COBALT -q debug-cache-quad
#COBALT --attrs filesystems=home,theta-fs0

NTHREADS_PER_CORE=2
NDEPTH=2

aprun -n 128 -N 64 -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out &
sleep 1

aprun -n 64 -N 64 -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out &
sleep 1

aprun -n 64 -N 64 -d ${NDEPTH} -j ${NTHREADS_PER_CORE} -cc depth -e OMP_NUM_THREADS=2 ./a.out &
wait
```
Each `aprun` command will launch executables onto separate sets of nodes. It's not currently possible to run multiple `aprun` instances on the same node at the same time.

There is a system limitation of 1,000 simultaneous aprun invocations in a job script. if this limit is hit, you will see the following error.
```
apsched: no more claims allowed for this reservation (max 1000)
```

### Using a Workflow Manager

There are a variety of workflow managers that can assist bundling jobs together. A few are listed below:

* [Balsam](https://balsam.readthedocs.io/en/latest/)

* [Parsl](http://parsl-project.org/) 

* [Scheduler.x](https://github.com/ncsa/Scheduler) 
 
* [Swift](http://swift-lang.org/main/) 