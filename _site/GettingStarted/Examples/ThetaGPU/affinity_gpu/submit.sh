#!/bin/bash -l
#COBALT -n 1 -t 15 -q full-node -A Catalyst

#mpirun -n 16 ./hello_affinity

mpirun -n 16 ./set_affinity_gpu_thetagpu.sh ./hello_affinity
