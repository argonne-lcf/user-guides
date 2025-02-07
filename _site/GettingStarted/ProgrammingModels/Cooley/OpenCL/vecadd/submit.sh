#!/bin/sh
#COBALT -n 1 -t 15 -q debug -A Catalyst --attrs filesystems=home,grand,eagle

mpirun -np 1 ./vecadd

