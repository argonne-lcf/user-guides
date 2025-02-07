# OneAPI Compiler Wrappers
SYCL applications can be built on Polaris using the oneapi module and compiler wrappers provided by ALCF. These can be loaded into the environment using the following commands. 

```
module restore
module load craype-accel-nvidia80
module use /soft/modulefiles
module load oneapi/upstream
module load mpiwrappers/cray-mpich-oneapi-upstream
```

The `module restore` command is a quick way to restore the default environment. All of the examples below are built and run within interactive jobs on Polaris nodes.

# Compilation w/ SYCL compiler
```
qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module restore
$ module load craype-accel-nvidia80
$ module use /soft/modulefiles
$ module load oneapi/upstream
$ module load mpiwrappers/cray-mpich-oneapi-upstream$ module load oneapi
$ make clean
$ make
mpicxx -g -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 -std=c++20 -c main.cpp
mpicxx -o vecadd -g -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80 -std=c++20 main.o  -L/opt/cray/pe/mpich/8.1.28/gtl/lib -lmpi_gtl_cuda
```
## Compilation flags
Important compilation flags for SYCL running on GPUs are `-fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80`. If using GPU-aware MPI, then the application needs to be linked to the appropriate gtl libraries `-L/opt/cray/pe/mpich/8.1.16/gtl/lib -lmpi_gtl_cuda`.

## Example output:
```
$ export MPICH_GPU_SUPPORT_ENABLED=1
$ mpiexec -n 8 --ppn 8 --depth=4 --cpu-bind depth ./vecadd 
List Platforms and Devices
NUM_OF_NODES= 1 TOTAL_NUM_RANKS= 8 RANKS_PER_NODE= 8 THREADS_PER_RANK= 1
List Platforms and Devices
Platform: NVIDIA CUDA BACKEND NVIDIA Corporation CUDA 12.2
-- Device: NVIDIA A100-SXM4-40GB is a gpu
-- Device: NVIDIA A100-SXM4-40GB is a gpu
-- Device: NVIDIA A100-SXM4-40GB is a gpu
-- Device: NVIDIA A100-SXM4-40GB is a gpu
Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
Rank 4 running on GPU 0!
Rank 5 running on GPU 1!
Rank 6 running on GPU 2!
Rank 7 running on GPU 3!
Running on 8 ranks
# of devices= 4

Using double-precision


Result is CORRECT!! :)
```
