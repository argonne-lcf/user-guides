# OpenCL examples:

Cooley nodes have 2 NVIDIA Kepler K80 GPUs. They support OpenCL.
The OpenCL documentation can be found here: [OpenCL Documentation](https://www.khronos.org/registry/OpenCL/sdk/2.2/docs/man/html/).

On the login node clone the following Repository:
```bash
git clone https://github.com/alcf-perfengr/alcl.git --branch cooley
```
or
```bash
git clone git@github.com:alcf-perfengr/alcl.git --branch cooley
```

On the compute node you need to load CUDA:
```
soft add +cuda-10.0
soft add +gcc-7.1.0
export CPATH=/soft/visualization/cuda-10.0/include/:$CPATH
export LIBRARY_PATH=/soft/visualization/cuda-10.0/include/:$LIBRARY_PATH
```

## C and C++:

C and C++ examples can be built, from their respective directories, with:
```bash
make "example name"
```
or ran with:
```bash
make run_"example name"
```

## Python:

On the login node install pyopencl:
```bash
pip install --user --upgrade numpy pyopencl
```
Each Python example can be ran on compute nodes using the the makefile:
```bash
make run_"example name"
```

Available Examples:

* platform : print available platforms
* tiny_cl_info: print available platforms and devices
* my_first_kernel: a simple hello world kernel
* verbose: a kernel showcasing OpenCL kernel index functions
* read_cl: a program loading the kernel from a file
* read_bincl: a program loading the kernel from a binary
* read_elfcl: a program loading the kernel in binary format from the executable (C/C++ only)
* buffer: a program getting data back from the device
* event_order: a program using event for synchronization
* event_unorder: a program showcasing out of order execution (if the implementation supports it)
* profiling: a program demonstrating kernel profiling
