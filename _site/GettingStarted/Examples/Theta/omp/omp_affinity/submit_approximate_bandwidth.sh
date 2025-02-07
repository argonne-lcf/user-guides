#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

# This shows how "approximate_bandwidth.cpp" differs in terms of scaling 
# when different OpenMP affinity settings are used.
# The script sets `OMP_PLACES=cores`, and looks at scalability for
# OMP_PROC_BIND=close and OMP_PROC_BIND=spread.

# The difference in placement between `close` and `spread` is that with
# `spread` the OpenMP threads are spread as far apart over cores, while
# in the case of `close`, the threads are sent to cores next to each
# other, which means they can share the same tile.

# Run this script, look at the output and then consider the following
# questions:
# How does the BW scale with the number of cores used, for both close
# and spread?
# How do they differ when the number of threads is less
# than 64 and greater than 64?

# The BW results from `close` are less than the results for `spread`
# when there is less than 1 thread per core.  In `close`, the threads
# are placed such that they will share a tile with new threads,
# while in `spread`, OpenMP threads do not share a tile until there are
# 64 threads.  While the number of threads is less than the number of
# cores, using only one thread per tile results in greater BW than
# having the threads share a core.

# However, once there is at least one thread per core, the measured
# values for `spread` and `close` are similar.

# Other things to try:
# - Run this with `OMP_PLACES=threads` instead of `OMP_PLACES=cores`
# - Run submit_pi.sh. Do the scaling results differ?

export OMP_PLACES=cores

echo "--------------------------------------------------------------------------"
export OMP_PROC_BIND=close
echo " ### Timings with OMP_PROC_BIND=${OMP_PROC_BIND} and OMP_PLACES=${OMP_PLACES} " 
echo " ### across different numbers of threads:"
echo ""
export OMP_NUM_THREADS=8
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
echo ""
export OMP_NUM_THREADS=16
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
echo ""
export OMP_NUM_THREADS=32
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=64
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=128
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth

echo ""
echo "--------------------------------------------------------------------------"
export OMP_PROC_BIND=spread
echo " ### Timings with OMP_PROC_BIND=${OMP_PROC_BIND} and OMP_PLACES=${OMP_PLACES} "
echo " ### across different numbers of threads:"
export OMP_NUM_THREADS=8
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=16
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=32
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=64
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
export OMP_NUM_THREADS=128
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./approximate_bandwidth
