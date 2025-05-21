## Kokkos

Kokkos Core implements a programming model in C++ for writing performance-portable applications targeting all major HPC platforms. For that purpose, it provides abstractions for both parallel execution of code and data management. Kokkos is designed to target complex node architectures with N-level memory hierarchies and multiple types of execution resources. It currently can use Serial and OpenMP (threads) for CPU execution spaces ("backends") and CUDA, HIP, SYCL, and OpenMPTarget for GPU execution spaces. By convention, Kokkos only allows one GPU backend at a time.

### Kokkos Documentation

* [Kokkos-core Wiki](https://kokkos.github.io/kokkos-core-wiki/)
* [Kokkos GitHub](https://github.com/kokkos/kokkos)

### Kokkos on Aurora

The prebuilt Kokkos on Aurora includes 3 backends: Serial and OpenMP for CPU execution and SYCL for GPU execution (with ahead-of-time (AOT) compilation, not just-in-time (JIT) compilation). To use it, run:

```bash
module load kokkos
```

This sets the following environment variables, some of which are used by `cmake`:

* `KOKKOS_ROOT` - path to the `lib64/`, `include/` files installed
* `LIBRARY_PATH` - prepends `$KOKKOS_ROOT/lib64` to this variable used by `cmake`
* `CPATH` - prepends `$KOKKOS_ROOT/include` to this variable used by `cmake`
* `LD_LIBRARY_PATH` - prepends `$KOKKOS_ROOT/lib64` to this variable

### Building a Kokkos Application Using `cmake`

Add these lines to `CMakeLists.txt`:

```cmake
find_package(Kokkos REQUIRED)
target_link_libraries(myTarget Kokkos::kokkos)
```

Here is a simple example `CMakeLists.txt` to compile an example program:

```cmake
cmake_minimum_required(VERSION 3.22)
project(buildExample)
find_package(Kokkos REQUIRED)

set(buildExample_SOURCE_DIR ".")

set(top_SRCS
  ${buildExample_SOURCE_DIR}/example1.cpp)

set(SOURCE_FILES ${top_SRCS})

add_executable(example1_sycl_aot ${SOURCE_FILES})
target_link_libraries(example1_sycl_aot Kokkos::kokkos)
target_include_directories(example1_sycl_aot PUBLIC ${buildExample_SOURCE_DIR})
```

Configure and build it like this:

```bash
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=icpx ..
make
```

### Building a Kokkos Application Using `make`

Here's an example `Makefile`:

```makefile
# KOKKOS_ROOT set via:
#   module load kokkos
# 
# You can look at the first lines of
# $KOKKOS_ROOT/lib64/cmake/Kokkos/KokkosConfigCommon.cmake to see the flags
# used in cmake configuration of the kokkos library build. The default Kokkos
# module on Aurora was built with the default oneAPI module and includes
# Serial, OpenMP (threads) and SYCL backends. So you should have that
# environment module loaded and include compiler flags for sycl and openmp:

# Aurora MPICH wrapper for C++ and C compilers:
CXX="mpic++ -cxx=icpx"
CC="mpicc -cc=icx"

SYCL_AOT_CPPFLAGS=-fsycl -fsycl-targets=spir64_gen -fno-sycl-id-queries-fit-in-int -fsycl-dead-args-optimization -fsycl-unnamed-lambda -std=c++17
SYCL_AOT_LDFLAGS=-Xsycl-target-backend "-device pvc"

CPPFLAGS=-g -O2 -fiopenmp -I $(KOKKOS_ROOT)/include $(SYCL_AOT_CPPFLAGS) -Wno-deprecated-declarations -Wno-tautological-constant-compare -Wno-unknown-attributes -ffp-model=precise

LDFLAGS=$(CPPFLAGS) $(SYCL_AOT_LDFLAGS)
LDLIBS=-L$(KOKKOS_ROOT)/lib64 -lkokkoscore -lkokkossimd -lkokkoscontainers -lpthread

SRCS=example1.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: example1_aurora

example1_aurora: $(OBJS)
	$(CXX) $(LDFLAGS) -o example1_aurora $(OBJS) $(LDLIBS)

example1.o: example1.cpp

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f example1_aurora
```

### Configuring Your Own Kokkos Build on Aurora

Here are recommended environment settings and configuration to build your own Kokkos libraries on Aurora:

#### Environment

To match what was done in the centrally-built Kokkos associated with the modules discussed above, use the same oneAPI version as indicated in `module help kokkos` and use the Aurora MPICH wrapper `mpic++ -cxx=icpx` as the C++ compiler (or just use `icpx` if you're not using MPI). To build Kokkos, you'll need CMake.

#### CMake Configuration

This example builds three backends: OpenMP, Serial, and SYCL.

```bash
git clone git@github.com:kokkos/kokkos.git
cd kokkos
mkdir build
cd build

cmake\
    -DCMAKE_CXX_COMPILER=icpx\
    -DKokkos_ENABLE_SERIAL=ON\
    -DKokkos_ENABLE_OPENMP=ON\
    -DKokkos_ENABLE_SYCL=ON\
    -DKokkos_ARCH_INTEL_PVC=ON\
    -DCMAKE_CXX_FLAGS="-fp-model=precise"\
    -DCMAKE_INSTALL_PREFIX=/path/to/your/install/directory\
    ..

make -j16 -l16 install
```
