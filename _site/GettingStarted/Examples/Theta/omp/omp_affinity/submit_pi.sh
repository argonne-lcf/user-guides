#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

# This shows how "pi.cpp" differs in terms of scaling 
# when different OpenMP affinity settings are used.
# The script sets `OMP_PLACES=cores`, and looks at scalability for
# OMP_PROC_BIND=close and OMP_PROC_BIND=spread.

# The difference in placement between `close` and `spread` is that with
# `spread` the OpenMP threads are spread as far apart over cores, while
# in the case of `close`, the threads are sent to cores next to each
# other, which means they can share the same tile.

# Run this script, look at the output and then consider the following
# questions:
# How does the timing scale with the number of cores used, for both close
# and spread?
# How do they differ when the number of threads is less
# than 64 and greater than 64?

# The timings are similar for all numbers of threads per core.

# Other things to try:
# - Run this with `OMP_PLACES=threads` instead of `OMP_PLACES=cores`
# - Run submit_approximate_bandwidth.sh. Do the scaling results differ?


export OMP_PLACES=cores

echo "--------------------------------------------------------------------------"
export OMP_PROC_BIND=close

echo " ### Timings with OMP_PROC_BIND=${OMP_PROC_BIND} and OMP_PLACES=${OMP_PLACES} " 
echo " ### across different numbers of threads:"
export OMP_NUM_THREADS=8
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=16
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=32
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=64
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=128
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=256
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi

echo ""
echo "--------------------------------------------------------------------------"
export OMP_PROC_BIND=spread
echo " ### Timings with OMP_PROC_BIND=${OMP_PROC_BIND} and OMP_PLACES=${OMP_PLACES} "
echo " ### across different numbers of threads:"
export OMP_NUM_THREADS=8
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=16
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=32
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=64
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=128
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
export OMP_NUM_THREADS=256
echo ""
echo " ### OMP_NUM_THREADS=${OMP_NUM_THREADS}"  
aprun -n 1 -N 1 -cc none ./pi
