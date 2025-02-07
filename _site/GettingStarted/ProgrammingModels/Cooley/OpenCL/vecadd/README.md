# Compilation
```
soft add +intel-composer-xe
soft add +mvapich2-intel
soft add +cuda-10.2

make clean
make

qsub ./submit.sh
```
## Example output:
```
Running on GPU!
Using single-precision

    CL_DEVICE_NAME: Tesla K80
    CL_DEVICE_VERSION: OpenCL 1.2 CUDA
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 13
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 823
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```
