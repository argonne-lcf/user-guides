# Math Libraries

## BLAS, LAPACK, and ScaLAPACK for CPUs

Some math libraries targeting CPUs are made available as part of the `nvhpc` modules and are based on the OpenBLAS project. Additional documentation is available from [NVIDIA](https://docs.nvidia.com/hpc-sdk/compilers/hpc-compilers-user-guide/#lib-use-lapack-blas-ffts). 

* BLAS & LAPACK can be found in the `$NVIDIA_PATH/compilers/lib` directory.

* ScaLAPACK can be found in the `$NVIDIA_PATH/comm_libs` directory.

[//]: # (ToDo: Need to test if these libraries are usable by gfortran, otherwise we need something compatible; AOCL might be solution)
[//]: # (ToDo: Add some pointers for AOCL when Abhishek gets it installed)

## NVIDIA Math Libraries for GPUs

Math libraries from NVIDIA are made available via the `nvhpc` modules. Many of the libraries users typically use can be found in the `$NVIDIA_PATH/math_libs` directory. Some examples follow and additional documentation is available from [NVIDIA](https://docs.nvidia.com/hpc-sdk/index.html#math-libraries).

* libcublas
* libcufft
* libcurand
* libcusolver
* libcusparse