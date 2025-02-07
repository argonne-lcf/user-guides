# Cray Compiler Wrappers
OpenCL applications can be built on Polaris using the NVIDIA or GNU programming environments using appropriate modules. The default programming environment is `PrgEnv-nvidia`. All of the examples below are built and run within interactive jobs on Polaris nodes.

# Compilation with NVIDIA compilers
```
$ qsub -I -l select=1,walltime=1:00:00 -q debug -l filesystems=home:grand:eagle -A <PROJECT>

$ module load craype-accel-nvidia80
$ make Makefile.nvidia clean

$ make Makefile.nvidia
CC -o vecadd -g -O3 -std=c++0x  -I${NVIDIA_PATH}/cuda/include main.o -L${NVIDIA_PATH}/cuda/lib64 -lOpenCL
```

## Example output:
```
$ mpiexec -n 1 ./vecadd
Running on GPU!
Using single-precision

    CL_DEVICE_NAME: NVIDIA A100-SXM4-40GB
    CL_DEVICE_VERSION: OpenCL 3.0 CUDA
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 108
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 1410
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```
