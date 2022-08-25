# Example Job Scripts

This page contains a small collection of example job scripts users may find useful for submitting their jobs on Polaris. Additional information on PBS and how to submit these job scripts is available [here](./job-and-queue-scheduling.md). A simple example using a similar script on Polaris is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_omp).

## CPU MPI-OpenMP Example

The following `submit.sh` example submits a 1-node job to Polaris with 16 MPI ranks per node and 2 OpenMP threads per rank. 

```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A Catalyst

# Change to working directory
cd ${PBS_O_WORKDIR}

# MPI and OpenMP settings
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=16
NDEPTH=2
NTHREADS=2

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} -env OMP_PLACES=threads ./hello_affinity
```

Each Polaris node has 1 Milan CPU with a total of 32 cores and each core supports 2 threads. The process affinity in this example is setup to map each MPI rank to 2 cores utilizing all one thread on each core. The OpenMP settings bind each OpenMP thread to one hardware thread within a core such that all 32 cores are utilized. Some additional notes on the contents of the script before the `mpiexec` command follow.

* `cd ${PBS_O_WORKDIR}` : change into the working directory from where `qsub` was executed.
* ``NNODES= `wc -l < $PBS_NODEFILE` ``: one method for determine the total number of nodes allocated to a job.
* `NRANKS_PER_NODE=16` : This is a helper variable to set the number of MPI ranks for each node to 16.
* `NDEPTH=2` : This is a helper variable to space MPI ranks 2 "slots" from each other. In this example, individual threads correspond to a slot. This will be used together with the `--cpu-bind` option from `mpiexec` and additional binding options are available (e.g. numa).
* `NTHREADS=2` : This is a helper variable to set the number of OpenMP threads per MPI rank. 
* `NTOTRANKS=$(( NNODES * NRANKS_PER_NODE))` : This is a helper variable calculating the total number of MPI ranks spanning all nodes in the job.

Information on the use of `mpiexec` is available via `man mpiexec`. Some notes on the specific options used in the above example follow.

* `-n ${NTOTRANKS}` : This is specifying the total number of MPI ranks to start.
* `--ppn ${NRANKS_PER_NODE}` : This is specifying the number of MPI ranks to start on each node.
* `--depth=${NDEPTH}` : This is specifying how many cores/threads to space MPI ranks apart on each node.
* `--cpu bind depth` : This is indicating the number of cores/threads will be bound to MPI ranks based on the `depth` argument.
* `--env OMP_NUM_THREADS=${NTHREADS}` : This is setting the environment variable `OMP_NUM_THREADS` : to determine the number of OpenMP threads per MPI rank.
* `--env OMP_PLACES=threads` : This is indicating how OpenMP should distribute threads across the resource, in this case across hardware threads.

## GPU MPI Example

Using the CPU job submission example above as a baseline, there are not many additional changes needed to enable an application to make use of the 4 NVIDIA A100 GPUs on each Polaris node. In the following 2-node example (because `#PBS -l select=2` indicates the number of nodes requested), 4 MPI ranks will be started on each node assigning 1 MPI rank to each GPU in a round-robin fashion. A simple example using a similar job submission script on Polaris is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu).


```
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A Catalyst

# Enable GPU-MPI (if supported by application)
export MPICH_GPU_SUPPORT_ENABLED=1

# Change to working directory
cd ${PBS_O_WORKDIR}

# MPI and OpenMP settings
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# For applications that internally handle binding MPI/OpenMP processes to GPUs
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} -env OMP_PLACES=threads ./hello_affinity

# For applications that need mpiexec to bind MPI ranks to GPUs
#mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} -env OMP_PLACES=threads ./set_affinity_gpu_polaris.sh ./hello_affinity
```

The OpenMP-related options are not needed if your application does not use OpenMP. Nothing additional is required on the `mpiexec` command for applications that internally manage GPU devices and handle the binding of MPI/OpenMP processes to GPUs. A small helper script is available for those with applications that rely on MPI to handle the binding of MPI ranks to GPUs. Some notes on this helper script and other key differences with the early CPU example follow.

* `export MPICH_GPU_SUPPORT_ENABLED=1` : For applications that support GPU-enabled MPI (i.e. use MPI to communicate data directly between GPUs), this environment variable is required to enable GPU support in Cray's MPICH. Ommitting this will result in a segfault. Support for this also requires that the application was linked against the the GPU Transport Layer library (e.g. -lmpi_gtl_cuda), which is automatically included for users by the `craype-accel-nvidia80` module in the default environment on Polaris. If this gtl library is not properly linked, then users will see a error message indicating that upon executing the first MPI command that uses a device pointer.

* `./set_affinity_gpu_polaris.sh` : This script is useful for those applications that rely on MPI to bind MPI ranks to GPUs on each node. Such a script is not necessary when the application handles process-gpu binding. This script simply sets the environment variable `CUDA_VISIBLE_DEVICES` to a restricted set of GPUs (e.g. each MPI rank sees only one GPU). Otherwise, users would find that all MPI ranks on a node will target the first GPU likely having a negative impact on performance. An example for this script is available in the [Getting Started repo](https://github.com/argonne-lcf/GettingStarted/blob/master/Examples/Polaris/affinity_gpu/set_affinity_gpu_polaris.sh) and copied below.

### Setting MPI-GPU affinity

The `CUDA_VISIBLE_DEVICES` environment variable is provided for users to set which GPUs on a node are accessible to an application or MPI ranks started on a node.

A copy of the small helper script provided in the [Getting Started repo](https://github.com/argonne-lcf/GettingStarted/blob/master/Examples/Polaris/affinity_gpu/set_affinity_gpu_polaris.sh) is provided below for reference.

```
$ cat ./set_affinity_gpu_polaris.sh
#!/bin/bash
num_gpus=4
gpu=$((${PMI_LOCAL_RANK} % ${num_gpus}))
export CUDA_VISIBLE_DEVICES=$gpu
echo “RANK= ${PMI_RANK} LOCAL_RANK= ${PMI_LOCAL_RANK} gpu= ${gpu}”
exec "$@"
```
The script is hard-coded for 4 GPUs on a Polaris node and simply pairs MPI ranks to GPUs in a round-robin fashion setting `CUDA_VISIBLE_DEVICES` appropriately. The `echo` command prints a helpful message for the user to confirm the desired mapping is achieved. Users are encouraged to edit this file as necessary for their particular needs. 

* IMPORTANT: If planning large-scale runs with many thousands of MPI ranks, then it is advised to comment out the `echo` command so as not to have thousands of lines of output written to stdout. 

### Using MPS on the GPUs

Documentation for the Nvidia Multi-Process Service (MPS) can be found [here](https://docs.nvidia.com/deploy/mps/index.html)

In the script below, note that if you are going to run this as a multi-node job you will need to do this on every compute node and you will need to ensure that the paths you specify for `CUDA_MPS_PIPE_DIRECTORY` and `CUDA_MPS_LOG_DIRECTORY`do not "collide" and end up with all the nodes writing to the same place.  The local SSDs or /dev/shm or incorporation of the node name into the path would all be possible ways of dealing with that issue.
```
#!/bin/bash
export CUDA_MPS_PIPE_DIRECTORY=</path/writeable/by/you>
export CUDA_MPS_LOG_DIRECTORY=</path/writeable/by/you>
CUDA_VISIBLE_DEVICES=0,1,2,3 nvidia-cuda-mps-control -d
echo "start_server -uid $( id -u )" | nvidia-cuda-mps-control
```  
to verify the control service is running:

`nvidia-smi | grep -B1 -A15 Processes`

and the output should look similar to this:

```
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|    0   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    1   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    2   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    3   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
+-----------------------------------------------------------------------------+
```

to shut down the service:

`echo "quit" | nvidia-cuda-mps-control`

to verify the service shut down properly:

`nvidia-smi | grep -B1 -A15 Processes`

and the output should look like this:

```
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+
```


