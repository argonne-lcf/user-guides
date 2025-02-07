# Cray Compiler Wrappers
CUDA applications can be built on Polaris using the NVIDIA or GNU programming environments using appropriate modules. The default programming environment is `PrgEnv-nvidia`. All of the examples below are built and run within interactive jobs on Polaris nodes.

# Compilation with NVIDIA compilers
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module load craype-accel-nvidia80
$ make clean
$ make 
PATH_TO_NVHPC= [/opt/nvidia/hpc_sdk/Linux_x86_64/23.9]
CC -g -O3 -std=c++0x  -cuda -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.9/cuda/include -D_SINGLE_PRECISION -c offload.cpp
CC -g -O3 -std=c++0x  -cuda -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.9/cuda/include -D_SINGLE_PRECISION -c main.cpp
CC -o vecadd -g -O3 -std=c++0x  -cuda -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.9/cuda/include -D_SINGLE_PRECISION offload.o main.o -L/opt/nvidia/hpc_sdk/Linux_x86_64/23.9/cuda/lib64 -lcudart
```

## Example output
```
$ mpiexec -np 4 ./vecadd
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 4 RANKS_PER_NODE= 4 THREADS_PER_RANK= 1
Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
# of devices= 4
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
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
# Compilation w/ GNU compilers
GNU compilers here are used only for compiling code targeting the CPUS. The NVIDIA compilers (e.g. `nvcc`) are still used to generate offloadable code for the GPUs.
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:eagle -A <PROJECT>

$ module restore
$ module load craype-accel-nvidia80
$ module swap PrgEnv-nvhpc PrgEnv-gnu
$ module use /soft/modulefiles
$ module load cudatoolkit-standalone

$ make -f Makefile.gnu clean
$ make -f Makefile.gnu
PATH_TO_NVHPC= [/soft/compilers/cudatoolkit/cuda-12.2.2]
CC -g -O3 -std=c++0x -I/soft/compilers/cudatoolkit/cuda-12.2.2/include -D_SINGLE_PRECISION -c main.cpp
nvcc -x cu  -D_SINGLE_PRECISION -c offload.cpp -o offload.o
CC -o vecadd -g -O3 -std=c++0x -I/soft/compilers/cudatoolkit/cuda-12.2.2/include -D_SINGLE_PRECISION main.o offload.o -L/soft/compilers/cudatoolkit/cuda-12.2.2/lib64 -lcudart
```
## Example output:
```
$ mpiexec -np 4 ./vecadd
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 4 RANKS_PER_NODE= 4 THREADS_PER_RANK= 1
Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
# of devices= 4
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
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

# Compilation w/ oneAPI compilers
oneAPI (clang) compilers can similarly compile CUDA code to offload to GPUs. 
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:eagle -A <PROJECT>

$ module restore
$ module load craype-accel-nvidia80
$ module use /soft/modulefiles
$ module load oneapi/upstream
$ module load mpiwrappers/cray-mpich-oneapi-upstream

$ make -f Makefile.oneapi clean
$ make -f Makefile.oneapi
mpicxx -g --cuda-gpu-arch=sm_80 -x cuda -c offload.cpp
mpicxx -g --cuda-gpu-arch=sm_80 -x cuda -c main.cpp
mpicxx -o vecadd -g --cuda-gpu-arch=sm_80 offload.o main.o  -L/opt/nvidia/hpc_sdk/Linux_x86_64/23.9/cuda/12.2/lib64 -lcudart -L/opt/cray/pe/mpich/8.1.28/gtl/lib -lmpi_gtl_cuda
```

