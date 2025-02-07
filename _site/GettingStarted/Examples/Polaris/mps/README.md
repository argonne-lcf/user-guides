# Compilation w/ Cray Compiler Wrappers
Users are able to build applications on the Polaris login nodes, but may find it convenient to build and test applications on the Polaris compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly submission scripts.
```
$ qsub -I -l select=1,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make -f Makefile.nvhpc clean
$ make -f Makefile.nvhpc

./submit.sh

## MPS Helper Scripts

NVIDIA's [Multi-Process Service (MPS)](https://docs.nvidia.com/deploy/mps/index.html) can help to improve GPU utilization when multiple MPI ranks are utilizing the same GPU. A couple helper scripts (`enable_mps_polaris.sh` and `disable_mps_polaris.sh`) are provided here for convenience. These scripts can be executed using `mpiexec` to start MPS on each node allocated to a job on Polaris. The job submission script `submit_mps.sh` provides an example where a single MPI rank on each node is used to start MPS before launching the main application(s). Additional information on using MPS is available in the ALCF Polaris documentation [here](https://www.alcf.anl.gov/support/user-guides/polaris/queueing-and-running-jobs/example-job-scripts/index.html).

```
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A <PROJECT>
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 8 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=8
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Enable MPS on each node allocated to job
mpiexec -n ${NNODES} --ppn 1 ./enable_mps_polaris.sh

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_polaris.sh ./hello_affinity

# Disable MPS on each node allocated to job
mpiexec -n ${NNODES} --ppn 1 ./disable_mps_polaris.sh

```
