# ADIOS2

The Adaptable Input/Output (I/O) System (ADIOS2)
is a framework for I/O and streaming of scientific data developed as part of the U.S. DOE Exascale Computing Project.
ADIOS2 conveniently provides C, C++, Fortran, and Python API for traditional file system I/O, 
as well as API for transporting data between applications running concurrently on HPC systems. 
Data transport with ADIOS2 can be performed via the file system, wide-area-networks (WAN), remote direct memory access (RDMA), 
or MPI to construct a variety of workflows, such as in-situ (or in-transit) visualization, data analysis, and ML training and inference 
from ongoing simulations.

Users are invited to find more information about ADIOS2 on their [GitHub page](https://github.com/ornladios/ADIOS2) and their [documentation](https://adios2.readthedocs.io/en/v2.10.2/).

## Accessing ADIOS2 on Aurora

Pre-built modules are available to all users which enable access to the latest version of ADIOS2 (v2.10).
These modules can be displayed with `module avail adios2` and comprise of a CPU only build and a SYCL build of the library, with the SYCL build being the default.
For instance, the default SYCL build can be loaded by executing

```bash
module load adios2
```

Note that the ADIOS2 modules also load a Spack installation of Python 3.10 with the `adios2` package.

A custom build of ADIOS2 is also possible on Aurora.
In this case, we recommend the users start with the following install script for a base build

```bash linenums="1" title="install_adios2.sh
export CRAYPE_LINK_TYPE=dynamic
git clone https://github.com/ornladios/ADIOS2.git ADIOS2
mkdir adios2-build && cd adios2-build
cmake \
    -DCMAKE_INSTALL_PREFIX=${PWD}/install \
    -DADIOS2_BUILD_EXAMPLES=ON \
    -DADIOS2_USE_MPI=ON \
    -DADIOS2_HAVE_MPI_CLIENT_SERVER=ON \
    -DADIOS2_USE_SST=ON \
    -DADIOS2_USE_SSC=ON \
    -DADIOS2_USE_Python=OFF \
    -DADIOS2_USE_HDF5=OFF \
    -DADIOS2_USE_BZip2=OFF \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    ../ADIOS2 2>&1 | tee adios2_config.log
make -j 8 2>&1 | tee adios2_build.log
make install 2>&1 | tee adios2_install.log
cd ..
```

!!! warning "Building the Python bindings"

	The Python bindings for ADIOS2 can be built by setting `ADIOS2_USE_Python=ON`, however this requires a Python 3 installation to be found. We recommend users load the Python AI/ML module with `module load frameworks` and build ADIOS2 under this environment. This will require users to augemnt their Python path with `export PYTHONPATH=$PYTHONPATH:/path/to/adios2-build/install/lib/python3.10/site-packages` in order to use the `adios2` package. Alternatively, users can use a custom Python installation, but note that ADIOS2 requires `mpi4py` as well. 

A full list of CMake options is available at the [documentation](https://adios2.readthedocs.io/en/latest/setting_up/setting_up.html#install-from-source).







