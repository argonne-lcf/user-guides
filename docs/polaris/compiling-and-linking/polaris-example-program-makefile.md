# Example Programs and Makefiles for Polaris

Several simple examples of building CPU and GPU-enabled codes on Polaris are available in the [ALCF GettingStart repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris) for several programming models. If build your application is problematic for some reason (e.g. absence of a GPU), then users are encouraged to build and test applications directly on one of the Polaris compute nodes via an interactive job. The discussion below makes use of the `NVHPC` compilers in the default environment as illustrative examples. Similar examples for other compilers on Polaris are available in the [ALCF GettingStarted repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris).

## CPU MPI+OpenMP Example

One of the first useful tasks with any new machine, scheduler, and job launcher is to ensure one is binding MPI ranks and OpenMP threads to the host cpu as intended. A simple HelloWorld MPI+OpenMP example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_omp) to get started with.

The application can be straightforwardly compiled using the Cray compiler wrappers.
```
CC -fopenmp main.cpp -o hello_affinity
```

The executable `hello_affinity` can then be launched in a job script (or directly in shell of interactive job) using `mpiexec` as discussed here.

[//]: # (ToDo: Add link to running jobs page)

```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00

# MPI example w/ 16 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=16
NDEPTH=4
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity
```

## CUDA

Several variants of C/C++ and Fortran CUDA examples are available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/CUDA) that include MPI and multi-gpu examples.

One can use the Cray compiler wrappers to compile GPU-enabled applications as well. This [example](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/CUDA/vecadd) of simple vector addition uses the NVIDIA compilers.

```
CC -g -O3 -std=c++0x -cuda main.cpp -o vecadd
```

The `craype-accel-nvidia80` module in the default environment will add the `-gpu` compiler flag for `nvhpc` compilers along with appropriate include directories and libraries. It is left to the user to provide an additional flag to the `nvhpc` compilers to select the target GPU programming model. In this case, `-cuda` is used to indicate compilation of CUDA code. The application can then be launched within a batch job submission script or as follows on one of the compute nodes.

```
$ ./vecadd 
# of devices= 4
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
Running on GPU 0!
Using single-precision

  Name= NVIDIA A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2

Result is CORRECT!! :)
```

## GPU OpenACC

A simple MPI-parallel OpenACC example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/OpenACC/vecadd_mpi). Compilation proceeds similar to the above CUDA example except for the use of the `-acc=gpu` compiler flag to indicate compilation of OpenACC code for GPUs.
```
CC -g -O3 -std=c++0x -acc=gpu -gpu=cc80,cuda11.0 main.cpp -o vecadd
```
In this example, each MPI rank sees all four GPUs on a Polaris node and GPUs are bound to MPI ranks round-robin within the application.

```
$ mpiexec -n 4 ./vecadd
# of devices= 4
Using single-precision

Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!

Result is CORRECT!! :)
```
If the application instead relies on the job launcher to bind MPI ranks to available GPUs, then a small helper script can be used to explicitly set `CUDA_VISIBLE_DEVICES` appropriately for each MPI rank. One example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu) where each MPI rank is similarly bound to a single GPU with round-robin assignment. The binding of MPI ranks to GPUs is discussed in more detail [here](../queueing-and-running-jobs/job-and-queue-scheduling.md#binding-mpi-ranks-to-gpus).

[//]: # (ToDo: Add link to running jobs page)

## GPU OpenCL

A simple OpenCL example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/OpenCL/vecadd). The OpenCL headers and library are available in the NVHPC SDK and cuda toolkits. The environment variable `NVIDIA_PATH` is defined for the `PrgEnv-nvhpc` programming environment. 
```
CC -o vecadd -g -O3 -std=c++0x  -I${NVIDIA_PATH}/cuda/include main.o -L${NVIDIA_PATH}/cuda/lib64 -lOpenCL
```

This simple example can be run on a Polaris compute node as follows.
```
$ ./vecadd
Running on GPU!
Using single-precision

    CL_DEVICE_NAME: NVIDIA A100-SXM4-40GB
    CL_DEVICE_VERSION: OpenCL 3.0 CUDA
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 108
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 1410
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```

## GPU OpenMP

A simple MPI-parallel OpenMP example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/OpenMP/vecadd_mpi). Compilation proceeds similar to the above examples except for use of the `-mp=gpu` compiler flag to indicated compilation of OpenMP code for GPUs.

```
CC -g -O3 -std=c++0x -mp=gpu -gpu=cc80,cuda11.0 -c main.cpp -o vecadd
```

Similar to the OpenACC example above, this code binds MPI ranks to GPUs in a round-robin fashion. 
```
$ mpiexec -n 4 ./vecadd
# of devices= 4
Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!

Result is CORRECT!! :)
```
