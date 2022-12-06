# HDF5
[HDF5 (Hierarchical Data Format 5)](https://www.hdfgroup.org/solutions/hdf5/) is a high-performance I/O technology suite, designed for complex data sets.  The HDF5 technology suite is composed of a data model, a file format, an API, a library, and a set of software tools.

## Getting Started
Cray-optimized HDF5 libraries and software tools can be added to your environment by loading the cray-hdf5 or cray-hdf5-parallel modules. 

For Example:
```
module load cray-hdf5
```
Or, for parallel HDF5 support:
```
module load cray-hdf5-parallel
```
These commands will load the default hdf5 module (currently 1.10.1.1).  A list of all available HDF5 modules can be viewed by typing:
```
module avail -t 2>&1 | grep -i cray-hdf5
```
After the desired module is loaded, the location of the HDF5 library will be stored in the CRAY_HDF5_DIR environment variable.

## Using HDF5 in Python
H5py is popular python interface to the HDF5 binary data format.  Currently, the  miniconda-3.6/conda-4.4.10 module provides parallel h5py support (while miniconda-2.7/conda-4.4.10 only provides serial support). 

To perform a custom pip-installation of parallel h5py in a local user directory, the following command can be used:
```
HDF5_MPI="ON" HDF5_DIR=$CRAY_HDF5_DIR pip install --user --no-binary=h5py h5py
```
This command requires that the cray-hdf5-parallel module is first loaded, because parallel h5py will need an existing HDF5 library to build against.

***Warning: Users should always CLONE the module-provided miniconda environment before making changes.***

## Other I/O Libraries
The libraries activated by the cray-hdf5-parallel modules leverage MPI-IO for parallel file-system access. Therefore, parallel performance is limited that of the active cray-mpich library (currently cray-mpich/7.7.0 by default).

In addition to HDF5 and MPI-IO, netCDF and PnetCDF modules are also available on Theta.  Since PnetCDF leverages MPI-IO for parallelism, it's parallel performance is also limited to that of MPI-IO.
