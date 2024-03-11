---
search:
  exclude: true
---

# OpenCL on Cooley
## Overview
OpenCL (Open Computing Language) is an open standard from the Khronos group that enables heterogeneous device programming (e.g. CPUS, GPUs, and FPGAs). Complete descriptions of the API, memory hierarchy, and usage can be found in OpenCL documentation and technical specifications found on the Khronos website.

## Using OpenCL at ALCF
OpenCL is provided on Cooley via the NVIDIA GPU drivers for the Kepler K80 GPUs. OpenCL 1.2 is supported on Cooley. There is not an OpenCL CPU driver available on Cooley to launch OpenCL kernels on the CPUs.

## Building on Cooley
To build an OpenCL application for the GPUs on Cooley, one can modify their software environment to load CUDA.
```
soft add +cuda-10.0
soft add +gcc-7.1.0
export CPATH=/soft/visualization/cuda-10.0/include:$CPATH
export LIBRARY_PATH=/soft/visualization/cuda-10.0/include:$LIBRARY_PATH
```

The following is an example to compile a simple OpenCL application.
```
mpicxx main.cpp -lOpenCL 
```

## Running jobs on Cooley
An example ‘test.sh’ job submission script follows.
```
#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 1))
mpirun -f $COBALT_NODEFILE -n $PROCS ./a.out
```

To request a single node with 10 minutes of walltime, charging to the MyProject project, one can use the following command.
```
qsub -n 1 -t 10 -A MyProject ./test.sh
```

## ALCF Tutorials
There is an OpenCL tutorial available from the ALCF on GitHub that provides a walk through on several concepts of the programming model: introduction to API, querying device info, compiling kernels, using buffers, profiling, etc… The repo can be cloned to your working directory with one of the following commands.
```
git clone https://github.com/alcf-perfengr/alcl.git --branch cooley

or

git clone git@github.com:alcf-perfengr/alcl.git --branch cooley

```

The tutorial provides examples for C, C++, Python, and Ruby. Individual C and C++ examples can be built and run, from their respective directories using ‘make’ and the name of the example (e.g. platform).
```
cd alcl/C++
make platform
make run_platform
```
**Note:** OpenCL examples will only run correctly on the Cooley compute nodes as there is no CPU driver installed. For the Python examples, users will need to first install pyopencl before running similar ‘make’ commands.

```
pip install --user --upgrade numpy pyopencl

cd alcl/Python
make my_first_kernel
make run_my_first_kernel
```

