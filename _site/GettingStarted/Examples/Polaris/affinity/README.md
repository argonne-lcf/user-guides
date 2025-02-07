# Compilation w/ Cray compiler wrappers
Users are able to build applications on the Polaris login nodes, but may find it convenient to build and test applications on the Polaris compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly test submission scripts and explore `mpiexec` settings within a single job.
```
$ qsub -I -l select=2,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make clean
$ make

$ ./submit.sh
```
## Example submission script
The following submission script will launch 16 MPI ranks on each node allocated. The MPI ranks are bound to CPUS with a depth (stride) of 4.
```
#!/bin/sh
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A <PROJECT>
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 16 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=16
NDEPTH=4
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity
```

## Example output:
This example launches 16 MPI ranks on each node with each rank bound to sets of four cores and output is written to the stdout file generated.
```
$ qsub -l select=2,walltime=0:10:00 -l filesystems=home:grand:eagle -A <PROJECT> ./submit.sh 

NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 32 RANKS_PER_NODE= 16 THREADS_PER_RANK= 1
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 0  list_cores= (0-3)

To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 1  list_cores= (4-7)
...
To affinity and beyond!! nname= x3007c0s13b0n0  rnk= 15  list_cores= (60-63)

To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 16  list_cores= (0-3)
...
To affinity and beyond!! nname= x3007c0s13b1n0  rnk= 31  list_cores= (60-63)

```

## PrgEnv-gnu
The default programming environment on Polaris uses the NVIDIA NVHPC compilers and assumes users are planning to run on the GPUs. The GNU programming environment can be loaded by switching to an appropriate module.
```
$ module swap PrgEnv-nvhpc PrgEnv-gnu
```
With no other change to the default environment, users will observe compilation of this simple MPI-only CPU code fail during linking as a path to the CUDA runtime library required by the `libmpi_gtl_cuda.so` library is not set. Example output follows.
```
$ make
CC -g -fopenmp -O3 -c main.cpp
CC -o hello_affinity -g -fopenmp -O3 main.o 
/usr/bin/ld: warning: libcudart.so.11.0, needed by /opt/cray/pe/mpich/8.1.16/gtl/lib/libmpi_gtl_cuda.so, not found (try using -rpath or -rpath-link)
/usr/bin/ld: /opt/cray/pe/mpich/8.1.16/gtl/lib/libmpi_gtl_cuda.so: undefined reference to `cudaMalloc@libcudart.so.11.0'
/usr/bin/ld: /opt/cray/pe/mpich/8.1.16/gtl/lib/libmpi_gtl_cuda.so: undefined reference to `cudaGetDeviceCount@libcudart.so.11.0'
/usr/bin/ld: /opt/cray/pe/mpich/8.1.16/gtl/lib/libmpi_gtl_cuda.so: undefined reference to `cudaGetDevice@libcudart.so.11.0'
/usr/bin/ld: /opt/cray/pe/mpich/8.1.16/gtl/lib/libmpi_gtl_cuda.so: undefined reference to `cudaMallocHost@libcudart.so.11.0'
...
```
This can be resolved by loading either of the `nvhpc-mixed` or `cudatoolkit-standalone` modules which will make the NVIDIA compilers `nvc, nvc++, nvfortran, nvcc` as well as tools and libraries available in the paths.

```
$ module swap PrgEnv-nvhpc PrgEnv-gnu
$ module load nvhpc-mixed
$ make
```
In this environment, the Cray compiler wrappers `cc, CC, ftn` point to the GNU CPU compilers `gcc, g++, gfortran` that should be used for compiling CPU code and linking applications, whereas the the NVIDIA compilers can be used to compiler GPU code.

## LLVM Compilers

LLVM compilers for SYCL and OpenMPTarget applications are also available on Polaris and provided by ALCF. To compile and link MPI-enabled applications, a separate `mpiwrappers/cray-mpich-[compiler]` module can be loaded to provide access to `mpicc, mpicxx, mpif90` compiler wrappers that should be used for compiling and linking MPI applications. Examples for compiling the simple CPU-only MPI example follow.

* `oneapi` for SYCL applications
```
$ module load oneapi
$ module load mpiwrappers/cray-mpich-oneapi
$ make -f Makefile.llvm
```

* `llvm` for OpenMPTarget applications
```
$ module load llvm
$ module load mpiwrappers/cray-mpich-llvm
$ make -f Makefile.llvm
```
In these environments, the Cray compiler wrappers `cc, CC, ftn` may still be available in the path, but those will not use the oneapi or llvm compilers loaded by these modules.

For GPU-enabled applications, which are the likely focus here, the oneapi module does load an appropriate cudatoolkit-standalone module. For llvm, one will need to explicitly load a cudatoolkit-standalone module themselves.
