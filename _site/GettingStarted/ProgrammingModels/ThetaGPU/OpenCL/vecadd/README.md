# Compilation w/ GCC
```
module load cobalt/cobalt-gpu
qsub -I -n 1 -t 60 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

make clean
make

./submit.sh
```
## Example output:
```
Running on GPU!
Using single-precision

    CL_DEVICE_NAME: A100-SXM4-40GB MIG 4c.7g.40gb
    CL_DEVICE_VERSION: OpenCL 1.2 CUDA
    CL_DEVICE_OPENCL_C_VERSION: OpenCL C 1.2 
    CL_DEVICE_MAX_COMPUTE_UNITS: 56
    CL_DEVICE_MAX_CLOCK_FREQUENCY: 1410
    CL_DEVICE_MAX_WORK_GROUP_SIZE: 1024

Result is CORRECT!! :)
```
