# Kokkos

## Kokkos

Kokkos Core implements a programming model in C++ for writing performance
portable applications targeting all major HPC platforms. For that purpose it
provides abstractions for both parallel execution of code and data
management. Kokkos is designed to target complex node architectures with
N-level memory hierarchies and multiple types of execution resources. It
currently can use Serial and OpenMP (threads) for CPU execution spaces
("backends") and CUDA, HIP, SYCL, and OpenMPTarget for GPU execution
spaces. By convention, Kokkos only allows one GPU backend at a time.

### Kokkos Documentation

* [Kokkos-core Wiki](https://kokkos.github.io/kokkos-core-wiki/)
* [Kokkos github](https://github.com/kokkos/kokkos)

### Kokkos on Polaris

The prebuilt Kokkos on polaris includes 3 backends: Serial and OpenMP for CPU
execution and CUDA for GPU execution. To use it, run

```
module use /soft/modulefiles
module load kokkos
```
### Configuring Your Own Kokkos Build on Polaris

Here are recommended environment settings and configuration to build your own
kokkos libraries on Polaris:

#### Environment

Use the default programming environment `PrgEnv-nvhpc`, and use the Cray wrapper
`CC` as the C++ compiler. To build Kokkos, you'll need cmake. To use C++17,
you'll need to work around a bug with the current `PrgEnv-nvhpc/8.3.3`
environment by loading a cudatoolkit-standalone module.

```
module load cmake cudatoolkit-standalone/11.6.2
```

#### CMake Configuration

This example builds three backends: OpenMP, Serial, and Cuda.

```
git clone git@github.com:kokkos/kokkos.git
cd kokkos
mkdir build
cd build

cmake\
 -DCMAKE_BUILD_TYPE=RelWithDebInfo\
 -DCMAKE_INSTALL_PREFIX="./install"\
 -DCMAKE_CXX_COMPILER=CC\
 -DKokkos_ENABLE_OPENMP=ON\
 -DKokkos_ENABLE_SERIAL=ON\
 -DKokkos_ARCH_ZEN2=ON\
 -DKokkos_ARCH_AMPERE80=ON\
 -DKokkos_ENABLE_CUDA=ON\
 -DKokkos_ENABLE_AGGRESSIVE_VECTORIZATION=ON\
 -DKokkos_ENABLE_TESTS=OFF\
 -DBUILD_TESTING=OFF\
 -DKokkos_ENABLE_CUDA_LAMBDA=ON\
 -DCMAKE_CXX_STANDARD=17\
 ..

make -j16 -l16 install
```
