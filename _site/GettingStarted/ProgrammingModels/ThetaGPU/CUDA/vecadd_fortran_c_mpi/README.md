# Compilation w/ GCC
```
module load cobalt/cobalt-gpu
qsub -I -n 1 -t 15 -q full-node --attrs filesystems=home,grand,eagle -A Catalyst

make clean
make

./submit.sh
```
## Example output:
```
# of devices= 8
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
Rank 4 running on GPU 4!
Rank 5 running on GPU 5!
Rank 6 running on GPU 6!
Rank 7 running on GPU 7!
Rank 8 running on GPU 0!
Rank 9 running on GPU 1!
Rank 10 running on GPU 2!
Rank 11 running on GPU 3!
Rank 12 running on GPU 4!
Rank 14 running on GPU 6!
Rank 15 running on GPU 7!
Rank 1 running on GPU 1!
Rank 13 running on GPU 5!
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [4] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [5] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [6] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [7] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
Rank 0 running on GPU 0!
Using single-precision

  Name= A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2
 Result is CORRECT!!! :)
```
# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 15 -q full-node --attrs filesystems=home,grand,eagle -A Catalyst

module load nvhpc-nompi
module switch openmpi/openmpi-4.0.5 openmpi/openmpi-4.0.5_ucx-1.10.0_nvhpc-21.7

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
# Example output
```
Rank 1 running on GPU 1!
Rank 2 running on GPU 2!
Rank 3 running on GPU 3!
Rank 4 running on GPU 4!
Rank 5 running on GPU 5!
Rank 6 running on GPU 6!
Rank 7 running on GPU 7!
Rank 8 running on GPU 0!
Rank 9 running on GPU 1!
Rank 10 running on GPU 2!
Rank 11 running on GPU 3!
Rank 12 running on GPU 4!
Rank 13 running on GPU 5!
Rank 14 running on GPU 6!
Rank 15 running on GPU 7!
# of devices= 8
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [4] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [5] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [6] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
  [7] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB ]
Rank 0 running on GPU 0!
Using single-precision

  Name= A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2
 Result is CORRECT!!! :)
```
