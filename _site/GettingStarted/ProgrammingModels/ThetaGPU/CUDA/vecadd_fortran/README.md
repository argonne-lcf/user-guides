# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 60 -q single-gpu --attrs filesystems=home,grand,eagle -A Catalyst

module load nvhpc-nompi
module switch openmpi/openmpi-4.0.5 openmpi/openmpi-4.0.5_ucx-1.10.0_nvhpc-21.7

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
## Example output:
```
 # of devices=             1
 Running on GPU             0
 Name= NVIDIA A100-SXM4-40GB
 Locally unique identifier= 
 Clock Frequency(KHz)=     1410000.    
 Major compute capability=             8
 Minor compute capability=             0
 Number of multiprocessors on device=           108
 Warp size in threads=            32
 Single precision performance ratio=             2
 
 Result is CORRECT!! :)

```
