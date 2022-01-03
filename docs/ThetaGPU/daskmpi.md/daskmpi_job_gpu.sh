#!/bin/bash

source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
conda activate cup
cd ~/projects/dask_mpi
echo "Current Directory: "
pwd
export PYTHONPATH='/home/fsimini/projects/dask_mpi'
mpirun -x LD_LIBRARY_PATH -x PYTHONPATH -x PATH -np 20 -npernode 10 --hostfile $COBALT_NODEFILE python start_daskmpi_gpu.py cupy_test.py

