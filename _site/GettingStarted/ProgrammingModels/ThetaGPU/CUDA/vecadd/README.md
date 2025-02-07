# OpenMPI Compiler Wrappers
When using the ALCF provided OpenMPI MPI library, a set of environment variables can be used to change from the default GNU compiler: OMPI_MPICC, OMPI_MPICXX, and OMPI_MPIF90, for example.
# Compilation w/ GCC
```
module load cobalt/cobalt-gpu
qsub -I -n 1 -t 60 -q single-gpu --attrs filesystems=home,grand,eagle -A Catalyst

make clean
make

./submit.sh
```
## Example output:
```
# of devices= 1
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB MIG 4c.7g.40gb ]
Running on GPU 0!
Using single-precision

  Name= A100-SXM4-40GB MIG 4c.7g.40gb
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 56
  Warp size in threads= 32
  Single precision performance ratio= 2

Result is CORRECT!! :)
```
# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 60 -q single-gpu --attrs filesystems=home,grand,eagle -A Catalyst

module load nvhpc-nompi

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
## Example output:
```
# of devices= 1
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB MIG 4c.7g.40gb ]
Running on GPU 0!
Using single-precision

  Name= A100-SXM4-40GB MIG 4c.7g.40gb
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 56
  Warp size in threads= 32
  Single precision performance ratio= 2

Result is CORRECT!! :)
```
# Compilation w/ LLVM & ALCF provided OpenMPI
```
qsub -I -n 1 -t 60 -q single-gpu --attrs filesystems=home,grand,eagle -A Catalyst

module load llvm

make -f Makefile.llvm clean
make -f Makefile.llvm

./submit.sh
```
## Example output:
```
# of devices= 1
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ A100-SXM4-40GB MIG 4c.7g.40gb ]
Running on GPU 0!
Using single-precision

  Name= A100-SXM4-40GB MIG 4c.7g.40gb
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 56
  Warp size in threads= 32
  Single precision performance ratio= 2

Result is CORRECT!! :)
```
