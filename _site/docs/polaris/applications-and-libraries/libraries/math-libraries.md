# Math Libraries

## BLAS, LAPACK, and ScaLAPACK for CPUs

Some math libraries targeting CPUs are made available as part of the `nvhpc` modules and are based on the OpenBLAS project. Additional documentation is available from [NVIDIA](https://docs.nvidia.com/hpc-sdk/compilers/hpc-compilers-user-guide/#lib-use-lapack-blas-ffts).

- BLAS & LAPACK can be found in the `$NVIDIA_PATH/compilers/lib` directory.
- ScaLAPACK can be found in the `$NVIDIA_PATH/comm_libs` directory.
- The GNU Scientific Library, [GSL-2.7](https://www.gnu.org/software/gsl/), is available as `module help math_libs/gsl`.
- AMD Optimizing CPU Libraries, [AOCL v4.2](https://www.amd.com/content/dam/amd/en/documents/developer/version-4-2-documents/aocl/aocl-4-2-user-guide.pdf), is available as `module help math_libs/aocl`.
- Other Cray-based math libs such as Libsci and FFTW are made available by `module load cray-libsci` & `module load cray-fftw`.
[//]: # (ToDo: Need to test if these libraries are usable by gfortran, otherwise we need something compatible; AOCL might be a solution)

## NVIDIA Math Libraries for GPUs

Math libraries from NVIDIA are made available via the `nvhpc` modules. Many of the libraries users typically use can be found in the `$NVIDIA_PATH/math_libs` directory. Some examples follow, and additional documentation is available from [NVIDIA](https://docs.nvidia.com/hpc-sdk/index.html#math-libraries).

* libcublas
* libcufft
* libcurand
* libcusolver
* libcusparse