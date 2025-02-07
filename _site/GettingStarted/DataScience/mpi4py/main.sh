#!/bin/bash -l
module load conda
conda activate base
NRANKS=$(wc -l < "${PBS_NODEFILE}")
NGPU_PER_RANK=$(nvidia-smi -L | wc -l)
NGPUS="$((${NRANKS}*${NGPU_PER_RANK}))"
mpiexec --envall -n "${NGPUS}" --ppn "${NGPU_PER_RANK}" --hostfile="${PBS_NODEFILE}" python3 main.py

