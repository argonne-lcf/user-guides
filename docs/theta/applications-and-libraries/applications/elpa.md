---
search:
  exclude: true
---

# ELPA

## What is ELPA?

ELPA is a Fortran/C/MPI library to solve dense hermitian (real or complex) matrices. ELPA is conceived to compute the eigenvectors and eigenvalues of large matrices in petascale computers. ELPA uses BLACS framework and some SCALAPACK functions to distribute and solve the eigen problem. Computationally intensive kernels in ELPA are optimized using intrinsic code for SSE, AVX, AVX2, AVX512, and QPX architectures. This code is popular in electronic structure codes, but it can be used for machine learning and other approaches that require full or partial spectrum solution of a matrix eigen problem.  ELPA scales efficiently in Theta solving matrices of 1 million by 1 million in less than 3 hours in 3000 KNL nodes.

## Using ELPA at ALCF

ALCF  provides assistance with compiling the library. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## How to obtain the code

ELPA can be downloaded free of charge from [https://elpa.mpcdf.mpg.de/software](https://elpa.mpcdf.mpg.de/software). 

## Building ELPA on Theta

ELPA must be compiled with AVX512 support in Theta. ELPA has OpenMP support, but it has shown lower performance for large number of MPI ranks. Because the interface of ELPA subroutines may change among versions 2016, 2017 and 2018, we strongly suggest that users visit [https://elpa.mpcdf.mpg.de/software](https://elpa.mpcdf.mpg.de/software) for further information.

This is an example for the compilation of ELPA in Theta.

```
> cat build_elpa_theta.sh
git clone https://gitlab.mpcdf.mpg.de/elpa/elpa.git
cd elpa
aclocal
autoreconf
./configure --prefix=/soft/applications/elpa/elpa2017 \
     --host=x86_64-suse-linux-gnu \
     --disable-shared --enable-avx512 \
     FC=ftn CC=cc \
     SCALAPACK_LDFLAGS="" \
     SCALAPACK_FCFLAGS="" \
     FCFLAGS=”-I/opt/intel/compilers_and_libraries_2018.0.128/linux/mkl/include/intel64/lp64”
make && make install
```
