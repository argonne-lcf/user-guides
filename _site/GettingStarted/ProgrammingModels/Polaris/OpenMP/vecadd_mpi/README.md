# Cray Compiler Wrappers
CUDA applications can be built on Polaris using the NVIDIA programming environment using appropriate modules. The default programming environment is `PrgEnv-nvidia`. All of the examples below are built and run within interactive jobs on Polaris nodes.

# Compilation w/ NVIDIA compilers
```
qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module load craype-accel-nvidia80
$ make -f Makefile.nvidia clean

$ make -f Makefile.nvidia
CC -g -O3 -std=c++0x -mp=gpu -gpu=cc80,cuda11.0 -c main.cpp
CC -o vecadd -g -O3 -std=c++0x -mp=gpu -gpu=cc80,cuda11.0 main.o 
```
## Compilation flags
Important compilation flags for OpenMP with the NVIDIA compilers are `-mp=gpu -gpu=cc80,cuda11.0`.

## Example output:
```
$ mpiexec -n 4 ./vecadd
# of devices= 4
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
Rank 0 running on GPU 0!


Result is CORRECT!! :)
```
