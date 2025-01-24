# Gromacs on Polaris

## What is Gromacs?
GROMACS is a versatile package to perform molecular dynamics, i.e., simulate the Newtonian equations of motion for systems with hundreds to millions of particles. It is primarily designed for biochemical molecules like proteins, lipids, and nucleic acids that have a lot of complicated bonded interactions. However, since GROMACS is extremely fast at calculating the nonbonded interactions (which usually dominate simulations), many groups are also using it for research on non-biological systems, e.g., polymers.

## Using GROMACS at ALCF
ALCF offers assistance with building binaries and compiling instructions for GROMACS. For questions, contact us at support@alcf.anl.gov.

## Building Gromacs

1. Download the latest source code: [GROMACS Download](http://manual.gromacs.org/documentation/2022.1/download.html)
2. Extract the tarball: `tar -xzf gromacs-2022.1.tar.gz`
3. Swap programming environments: `module swap PrgEnv-nvhpc PrgEnv-gnu`
4. Load the CUDA toolkit: `module load cudatoolkit-standalone/11.2.2`
5. Load GCC: `module load gcc/10.3.0`
6. Load CMake: `module load cmake`
7. Change directory: `cd gromacs-2022.1`
8. Create a build directory: `mkdir build`
9. Configure the build:
   ```
   cmake -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC \
         -DBUILD_SHARED_LIBS=OFF -DGMX_BUILD_OWN_FFTW=ON \
         -DCMAKE_INSTALL_PREFIX=/path-to/gromacs-2022.1/build \
         -DGMX_MPI=ON -DGMX_OPENMP=ON -DGMX_GPU=CUDA \
         -DCUDA_TOOLKIT_ROOT_DIR=/soft/compilers/cudatoolkit/cuda-11.2.2
   ```
10. Compile the code: `make -j 8`
11. Install the binaries: `make install`
12. The installed binary is `build/bin/gmx_mpi`.

## Running Gromacs on Polaris
Prebuilt GROMACS binaries can be found in the directory `/soft/applications/Gromacs/gromacs-2022.1`.

A sample PBS script follows that will run GROMACS on two nodes, using 4 MPI ranks per node, and each rank with four OpenMP threads. The PME kernel owns one MPI rank and one GPU per node, while the nonbonded kernel uses 3 MPI ranks and 3 GPUs per node.

```sh
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A PROJECT
#PBS -l filesystems=home:eagle

cd ${PBS_O_WORKDIR}

module swap PrgEnv-nvhpc PrgEnv-gnu
module load cudatoolkit-standalone/11.2.2

export OMP_NUM_THREADS=4

mpirun --np 8 /soft/applications/Gromacs/gromacs-2022.1/gmx_mpi \
      mdrun -gputasks 0123 -nb gpu -pme gpu -npme 1 -ntomp 4 \
      -dlb yes -resethway -pin on -v deffnm step5_1 -g test.log
```

We strongly suggest that users try combinations of different numbers of nodes, MPI ranks per node, number of GPU tasks/devices, GPU task decomposition between nonbonded and PME kernels, and OMP threads per rank to find the optimal throughput for their particular workload.