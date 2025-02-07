# Compilation w/ Cray compiler wrappers

On Crux, the Cray compiler wrappers `cc, CC, ftn` point to the base CPU compilers for the Programming Environment loaded (e.g. gnu and cray). It is recommended to use the Cray compiler wrappers for compiling and linking applications.

Users are able to build applications on the Crux login nodes, but may find it convenient to build and test applications on the Crux compute nodes in short interactive jobs. This also has the benefit of allowing one to quickly test submission scripts and explore `mpiexec` settings within a single job.
```
$ qsub -I -l select=2,walltime=0:30:00 -l filesystems=home:grand:eagle -A <PROJECT>

$ make clean
$ make

$ ./submit.sh
```

## Example submission script

The following submission script will launch 128 MPI ranks on each node allocated. Each node contains dual 64-core CPUs and each MPI rank is bound to single core.
```
#!/bin/sh -l
#PBS -l select=2:system=crux
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug 
#PBS -A <PROJECT>
#PBS -l filesystems=home:grand:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 128 MPI ranks per node (1 rank per core)
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=128
NDEPTH=1
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

echo "Affinitying using cpu-bind depth"
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity
```

Example output follows and is written to the stdout file generated.

```
$ qsub -l select=2,walltime=0:10:00 -l filesystems=home:grand:eagle -A <PROJECT> ./submit.sh 

NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 256 RANKS_PER_NODE= 128 THREADS_PER_RANK= 1
Affinitying using cpu-bind depth
To affinity and beyond!! nname= x1000c0s3b0n0  rnk= 0  list_cores= (0)
...
To affinity and beyond!! nname= x1000c0s3b0n0  rnk= 127  list_cores= (127)

To affinity and beyond!! nname= x1000c0s3b0n1  rnk= 128  list_cores= (0)
...
To affinity and beyond!! nname= x1000c0s3b0n1  rnk= 255  list_cores= (127)
```
