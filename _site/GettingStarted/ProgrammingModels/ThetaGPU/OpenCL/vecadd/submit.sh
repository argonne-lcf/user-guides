#!/bin/bash -l
#COBALT -n 1 -t 15 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

mpirun -np 1 ./vecadd

