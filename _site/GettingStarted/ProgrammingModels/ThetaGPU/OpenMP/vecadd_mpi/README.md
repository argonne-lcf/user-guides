# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 15 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

module load nvhpc-nompi

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
## Compilation flags
The following are important compilation flags for OpenMP with the NVIDIA compilers.
```
-mp=gpu -gpu=cc80,cuda11.0
```
## Example output:
```
# of devices= 1
Rank 0 running on GPU 0!
Rank 1 running on GPU 0!
Rank 2 running on GPU 0!
Rank 3 running on GPU 0!
Rank 4 running on GPU 0!
Rank 5 running on GPU 0!
Rank 6 running on GPU 0!
Rank 7 running on GPU 0!
Using single-precision


Result is CORRECT!! :)
```
# Compilation w/ LLVM & ALCF provided OpenMPI
```
qsub -I -n 1 -t 15 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

module load llvm

make -f Makefile.llvm clean
make -f Makefile.llvm

./submit.sh
```
## Compilation flags
The following are important compilation flags for OpenMP with the LLVM compilers.
```
-fopenmp-targets=nvptx64
```
