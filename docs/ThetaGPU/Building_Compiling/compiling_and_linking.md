# Compiling and Linking

ThetaGPU basically has AMD processors on the service nodes (`thetagpusn1,2`) and AMD processors and NVIDIA A100 GPUs on the compute nodes [see [overview](../system_overview_and_node_information.md) page]. The service nodes can be used to create containers and launch jobs, and eventually to use as a cross-compiling environment for compute nodes. Until the cross-compiling environment is set up, the compute nodes will have to be used for compiling. This can be done by using an interactive Cobalt job (via `qsub -I`), or until we have reserved or added a dedicated build node.

The default programming environment on the ThetaGPU compute nodes is the GNU compiler tools coupled with NVIDIA’s CUDA toolkit. 

For non-GPU codes:

 - `gcc` for C compiler
 - `g++` for C++
 - `gfortran` for Fortran


For CUDA codes:

 - `nvcc`
  

For MPI, the latest MPI is in `/lus/theta-fs0/software/thetagpu/openmpi-4.0.5.`

 - `mpicc`
 - `mpicxx`
 - `mpif77/mpif90` not configured yet

`mpirun` is a wrapper in `/usr/local/bin` that sets the appropriate options and uses the mpirun in the MPI directory above.

On the service nodes, GNU compilers are available.

There are no `modules/modulefiles` yet set up on ThetaGPU.
