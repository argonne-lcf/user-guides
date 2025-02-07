#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

echo "*** Running Three Versions (might take awhile):"
echo "*** No vectorization, vectorization, and vectorization with memory alignment" 
export OMP_NUM_THREADS=1
echo ""
echo "*** No vectorization:"
aprun -n 1 -N 1 -cc depth  -j 1 -d 1 ./simd.novec 
echo ""
echo "*** Vectorization:" 
aprun -n 1 -N 1 -cc depth  -j 1 -d 1 ./simd.vec 
echo ""
echo "*** Vectorization with memory alignment:" 
aprun -n 1 -N 1 -cc depth -j 1 -d 1 ./simd.vec.mem.align 
