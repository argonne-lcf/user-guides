# Clang Compiler on Polaris
Cray Programming Environment does not offer LLVM compiler support.
Thus cc/CC compiler wrappers with LLVM compilers are not available.

If LLVM compilers is needed without MPI support, load `llvm` module.
When OpenMP offload or CUDA features are needed in the LLVM compilers, load `cudatoolkit-standalone` module.

To use Clang with MPI, load `mpiwrappers/cray-mpich-llvm` module
which loads the following modules
`cray-mpich`, MPI compiler wrappers mpicc/mpicxx/mpif90. mpif90 uses gfortran.
`cray-pals`, MPI launchers mpiexec/aprun/mpirun
**Limitation** There is no GPU-aware MPI support by default. If needed, manually add the GTL (GPU Transport Layer) library to the link line.

[//]: # (ToDo: Install llvm/clang compiler and module, create examples, and document)
