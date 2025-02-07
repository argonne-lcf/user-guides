#!/bin/bash -l
#COBALT -n 1
#COBALT -t 10
#COBALT -A SDL_Workshop
#COBALT -q training
#COBALT --attrs mcdram=cache:numa=quad
#COBALT --attrs filesystems=home,grand,eagle

# -- For additional information on using Conda to manage phthon package on 
# --  Theta, please look at the following ALCF website(s):
# -- 
# -- https://www.alcf.anl.gov/user-guides/conda

module load miniconda-3/latest
aprun -n 64 -N 64 -d 1 -j 1 -cc depth -e OMP_NUM_THREADS=1 python ./hello_world.py


