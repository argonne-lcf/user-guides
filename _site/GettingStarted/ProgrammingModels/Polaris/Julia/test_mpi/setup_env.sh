module restore
module load cray-hdf5-parallel

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/nvidia/hpc_sdk/Linux_x86_64/22.11/cuda/11.8/extras/CUPTI/lib64/

export TMPDIR=/local/scratch

export PATH=/home/knight/.juliaup/bin:${PATH}
export JULIA_DEPOT_PATH=/grand/catalyst/proj-shared/knight/polaris/julia/depot

export JULIA_CUDA_MEMORY_POOL=none
export MPICH_GPU_SUPPORT_ENABLED=1
export JULIA_MPI_PATH=${CRAY_MPICH_DIR}
export JULIA_MPI_HAS_CUDA=1

