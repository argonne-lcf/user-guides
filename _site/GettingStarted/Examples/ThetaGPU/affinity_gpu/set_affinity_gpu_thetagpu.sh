#!/bin/bash -l
num_gpus=8
gpu=$((${OMPI_COMM_WORLD_LOCAL_RANK} % ${num_gpus}))
export CUDA_VISIBLE_DEVICES=$gpu
echo “RANK= ${OMPI_COMM_WORLD_RANK} LOCAL_RANK= ${OMPI_COMM_WORLD_LOCAL_RANK} gpu= ${gpu}”
exec "$@"
