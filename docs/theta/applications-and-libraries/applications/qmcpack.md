# QMCPACK on Theta
## Overview
QMCPACK is a modern, high-performance, open-source Quantum Monte Carlo (QMC) simulation code. Its main applications are electronic structure calculations of molecular, quasi-2D, and solid-state systems.

## How to access QMCPACK
Prebuilt QMCPACK binaries are provided on Theta under /soft/applications/qmcpack in folder latest-release and current-develop. latest-release uses the latest release of QMCPACK and is recommended for production runs. current-develop uses the development branch which contains new features and bug fixes before the next release. While latest-release and current-develop are updated, all the old binaries are still accessible to users under the folder of year (2017,2018,2019). All the binaries are dynamically linked and require certain versions of libraries to function properly. Please read the README file and load necessary modules. QMCPACK is heavily optimized for Xeon Phi processors by using the Structure-of-Array data layout and new algorithms. Please use the SoA binary whenever possible.

## Using QMCPACK at ALCF
ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

If building your own QMCPACK is necessary, follow the instruction described below.

## Building on Theta
This recipe was verified on February 20, 2019, with QMCPACK v3.6.0.

```
export CRAYPE_LINK_TYPE=dynamic
# Do not use cmake 3.9.1, it causes trouble with parallel HDF5.
module load cmake/3.11.4
module unload cray-libsci
module load cray-hdf5-parallel
module load gcc   # Make C++ 14 standard library available to the Intel compiler
export BOOST_ROOT=/soft/libraries/boost/1.64.0/intel
cmake -DENABLE_SOA=1 ..
make -j 24
```

## Running QMCPACK jobs on Theta
Below is an example of a submission script of running the binary qmcpack on Theta:

```
#!/bin/bash
#COBALT -q default
#COBALT -A YOUR_PROJECT
#COBALT -n 128
#COBALT -t 30
#COBALT -O dmc
#COBALT --attrs mcdram=cache:numa=quad

file_prefix=NiO-fcc-S8-dmc
exe=/soft/applications/qmcpack/latest-release/build_KNL_Intel_real_SoA/bin/qmcpack

NCORES=64
HT=1
NTHREADS=$((NCORES * HT))

aprun -n $COBALT_PARTSIZE -N 1 -cc depth -d $NTHREADS -j $HT $exe $file_prefix.xml > $file_prefix.out
```

## References
- [https://qmcpack.org](https://qmcpack.org)
- [https://qmcpack.readthedocs.io/en/develop/](https://qmcpack.readthedocs.io/en/develop/))
