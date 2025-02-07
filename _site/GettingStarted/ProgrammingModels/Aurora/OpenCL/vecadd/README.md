# Compilation w/ oneAPI
```
qsub -I -l select=1,walltime=0:10:00,place=scatter -A Aurora_deployment -q EarlyAppAccess

make clean
make

./submit.sh
```
## Example output:
```
Running on GPU!
Using single-precision

    CL_DEVICE_NAME: Intel(R) Data Center GPU Max 1550
    CL_DEVICE_VERSION: OpenCL 3.0 NEO 
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 896
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 1600
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```
