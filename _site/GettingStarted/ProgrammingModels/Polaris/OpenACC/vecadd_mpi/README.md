# Cray Compiler Wrappers
OpenACC applications can be built on Polaris using the NVIDIA programming environment using the appropriate module, which is currently the default programming environment `PrgEnv-nvidia`. The examples below are built and run within interactive jobs on Polaris nodes.

# Compilation with NVIDIA compilers
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module load craype-accel-nvidia80
$ make -f Makefile.nvidia clean

$ make -f Makefile.nvidia
CC -g -O3 -std=c++0x -acc=gpu -gpu=cc80,cuda11.0 -c main.cpp
CC -o vecadd -g -O3 -std=c++0x -acc=gpu -gpu=cc80,cuda11.0 main.o
```
## Compilation flags
Important compilation flags for OpenACC with the NVIDIA compilers are `-acc=gpu -gpu=cc80,cuda11.0`. In this example, each MPI rank sees all four GPUs on a Polaris node and GPUs are bound to MPI ranks round-robin within the application.

## Example output:
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

