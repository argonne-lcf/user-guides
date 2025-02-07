# Cray Compiler Wrappers
CUDA applications can be built on Polaris using the NVIDIA or GNU programming environments using appropriate modules. The default programming environment is `PrgEnv-nvidia`. All of the examples below are built and run within interactive jobs on Polaris nodes.

# Compilation with NVIDIA compilers
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module load craype-accel-nvidia80
$ make -f Makefile.nvidia clean

$ make -f Makefile.nvidia
CC -g -O3 -std=c++0x -cuda -c main.cpp
CC -o vecadd -g -O3 -std=c++0x -cuda main.o 
```
## Compilation flags
Important compilation flags for CUDA with the NVIDIA compilers are `-cuda -gpu=cc80,cuda11.0`.

## Example output
```
$ mpiexec -np 1 ./vecadd 
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
# Compilation w/ GNU compilers
GNU compilers here are used only for compiling code targeting the CPUS. The NVIDIA compilers (e.g. `nvcc`) are still used to generate offloadable code for the GPUs.
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module switch PrgEnv-nvidia/8.3.3 PrgEnv-gnu
$ module load craype-accel-nvidia80

$ make -f Makefile.gnu clean
$ make -f Makefile.gnu
nvcc -x cu -arch=sm_80 -c main.cpp -o main.o
CC -o vecadd -g -O3 -std=c++0x main.o 
```
## Example output:
```
$ mpiexec -np 1 ./vecadd 
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
