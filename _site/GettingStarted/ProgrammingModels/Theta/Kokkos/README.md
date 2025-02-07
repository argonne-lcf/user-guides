# Kokkos on Theta

Kokkos is a C++ Performance Portability Programming EcoSystem.
 
General information and documentation on Kokkos can be found on the Kokkos Github repository at
https://github.com/kokkos/kokkos, and an extensive tutorial can be found in the Kokkos Tutorials repository at
https://github.com/kokkos/kokkos-tutorials.
 
The rest of this document will focus on getting Kokkos up and building on Theta at ALCF.
Familiarity with `bash` and `git` is assumed.
 
## Step 1:  Clone the repository.
 
```sh
git clone git@github.com:kokkos/kokkos.git
cd kokkos
export KOKKOS_PATH="${PWD}"
```
 
Note:  the Kokkos Project strives to keep the master branch stable.
 
 
## Step 2:  Make sure that a relatively new version of CMake is available.
 
```sh
module load cmake/3.18.0
```
 
## Step 3:  Create a build directory.
 
```sh
mkdir build && cd build
```
 
 
## Step 4:  Generate the Makefile.
 
```sh
cmake ../kokkos \
    -DCMAKE_CXX_COMPILER=CC \
    -DCMAKE_INSTALL_PREFIX="${PWD}/kokkos-install" \
    -DKokkos_ENABLE_OPENMP=On \
    -DKokkos_ENABLE_HWLOC=On \
    -DKokkos_HWLOC_DIR=/usr \
    -DHWLOC_LIBRARY=/usr/lib64/libhwloc.so
```
 
Note:  the default compiler on Theta should be sufficient to build Kokkos.
 
 
##  Step 5: Install Kokkos.
 
```sh
make install
```
 
Note:  this will end up installing Kokkos in `"${KOKKOS_PATH}/build/kokkos-install"`.  If you wish to install it in a different directory, change `CMAKE_INSTALL_PREFIX` in step 4.

