# Example Programs and Makefiles for Polaris

Several simple examples of building CPU and GPU-enabled codes on Polaris are available in the [ALCF GettingStart repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris) for several programming models. It is currently recommended to build and test applications directly on one of the Polaris compute nodes via an interactive job. The discussion below makes use of the NVIDIA compilers as illustrative examples. Similar examples for the other compilers on Polaris are available in the [ALCF GettingStarted repo](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris).

## CPU MPI+OpenMP Example

One of the first useful tasks with any new machine, scheduler, and job launcher is to ensure one is binding MPI ranks and OpenMP threads to the host cpu as intended. A simple HelloWorld MPI+OpenMP example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_omp) to get started with.

The application can be straightforwardly compiled using the Cray compiler wrappers.
```
CC -fopenmp main.cpp -o hello_affinity
```

The executable `hello_affinity` can then be launch in a job script (or directly in shell of interactive job) using `mpiexec` as discussed here.

[//]: # (ToDo: Add link to running jobs page)

```
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
module load craype-accel-nvidia80
CC -g -O3 -std=c++0x -cuda main.cpp -o vecadd
```

The `module load craype-accel-nvidia80` module will update the compiler flags and libraries used by the compiler wrapper to enable compilation of code for GPUs. The compiler wrapper will add the `-gpu` compiler flag and the user needs to indicate the GPU programming model with the correct compiler flag. In this case, `-cuda` is used to indicate compilation of CUDA code. The application can then be launched on one of the compute nodes.

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
module load craype-accel-nvidia80
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
If the application instead relies on the job launcher to bind MPI ranks to available GPUs, then a small helper script can be used to explicitly set `CUDA_VISIBLE_DEVICES` appropriately for each MPI rank. One example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu) and each MPI will similarly be bound to a single GPU with round-robin assignment. The binding of MPI ranks to GPUs is discussed in more detail here.

[//]: # (ToDo: Add link to running jobs page)

## GPU OpenCL

A simple OpenCL example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/OpenCL/vecadd). The OpenCL headers and library are available in the NVIDIA in the NVHPC SDK and cuda toolkits. The environment variable `NVIDIA_PATH` is defined for the `PrgEnv-nvidia` programming environment. 
```
CC -o vecadd -g -O3 -std=c++0x  -I${NVIDIA_PATH}/cuda/include main.o -L${NVIDIA_PATH}/cuda/lib64 -lOpenCL
```

This simple example can be run on a Polaris compute node as follows.
```
./vecadd
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

A simple MPI-parallel OpenMP example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/ProgrammingModels/Polaris/OpenMP/vecadd_mpi). Compilation proceeds similar to the above CUDA example except for the use of the `-mp=gpu` compiler flag to indicated compilation of OpenMP code for GPUs.

```
module load craype-accel-nvidia80
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
