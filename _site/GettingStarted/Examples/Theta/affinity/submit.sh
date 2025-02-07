#!/bin/bash -l
#COBALT -n 1 -t 30 -A Comp_Perf_Workshop -q comp_perf_workshop --attrs mcdram=cache:numa=quad --attrs filesystems=home,theta-fs0


#### Examples with setting affinity with aprun:

### MPI-only:

# Example #1: 1 nodes, 64 ranks/node, 1 thread/rank, 1 rank/core
#aprun -n 64 -N 64 -d 1 -j 1 -cc depth ./hello_affinity

# Example #2: 2 nodes, 64 ranks/node, 1 thread/rank, 1 rank/core
#aprun -n 128 -N 64 -d 1 -j 1 -cc depth ./hello_affinity


### MPI and OpenMP:

# Example #3: 1 nodes, 32 ranks/node, 4 thread/rank, 2 threads/core
aprun -n 32 -N 32 -d 4 -j 2 -cc depth -e OMP_NUM_THREADS=4 ./hello_affinity

# Example #4: 2 nodes, 32 ranks/node, 4 thread/rank, 2 threads/core
#aprun -n 64 -N 32 -d 4 -j 2 -cc depth -e OMP_NUM_THREADS=4 ./hello_affinity

# Example #5: 2 nodes, 1 ranks/node, 64 thread/rank, 1 rank/core
#aprun -n 2 -N 1 -d 64 -j 1 -cc depth -e OMP_NUM_THREADS=64 ./hello_affinity


#### Examples with setting affinty with both aprun and OpenMP affinity settings

# Example #6: 2 nodes, 1 ranks/node, 64 thread/rank, 1 rank/core
#aprun -n 2 -N 1 -cc none -e OMP_NUM_THREADS=64 -e OMP_PLACES={0}:64:1 ./hello_affinity

# Example #7: 2 nodes, 1 ranks/node, 4 thread/rank, 4 HW threads/core active. "spread" spreads out the threads
#OMP_DISPLAY_AFFINITY=true OMP_NUM_THREADS=4 OMP_PLACES=threads OMP_PROC_BIND=spread aprun -n 2 -N 1 -cc depth -d 128 -j 4 ./hello_affinity

# Example #8: 2 nodes, 1 ranks/node, 4 thread/rank, 4 HW threads/core active "close" keeps the threads close
#OMP_DISPLAY_AFFINITY=true OMP_NUM_THREADS=4 OMP_PLACES=threads OMP_PROC_BIND=close aprun -n 2 -N 1 -cc depth -d 128 -j 4 ./hello_affinity
