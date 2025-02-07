# Compilation w/ Cray Compiler Wrappers

Users are able to build applications on the Polaris login nodes, but may find it convenient to build and test applications on the Polaris compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly submission scripts.

```
$ qsub -I -l select=1,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make -f Makefile.nvhpc clean
$ make -f Makefile.nvhpc

./submit.sh
```

## Multiple MPI Applications on Single Node

There are four sets of CPUs forming NUMA domains on a Polaris node and each set is closest to one GPU. The topology can be viewed using the `nvidia-smi topo -m` command while on a Polaris compute node.

```
$ nvidia-smi topo -m
GPU0	GPU1	GPU2	GPU3	mlx5_0	mlx5_1 CPU Affinity NUMA Affinity
GPU0	 X 	NV4	NV4	NV4	SYS	SYS	24-31,56-63	3
GPU1	NV4	 X 	NV4	NV4	SYS	PHB	16-23,48-55	2
GPU2	NV4	NV4	 X 	NV4	SYS	SYS	8-15,40-47	1
GPU3	NV4	NV4	NV4	 X 	PHB	SYS	0-7,32-39	0
mlx5_0	SYS	SYS	SYS	PHB	 X 	SYS		
mlx5_1	SYS	PHB	SYS	SYS	SYS	 X 		

Legend:

  X    = Self
  SYS  = Connection traversing PCIe as well as the SMP interconnect between NUMA nodes (e.g., QPI/UPI)
  NODE = Connection traversing PCIe as well as the interconnect between PCIe Host Bridges within a NUMA node
  PHB  = Connection traversing PCIe as well as a PCIe Host Bridge (typically the CPU)
  PXB  = Connection traversing multiple PCIe bridges (without traversing the PCIe Host Bridge)
  PIX  = Connection traversing at most a single PCIe bridge
  NV#  = Connection traversing a bonded set of # NVLinks
```

Providing `mpiexec` with a list of CPUs and setting `CUDA_VISIBLE_DEVICES` appropriately is one way to launch multiple applications concurrently on a node with each using a distinct set of CPUs and GPUs. The example script `submit_4x8.sh` launches four separate instances of an application each using one NUMA domain and GPU.

```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 8 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=8
NTHREADS=1

nvidia-smi topo -m

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

export CUDA_VISIBLE_DEVICES=0
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:24:25:26:27:28:29:30:31 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=1
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:16:17:18:19:20:21:22:23 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=2
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:8:9:10:11:12:13:14:15 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=3
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --cpu-bind list:0:1:2:3:4:5:6:7 ./hello_affinity &

wait
```

Note, this example backgrounds each `mpiexec` and waits for them to complete before exiting the script mimicking what one would typically desire in a production scenario. Output from the different executions may be mixed when written to stdout.

## Multiple MPI Applications on Different Nodes

Users can use multiple hostfiles to run different applications across different nodes within a single job. In the example below, the `split` command is used to create several hostfiles containing subsets of nodes allocated to the job. In this example, each application is running on a separate set of nodes, but that is not currently a requirement. In the 4-node example below, each application will run on two nodes. 

```
#!/bin/sh
#PBS -l select=4:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 2 nodes, 4 MPI ranks per node spread evenly across cores
# User must ensure there are enough nodes in job to support all runs
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=4
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --hostfile ${lh} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity &
  sleep 1s
done

wait 
```

## Multiple-Instance Multiple-Node Example

The two example submission scripts above can be combined if a user would like to run multiple applications within and across nodes. The following example script runs four applications in each node binding GPUs to the closest set of 8 CPU cores.

```
#!/bin/bash -l
#PBS -l select=4:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling 
#PBS -A Catalyst
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ multiple runs per batch job
NNODES=`wc -l < $PBS_NODEFILE`

# Settings for each run: 4 runs each with 8 MPI ranks per node spread evenly across cores mapped to closest GPU
NUM_NODES_PER_MPI=1
NRANKS_PER_NODE=8
NTHREADS=1

NTOTRANKS=$(( NUM_NODES_PER_MPI * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} NUM_NODES_PER_MPI= ${NUM_NODES_PER_MPI} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Increase value of suffix-length if more than 99 jobs
split --lines=${NUM_NODES_PER_MPI} --numeric-suffixes=1 --suffix-length=2 $PBS_NODEFILE local_hostfile.

for lh in local_hostfile*
do
  echo "Launching mpiexec w/ ${lh}"
  MPI_ARG="-n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --hostfile ${lh} "

  mpiexec ${MPI_ARG} --cpu-bind list:24:25:26:27:28:29:30:31 -env CUDA_VISIBLE_DEVICES=0 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:16:17:18:19:20:21:22:23 -env CUDA_VISIBLE_DEVICES=1 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:8:9:10:11:12:13:14:15 -env CUDA_VISIBLE_DEVICES=2 ./hello_affinity &

  mpiexec ${MPI_ARG} --cpu-bind list:0:1:2:3:4:5:6:7 -env CUDA_VISIBLE_DEVICES=3 ./hello_affinity &

  sleep 1s
done

wait
``` 
