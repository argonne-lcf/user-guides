# Aurora Example Program Makefile

[//]: * (All GettingStarted references will need correct links added for publishing)

Several simple examples of building CPU and GPU-enabled codes on Aurora are available in the ALCF GettingStarted repo for supported programming models. If building your application on the login node is problematic for some reason (e.g., absence of a GPU), then users are encouraged to build and test applications directly on one of the Aurora compute nodes via an interactive job. The discussion below makes use of the `oneAPI` compilers in the default environment as illustrative examples.

## CPU MPI+OpenMP Example

One of the first useful tasks with any new machine, scheduler, and job launcher is to ensure one is binding MPI ranks and OpenMP threads to the host CPU as intended. A simple HelloWorld MPI+OpenMP example is available here to get started with.

The Aurora compute nodes are dual-socket with 52 physical cores in each socket for a total of 104 cores. As hyperthreading is enabled, each core will show up as two CPUs for a total of 208. In many of the examples below, only a single process is spawned on each physical core.

The application can be straightforwardly compiled using the MPICH compiler wrappers in the default environment.

```bash
mpicxx -g -fopenmp -O3 main.cpp
```

The executable `hello_affinity` can then be launched in a job script (or directly in the shell of an interactive job) using `mpiexec` as discussed here.

```bash
#!/bin/bash -l
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=0:15:00
#PBS -q <queue>
#PBS -A <ProjectName>
#PBS -l filesystems=<fs1:fs2>

#cd ${PBS_O_WORKDIR}

# MPI example w/ MPI ranks and OpenMP threads spread evenly across cores (one process per physical core)
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=26
NDEPTH=4
NTHREADS=4

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} --env OMP_PLACES=cores ./hello_affinity
```

## GPU OpenMP Example

A simple OpenMP offload example is available here. Compilation proceeds similarly to the above CPU-only example except for the use of compiler flags to enable GPU offload.

```bash
mpicxx -fiopenmp -fopenmp-targets=spir64 main.cpp
```

Running the example with 12 MPI ranks and no other settings will generate output like the following.

```bash
$ make

$ mpiexec -n 12 --ppn 12 --depth=1 --cpu-bind depth ./hello_affinity
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 12 RANKS_PER_NODE= 12 THREADS_PER_RANK= 1

  Using OPENMP v5.0
  num_devices=     6
  Default device=  0
  Host=            6
  num_teams=       896
  num_threads=     1
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  list_cores= (0)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  list_cores= (1)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 2  list_cores= (2)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 3  list_cores= (3)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 4  list_cores= (4)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 5  list_cores= (5)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 6  list_cores= (6)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 7  list_cores= (7)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 8  list_cores= (8)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 9  list_cores= (9)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 10  list_cores= (10)  num_devices= 6  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 11  list_cores= (11)  num_devices= 6  gpu_id= 0
```

This simple application does not handle binding of MPI ranks to GPUs, so each of the 12 MPI ranks detects all six GPUs on the node and by default all will select the first GPU listed. The binding of MPI ranks to GPUs can be handled by `mpiexec` in the near future, but for the time being a simple helper script is available for those that need it. There is a centrally installed general `gpu_tile_compact.sh` script available for use, but the examples include the following example script for convenience in case one would like to explore different CPU-GPU bindings (e.g., bind first _N_ MPI ranks to the first GPU).

```bash
$ cat set_affinity_gpu_aurora.sh 
#!/usr/bin/env bash

num_gpu=6
num_tile=2

gpu_id=$(( (PALS_LOCAL_RANKID / num_tile ) % num_gpu ))
tile_id=$((PALS_LOCAL_RANKID % num_tile))

unset EnableWalkerPartition
export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1
export ZE_AFFINITY_MASK=$gpu_id.$tile_id

echo "RANK= ${PALS_RANKID} LOCAL_RANK= ${PALS_LOCAL_RANKID} gpu= ${gpu_id}  tile= ${tile_id}"

#https://stackoverflow.com/a/28099707/7674852
"$@"
```

The `ZE_AFFINITY_MASK` environment variable sets the devices that will be available to the CPU process and can be a comma-separated list of GPUs and/or GPU tiles. Each Aurora GPU consists of two tiles that can be separately bound to CPU processes. This simple script will set `ZE_AFFINITY_MASK` for each MPI rank such that GPU tiles on a node are round-robin assigned.

```bash
$ mpiexec -n 12 --ppn 12 --depth=1 --cpu-bind depth ./set_affinity_gpu_aurora.sh ./hello_affinity
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 12 RANKS_PER_NODE= 12 THREADS_PER_RANK= 1

  Using OPENMP v5.0
  num_devices=     1
  Default device=  0
  Host=            1
  num_teams=       448
  num_threads=     1
To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 0  list_cores= (0)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 1  list_cores= (1)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 2  list_cores= (2)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 3  list_cores= (3)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 4  list_cores= (4)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 5  list_cores= (5)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 6  list_cores= (6)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 7  list_cores= (7)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 8  list_cores= (8)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 9  list_cores= (9)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 10  list_cores= (10)  num_devices= 1  gpu_id= 0

To affinity and beyond!! nname= x1922c1s1b0n0  rnk= 11  list_cores= (11)  num_devices= 1  gpu_id= 0
```

## GPU SYCL Example

A simple SYCL offload example is available here. Compilation proceeds similarly to the above examples except for the compiler flags enabling GPU offload.

```bash
mpicxx -std=c++17 -fsycl -fsycl-targets=spir64 main.cpp
```

Note, this particular example makes use of the Level-Zero API and requires linking with `-lze_loader`, which is not something required of a typical SYCL application. Running the SYCL example using the affinity script binding MPI ranks to individual GPU tiles results in output like the following.

```bash
$ make

$ mpiexec -n 12 --ppn 12 --depth=1 --cpu-bind depth ./set_affinity_gpu_aurora.sh ./hello_affinity

NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 12 RANKS_PER_NODE= 12 THREADS_PER_RANK= 1
COMMAND= mpiexec -n 12 --ppn 12 --depth=1 --cpu-bind depth ./set_affinity_gpu_aurora.sh ./hello_affinity

"RANK= 0 LOCAL_RANK= 0 gpu= 0 tile= 0"
"RANK= 1 LOCAL_RANK= 1 gpu= 0 tile= 1"
"RANK= 2 LOCAL_RANK= 2 gpu= 1 tile= 0"
"RANK= 3 LOCAL_RANK= 3 gpu= 1 tile= 1"
"RANK= 4 LOCAL_RANK= 4 gpu= 2 tile= 0"
"RANK= 5 LOCAL_RANK= 5 gpu= 2 tile= 1"
"RANK= 6 LOCAL_RANK= 6 gpu= 3 tile= 0"
"RANK= 7 LOCAL_RANK= 7 gpu= 3 tile= 1"
"RANK= 8 LOCAL_RANK= 8 gpu= 4 tile= 0"
"RANK= 9 LOCAL_RANK= 9 gpu= 4 tile= 1"
"RANK= 10 LOCAL_RANK= 10 gpu= 5 tile= 0"
"RANK= 11 LOCAL_RANK= 11 gpu= 5 tile= 1"

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 0  list_cores= (0)  num_devices= 1  gpu_uuid=  01000000-0000-0000-dbb1-2f985946b0dd
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 1  list_cores= (1)  num_devices= 1  gpu_uuid=  02000000-0000-0000-dbb1-2f985946b0dd

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 2  list_cores= (2)  num_devices= 1  gpu_uuid=  01000000-0000-0000-9d4c-a3a038130bd2
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 3  list_cores= (3)  num_devices= 1  gpu_uuid=  02000000-0000-0000-9d4c-a3a038130bd2

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 4  list_cores= (4)  num_devices= 1  gpu_uuid=  01000000-0000-0000-f684-455a4554b231
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 5  list_cores= (5)  num_devices= 1  gpu_uuid=  02000000-0000-0000-f684-455a4554b231

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 6  list_cores= (6)  num_devices= 1  gpu_uuid=  01000000-0000-0000-d04a-9a289a53274e
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 7  list_cores= (7)  num_devices= 1  gpu_uuid=  02000000-0000-0000-d04a-9a289a53274e

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 8  list_cores= (8)  num_devices= 1  gpu_uuid=  01000000-0000-0000-a178-e2f3a2a0df2b
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 9  list_cores= (9)  num_devices= 1  gpu_uuid=  02000000-0000-0000-a178-e2f3a2a0df2b

To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 10  list_cores= (10)  num_devices= 1  gpu_uuid=  01000000-0000-0000-1b72-105049dfed26
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 11  list_cores= (11)  num_devices= 1  gpu_uuid=  02000000-0000-0000-1b72-105049dfed26
```

Upon carefully comparing the UUIDs from each rank, one can see the first field distinguishing the 1st or 2nd tile on a GPU and the last two fields distinguishing the 6 GPUs on a compute node. If the affinity script was not used for binding MPI ranks to GPUs, then each MPI rank would report UUIDs for all GPUs like in the following.

```
To affinity and beyond!! nname= x1922c2s6b0n0  rnk= 0  list_cores= (0)  num_devices= 6  gpu_uuid=  00000000-0000-0000-dbb1-2f985946b0dd 00000000-0000-0000-9d4c-a3a038130bd2 00000000-0000-0000-f684-455a4554b231 00000000-0000-0000-d04a-9a289a53274e 00000000-0000-0000-a178-e2f3a2a0df2b 00000000-0000-0000-1b72-105049dfed26
```

## GPU OpenCL Example

A simple OpenCL example is available here. The `include` and `lib` directories for the OpenCL headers and libraries are in the default environment. One simply needs to link the application against `-lOpenCL`.

```bash
mpicxx main.cpp -lOpenCL
```

This simple example can be run on a single tile of an Aurora GPU as follows.

```bash
$ export XE_AFFINITY_MASK=0.0
$ ./vecadd
Running on GPU!
Using double-precision

    CL_DEVICE_NAME: Intel(R) Data Center GPU Max 1550
    CL_DEVICE_VERSION: OpenCL 3.0 NEO 
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 896
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 1600
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```
