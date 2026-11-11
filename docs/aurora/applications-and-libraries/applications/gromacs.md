---
description: Build and run GROMACS on Aurora with SYCL GPU support, PBS job scripts, and tuning for multi-node scaling.
tags:
  - Molecular Dynamics
---

# GROMACS on Aurora

## What is GROMACS?

GROMACS is a versatile package to perform molecular dynamics, i.e. simulate the Newtonian equations of motion for systems with hundreds to millions of particles. It is primarily designed for biochemical molecules like proteins, lipids and nucleic acids that have a lot of complicated bonded interactions. However, since GROMACS is extremely fast at calculating the nonbonded interactions (which usually dominate simulations), many groups also use it for research on non-biological systems, e.g. polymers.

## Using GROMACS at ALCF

ALCF offers assistance with building binaries and compiling instructions for GROMACS. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Building GROMACS

1. Download the source code. See [GROMACS documentation](https://manual.gromacs.org/documentation/) for the latest release.

    ```bash
    wget https://ftp.gromacs.org/gromacs/gromacs-2026.4.tar.gz
    ```

    Where `https://ftp.gromacs.org/gromacs/gromacs-2026.4.tar.gz` is the URL copied from the download page.

2. Extract the tarball:

    ```bash
    tar -xzf gromacs-2026.4.tar.gz
    cd gromacs-2026.4
    ```

3. Load required modules (verify versions on Aurora with `module avail`):

    ```bash
    module load gcc/14.3.0
    module load cmake
    ```

4. Create a build directory:

    ```bash
    mkdir -p build && cd build
    ```

5. Configure the build:

    ```bash
    cmake .. \
        -DCMAKE_C_COMPILER=mpicc \
        -DCMAKE_CXX_COMPILER=mpicxx \
        -DCMAKE_INSTALL_PREFIX=$HOME/software/gromacs-2026.4 \
        -DGMX_MPI=ON -DGMX_OPENMP=ON -DGMX_GPU=SYCL -DGMX_SYCL=DPCPP \
        -DGMX_GPU_NB_CLUSTER_SIZE=8 \
        -DGMX_GPU_NB_NUM_CLUSTER_PER_CELL_X=1
    ```

    Where `$HOME/software/gromacs-2026.4` is the install directory on Aurora.

6. Compile the code:

    ```bash
    make -j 8
    ```

7. Install the binaries:

    ```bash
    make install
    ```

    The installed binary is `$HOME/software/gromacs-2026.4/bin/gmx_mpi`.

## Running GROMACS on Aurora

A sample PBS script for Aurora follows. Set `<project>` to your allocation and `<path>` to your input/output paths as described below.

```bash linenums="1" title="submit_gromacs.sh"
#!/bin/bash -l
#PBS -l select=1
#PBS -l place=scatter
#PBS -l walltime=00:10:00
#PBS -l filesystems=home:flare
#PBS -q debug
#PBS -A <project>

cd ${PBS_O_WORKDIR}

NNODES=$(wc -l < "${PBS_NODEFILE}")
NRANKS_PER_NODE=12
NTHREADS=4

export MPICH_GPU_SUPPORT_ENABLED=1
export GMX_ENABLE_DIRECT_GPU_COMM=1
export OMP_NUM_THREADS=${NTHREADS}
export OMP_PLACES=cores

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

EXE=$HOME/software/gromacs-2026.4/bin/gmx_mpi

MPI_ARG=" -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} "
MPI_ARG+=" --cpu-bind list:1-8:9-16:17-24:25-32:33-40:41-48:53-60:61-68:69-76:77-84:85-92:93-100 "

# 12 ranks per node - use for 1 node
EXE_ARG="mdrun -gputasks 0,1,2,3,4,5,6,7,8,9,10,11 -nb gpu -pme gpu -npme 1 -ntomp ${NTHREADS} -pin off "
# 11 ranks per node - use for 2,3,5 nodes (also set NRANKS_PER_NODE=11 above)
# EXE_ARG="mdrun -gputasks 0,1,2,3,4,5,6,7,8,9,10 -nb gpu -pme gpu -npme 1 -ntomp ${NTHREADS} -pin off "

EXE_ARG+=" -nobackup -dlb no -resethway -nsteps 20000 -s <path> -cpi <path> -deffnm <path> -noappend "

COMMAND="mpiexec ${MPI_ARG} ${EXE} ${EXE_ARG}"
echo "COMMAND= ${COMMAND}"
${COMMAND}
```

Where `<project>` is your allocation name, `home:flare` is the `<path>` filesystem request (see `qstat -Bf` for `valid_filesystems`), and each `<path>` in `EXE_ARG` is the TPR input, checkpoint file, and output prefix respectively.

We strongly suggest that users try combinations of different numbers of nodes, MPI ranks per node, number of GPU tasks/devices, GPU task decomposition between nonbonded and PME kernels, and OMP threads per rank to find the optimal throughput for their particular workload.

