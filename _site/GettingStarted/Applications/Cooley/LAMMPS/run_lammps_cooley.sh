#!/bin/sh

NUM_NODES=`cat $COBALT_NODEFILE | wc -l`
NUM_PROCS=$((NUM_NODES * 12))
export OMP_NUM_THREADS=1

EXE=/home/knight/coral/lammps/src/lmp_cooley
LMP_ARG='-in lmp.in -sf omp -var NSTEPS 200'

mpirun -f $COBALT_NODEFILE -n $NUM_PROCS $EXE $LMP_ARG


