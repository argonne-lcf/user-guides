# Compilation w/ oneAPI

```
qsub -I -l select=1,walltime=0:10:00,place=scatter -A Aurora_deployment -q EarlyAppAccess

make clean
make 

./submit.sh
```

## Compilation flags

The following are important compilation flags for OpenMP with the oneAPI compilers.
 - JIT : `-fiopenmp -fopenmp-targets=spir64`
 - AOT : `-fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend=spir64_gen "-device 12.60.7"`

The default is for OpenMP to target individual devices. Tiles can instead be targeted by setting `LIBOMPTARGET_DEVICES=subdevice`.

## Example output:

```
$ export LIBOMPTARGET_DEVICES=subdevice
$ mpiexec -n 12 --ppn 12 --depth=1 --cpu-bind depth ./vecadd
# of devices= 12
Using single-precision
Rank 0 running on GPU 0!
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
Rank 4 running on GPU 4!
Rank 5 running on GPU 5!
Rank 6 running on GPU 6!
Rank 7 running on GPU 7!
Rank 8 running on GPU 8!
Rank 9 running on GPU 9!
Rank 10 running on GPU 10!
Rank 11 running on GPU 11!

Result is CORRECT!! :)
