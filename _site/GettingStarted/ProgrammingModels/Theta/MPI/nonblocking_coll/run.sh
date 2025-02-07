#!/bin/bash -x
#COBALT -A Comp_Perf_Workshop
#COBALT -n 4
#COBALT -t 10
#COBALT --jobname nonblock_coll
#COBALT -O nonblocking_test
#COBALT -q debug-flat-quad
#COBALT --attrs filesystems=home,grand,eagle

export MPICH_NEMESIS_ASYNC_PROGRESS=SC 
export MPICH_MAX_THREAD_SAFETY=multiple
export MPICH_USE_DMAPP_COLL=1

rpn=1
#usage: stencil_mpi <n> <energy> <niters> <px> <py>
aprun -n $((COBALT_JOBSIZE * $rpn)) -N $rpn numactl -m 1 ./stencil_alltoallw 100 100 10 2 2
