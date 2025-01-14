# Gromacs on ThetaGPU

## What is Gromacs?
GROMACS is a versatile package to perform molecular dynamics, i.e., simulate the Newtonian equations of motion for systems with hundreds to millions of particles. It is primarily designed for biochemical molecules like proteins, lipids, and nucleic acids that have a lot of complicated bonded interactions. However, since GROMACS is extremely fast at calculating the nonbonded interactions (which usually dominate simulations), many groups are also using it for research on non-biological systems, e.g., polymers.

## Using GROMACS at ALCF
ALCF offers assistance with building binaries and compiling instructions for GROMACS. For questions, contact us at support@alcf.anl.gov.

## Building Gromacs

1. Download the latest source code: [http://manual.gromacs.org/documentation/2022.1/download.html](http://manual.gromacs.org/documentation/2022.1/download.html)
2. Extract the tarball:
   ```bash
   tar -xzf gromacs-2022.1.tar.gz
   ```
3. Submit an interactive job to a ThetaGPU compute node from the Theta login node:
   ```bash
   user@thetalogin4:~> module load cobalt/cobalt-gpu
   user@thetalogin4:~> qsub -I -n 1 -t 60 -q single-gpu -A PROJECT --attrs filesystems=home
   ```
   Wait for the job to start and open an interactive session:
   ```bash
   Job routed to queue "single-gpu".
   Wait for job 10108666 to start...
   Opening interactive session to thetagpu06-gpu0
   ...
   user@thetagpu06:~$
   ```
4. Change to the Gromacs directory:
   ```bash
   cd gromacs-2022.1
   ```
5. Create a build directory:
   ```bash
   mkdir build
   ```
6. Load the cmake module:
   ```bash
   module load cmake
   ```
7. Configure the build with cmake:
   ```bash
   cmake -DCMAKE_C_COMPILER=mpicc -DCMAKE_CXX_COMPILER=mpicxx \
         -DBUILD_SHARED_LIBS=OFF -DGMX_BUILD_OWN_FFTW=ON \
         -DCMAKE_INSTALL_PREFIX=/path-to/gromacs-2022.1/build \
         -DGMX_MPI=ON -DGMX_OPENMP=ON -DGMX_GPU=CUDA \
         -DCUDA_TOOLKIT_ROOT_DIR=/user/local/cuda-11.4
   ```
8. Compile the code:
   ```bash
   make -j 16
   ```
9. Install the binaries:
   ```bash
   make install
   ```
10. The installed binary is `build/bin/gmx_mpi`.

## Running Gromacs on ThetaGPU
Prebuilt Gromacs binaries can be found in the directory `/soft/applications/gromacs/gromacs_cuda`.

A sample qsub script follows that will run GROMACS on a full node using all eight GPUs available.

```bash
#!/bin/bash -l
#COBALT -n 1
#COBALT -t 30 
#COBALT -q full-node 
#COBALT -project catalyst 
#COBALT --attrs filesystems=home,theta-fs0

NODES=`cat $COBALT_NODEFILE | wc -l`

mpirun -hostfile $COBALT_NODEFILE --np 8 \
      /soft/applications/gromacs/gromacs_cuda/gmx_mpi.2022.1 \
      mdrun -ntomp 8 -gputasks 01234567 -nb gpu -pme gpu -npme 1 \
      -dlb yes -resethway -pin on -v deffnm step5_1 -g test.log
```

We strongly suggest that users try combinations of different numbers of nodes, MPI ranks per node, number of GPU tasks/devices, GPU task decomposition between nonbonded and PME kernels, and OMP threads per rank to find the optimal throughput for their particular workload.

The following is a representative benchmark for a system with 30,000 atoms generated on a single ThetaGPU node with the above example.

| | Core time (sec) | Wall time (sec) | (%) | 
| ----------- | ----------- | ----------- | -- |
| Time | 691.769 | 10.810 | 6399.6 |

| | ns/day | hour/ns |
| -- | -- | -- |
| Performance | 399.661 | 0.060  |