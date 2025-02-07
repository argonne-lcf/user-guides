#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

# -- For additional information on using CrayPat-lite to profile code
# -- on Theta, please look at the following ALCF website(s):
# -- 
# -- https://www.alcf.anl.gov/user-guides/craypat-lite

# Make sure to have the darshan module unloaded and perftools-lite
# loaded when you compile (you don't need them loaded at runtime, though).

export n_nodes=$COBALT_JOBSIZE
export n_mpi_ranks_per_node=64
export n_mpi_ranks=$(($n_nodes * $n_mpi_ranks_per_node))
export OMP_NUM_THREADS=1

# check if the binary is instrumented with craypat:
strings=$(strings main | grep -m 1 "CrayPat/X")
echo $strings
if [ -z $strings ]
then
    echo "Binary is not instrumented with CrayPat-lite,"
    echo "like it should be in this example. Make sure"
    echo "module darshan is unloaded and module perftools-lite"
    echo "is loaded when you compile."
else
    echo "Binary is instrumented with CrayPat!"
fi

aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node -cc depth -d $OMP_NUM_THREADS -j 1 ./main

