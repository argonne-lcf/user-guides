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
This sets the following environment variables, some of which are used by
`cmake`:

* `KOKKOS_HOME` - path to the `lib64/`, `include/` files installed
* `LIBRARY_PATH` - prepends `$KOKKOS_HOME/lib64` to this variable used by `cmake`
* `CPATH` - prepends `$KOKKOS_HOME/include` to this variable used by `cmake`
* `LD_LIBRARY_PATH` - prepends `$KOKKOS_HOME/lib64` to this variable


### Building a Kokkos Application Using `cmake`

Add these lines to `CMakeLists.txt`:

```
find_package(Kokkos REQUIRED)
target_link_libraries(myTarget Kokkos::kokkoscore)
```

Here is a simple example `CMakeLists.txt` to compile an example program:

```
cmake_minimum_required(VERSION 3.22)
project(buildExample)
find_package(Kokkos REQUIRED)

set(buildExample_SOURCE_DIR ".")

set(top_SRCS
  ${buildExample_SOURCE_DIR}/example1.cpp)

set(SOURCE_FILES ${top_SRCS})

add_executable(example1_sycl_aot ${SOURCE_FILES})
target_link_libraries(example1_sycl_aot Kokkos::kokkoscore)
target_include_directories(example1_sycl_aot PUBLIC ${buildExample_SOURCE_DIR})
```

Configure and build it like this:

```
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=CC -DCMAKE_C_COMPILER=cc ..
make
```

### Building a Kokkos Application Using `make`

Here's an example `Makefile`:

```
# KOKKOS_HOME set via:
#   module load kokkos

# You can look at the first lines of $KOKKOS_HOME/KokkosConfigCommon.cmake to
# see the flags used in cmake configuration of the kokkos library build. The
# default Kokkos module on Polaris was built with PrgEnv-nvhpc and includes
# Serial, OpenMP (threads) and CUDA backends. So you should have that
# environment module loaded and include compiler flags for cuda and openmp:

# Cray MPI wrapper for C++ and C compilers:
CXX=CC
CC=cc

CPPFLAGS=-cuda -fopenmp
LDFLAGS=

LDFLAGS=$(CPPFLAGS) $(LDFLAGS)
LDLIBS=-L$(KOKKOS_HOME)/lib64 -lkokkoscore -lkokkossimd -lpthread

SRCS=example1.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: example1_polaris

example1_polaris: $(OBJS)
        $(CXX) $(LDFLAGS) -o example1_polaris $(OBJS) $(LDLIBS)

example1.o: example1.cpp

clean:
        rm -f $(OBJS)

distclean: clean
        rm -f example1_polaris
```


### Configuring Your Own Kokkos Build on Polaris

Here are recommended environment settings and configuration to build your own
kokkos libraries on Polaris:

#### Environment

To match what was done in the centrally-built kokkos associated with the
modules discussed above, use the default programming environment
`PrgEnv-nvhpc`, and use the Cray wrapper `CC` as the C++ compiler. To build
Kokkos, you'll need cmake. You may also use `PrgEnv-gnu` to build kokkos (also
using the Cray wrapper `CC` as the C++ compiler).

To use C++17, you'll need to work around a bug with the current
`PrgEnv-nvhpc/8.3.3` environment by loading a cudatoolkit-standalone module:

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
 -DKokkos_ARCH_ZEN3=ON\
 -DKokkos_ARCH_AMPERE80=ON\
 -DKokkos_ENABLE_CUDA=ON\
 -DKokkos_ENABLE_AGGRESSIVE_VECTORIZATION=ON\
 -DKokkos_ENABLE_TESTS=OFF\
 -DBUILD_TESTING=OFF\
 -DKokkos_ENABLE_CUDA_LAMBDA=ON\
 -DKokkos_ENABLE_IMPL_DESUL_ATOMICS=OFF\
 -DCMAKE_CXX_STANDARD=17\
 ..

make -j16 -l16 install
```
