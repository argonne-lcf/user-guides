# Compilation
```
soft add +intel-composer-xe
soft add +mvapich2-intel
soft add +cuda-10.2

make clean
make

qsub ./submit.sh
```
## Example output:
```
# of devices= 2
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ Tesla K80 ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ Tesla K80 ]
Running on GPU 0!
Using single-precision

  Name= Tesla K80
  Locally unique identifier= 
  Clock Frequency(KHz)= 823500
  Compute Mode= 0
  Major compute capability= 3
  Minor compute capability= 7
  Number of multiprocessors on device= 13
  Warp size in threads= 32
  Single precision performance ratio= 3

Result is CORRECT!! :)
```
