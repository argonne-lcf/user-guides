```
# Compilation w/ NVHPC SDK w/ ALCF provided OpenMPI
```
qsub -I -n 1 -t 15 -q full-node -A Catalyst

module use /lus/theta-fs0/software/environment/thetagpu/lmod/tmp
module switch openmpi/openmpi-4.0.5 openmpi-4.1.0_nvhpc-21.3 

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
 # of devices=             8
 Rank             0  running on GPU             0
 Rank             7  running on GPU             7
 Rank             8  running on GPU             0
 Rank             1  running on GPU             1
 Rank             3  running on GPU             3
 Rank             6  running on GPU             6
 Rank             4  running on GPU             4
 Rank             9  running on GPU             1
 Rank            10  running on GPU             2
 Rank            12  running on GPU             4
 Rank            14  running on GPU             6
 Rank            15  running on GPU             7
 Rank             2  running on GPU             2
 Rank            11  running on GPU             3
 Rank            13  running on GPU             5
 Rank             5  running on GPU             5
 Name= A100-SXM4-40GB
 Locally unique identifier= 
 Clock Frequency(KHz)=     1410000.    
 Major compute capability=             8
 Minor compute capability=             0
 Number of multiprocessors on device=           108
 Warp size in threads=            32
 Single precision performance ratio=             2
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
 Result is CORRECT!! :)
```
