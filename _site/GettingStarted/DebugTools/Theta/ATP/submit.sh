#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

# -- For additional information on using ATP to debug code
# -- on Theta, please look at the following ALCF website(s):
# -- 
# -- https://www.alcf.anl.gov/user-guides/atp-and-stat

export n_nodes=$COBALT_JOBSIZE
export n_mpi_ranks_per_node=64
export n_mpi_ranks=$(($n_nodes * $n_mpi_ranks_per_node))
export OMP_NUM_THREADS=1

# To enable ATP, ATP_ENABLED must be set at runtime
export ATP_ENABLED=1

# the following line is necessary for Intel Fortran codes 
# export FOR_IGNORE_EXCEPTIONS=true

# If main.cpp has not been changed to remove the bug, this
# run will cause a segfault, and ATP will produce a stacktrack
# and ATP core files.
aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node -cc depth -d $OMP_NUM_THREADS -j 1 ./main

