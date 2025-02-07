#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -A Datascience

echo $CONTAINER

cd ${PBS_O_WORKDIR}

# SET proxy for internet access
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128

wget https://github.com/horovod/horovod/raw/master/examples/pytorch/pytorch_synthetic_benchmark.py

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=8
NDEPTH=8
NTHREADS=1
#PPN=16
#PROCS=$((NODES * PPN))
#echo "NUM_OF_NODES= ${NODES} TOTAL_NUM_RANKS= ${PROCS} RANKS_PER_NODE= ${PPN}"
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

echo test mpi
mpiexec -n ${NNODES} --ppn 1 \
   singularity exec --nv -B $PWD $CONTAINER \
      python $PWD/pytorch_synthetic_benchmark.py

echo adjust memory in pytorch for this
#mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} \
#   singularity exec --nv -B $PWD $CONTAINER \
#      python $PWD/pytorch_synthetic_benchmark.py

