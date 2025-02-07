#!/bin/bash
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs filesystems=home,grand,eagle

# make sure to add +anaconda to .soft.cooley and then "resoft"
# before running.
# if you see errors like "ImportError: No module named mpi4py"
# you need to add the anaconda python environment in .soft.cooley

NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))

mpirun -f $COBALT_NODEFILE -n $PROCS -env OMP_NUM_THREADS 1 python ./hello_world.py
status=$?

echo "mpirun status is $status"
exit $status
