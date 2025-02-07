# CUDA_VISIBLE_DEVICES
The aforementioned environment variable can be used to set which GPU devices are visible to an application and the order in which they appear. The default in examples below below will use all available devices present on a compute node. This would be 8 in 'full-node' runs and 1 in 'single-gpu' runs. In full-node runs, this number can be altered as in the following example.
```
export CUDA_VISIBLE_DEVICES=0,1
./submit.sh
```
## Example output from two GPUs
```
# of devices detected= 2
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]

Running on 2 GPUs!
Using single-precision

  Info from device 0
  Name= NVIDIA A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2

Result from GPU 0 is CORRECT!! :)

Result from GPU 1 is CORRECT!! :)
```
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
# of devices detected= 8
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [4] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]

  Info from device 0
  Name= NVIDIA A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2

Result from GPU 0 is CORRECT!! :)

Result from GPU 1 is CORRECT!! :)

Result from GPU 2 is CORRECT!! :)

Result from GPU 3 is CORRECT!! :)

Result from GPU 4 is CORRECT!! :)

Result from GPU 5 is CORRECT!! :)

Result from GPU 6 is CORRECT!! :)

Result from GPU 7 is CORRECT!! :)
```
# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 60 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

module load nvhpc-nompi/21.3
module switch openmpi openmpi/openmpi-4.0.5_ucx-1.10.0_nvhpc-21.7

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
## Example output:
```
# of devices detected= 8
  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [4] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]

  Info from device 0
  Name= NVIDIA A100-SXM4-40GB
  Locally unique identifier= 
  Clock Frequency(KHz)= 1410000
  Compute Mode= 0
  Major compute capability= 8
  Minor compute capability= 0
  Number of multiprocessors on device= 108
  Warp size in threads= 32
  Single precision performance ratio= 2

Result from GPU 0 is CORRECT!! :)

Result from GPU 1 is CORRECT!! :)

Result from GPU 2 is CORRECT!! :)

Result from GPU 3 is CORRECT!! :)

Result from GPU 4 is CORRECT!! :)

Result from GPU 5 is CORRECT!! :)

Result from GPU 6 is CORRECT!! :)

Result from GPU 7 is CORRECT!! :)
```
