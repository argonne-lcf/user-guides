#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug-scaling
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:grand
#PBS -A Datascience

cd ${PBS_O_WORKDIR}
echo $CONTAINER

# SET proxy for internet access
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128

# MPI example w/ 16 MPI ranks per node spread evenly across cores
NODES=`wc -l < $PBS_NODEFILE`
PPN=1
PROCS=$((NODES * PPN))
echo "NUM_OF_NODES= ${NODES} TOTAL_NUM_RANKS= ${PROCS} RANKS_PER_NODE= ${PPN}"

MPI_BASE=/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/comm_libs/hpcx/hpcx-2.9.0/ompi/
export PATH=$MPI_BASE/bin:$PATH
export LD_LIBRARY_PATH=$MPI_BASE/lib:$LD_LIBRARY_PATH
export SINGULARITYENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo mpirun=$(which mpirun)

echo library path
mpirun -hostfile $PBS_NODEFILE -n $PROCS -npernode $PPN singularity exec --nv -B $MPI_BASE $CONTAINER ldd /usr/source/mpi_hello_world

echo C++ MPI
mpirun -hostfile $PBS_NODEFILE -n $PROCS -npernode $PPN singularity exec --nv -B $MPI_BASE $CONTAINER /usr/source/mpi_hello_world

echo Python MPI
mpirun -hostfile $PBS_NODEFILE -n $PROCS -npernode $PPN singularity exec --nv -B $MPI_BASE $CONTAINER python3 /usr/source/mpi_hello_world.py
