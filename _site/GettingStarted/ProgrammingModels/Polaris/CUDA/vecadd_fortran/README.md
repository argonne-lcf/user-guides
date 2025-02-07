# Cray Compiler Wrappers
CUDA Fortran applications can be built on Polaris using the NVIDIA programming environment using the appropriate module, which is currently the default programming environment `PrgEnv-nvidia`. The examples below are built and run within interactive jobs on Polaris nodes.

# Compilation w/ NVIDIA compilers
```
$ qsub -I -l select=1,walltime=1:00:00 -l filesystems=home:grand:eagle

$ module load craype-accel-nvidia80
$ make -f Makefile.nvidia clean

$ make -f Makefile.nvidia
ftn -g -Mfree -cuda -O3 -c main.F
ftn -o vecadd -g -Mfree -cuda -O3 main.o 
```

## Compilation flags
Important compilation flags for CUDA with the NVIDIA compilers are `-cuda -gpu=cc80,cuda11.0`.

## Example output:
```
$ mpiexec -np 1 ./vecadd 
 # of devices=             4
 Running on GPU             0
 Name= NVIDIA A100-SXM4-40GB
 Clock Frequency(KHz)=     1410000.    
 Major compute capability=             8
 Minor compute capability=             0
 Number of multiprocessors on device=           108
 Warp size in threads=            32
 Single precision performance ratio=             2
 
 Result is CORRECT!! :)
```
