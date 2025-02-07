# CUDA_VISIBLE_DEVICES
The aforementioned environment variable can be used to set which GPU devices are visible to an application and the order in which they appear. The default in examples below below will use all available devices present on a compute node. This would be 8 in 'full-node' runs and 1 in 'single-gpu' runs. In full-node runs, this number can be altered as in the following example.
```
export CUDA_VISIBLE_DEVICES=0,1
./submit.sh
```
## Example output from two GPUs
```
# of devices detected= 2

Checking P2P Access
Device i= 0
  --  Device j= 1  access= 1
Device i= 1
  --  Device j= 0  access= 1
P2P Access is enabled

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

 GPU-GPU Reduction  il= 0  ngpus_active= 2  ngpu_recv= 1
    1 --> 0

  r[0]= 2.000000  host_result= 2.000000

Result from all GPUs is CORRECT!! :)
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

Checking P2P Access
Device i= 0
  --  Device j= 1  access= 1
  --  Device j= 2  access= 1
  --  Device j= 3  access= 1
  --  Device j= 4  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 1
  --  Device j= 0  access= 1
  --  Device j= 2  access= 1
  --  Device j= 3  access= 1
  --  Device j= 4  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 2
  --  Device j= 0  access= 1
  --  Device j= 1  access= 1
  --  Device j= 3  access= 1
  --  Device j= 4  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 3
  --  Device j= 0  access= 1
  --  Device j= 1  access= 1
  --  Device j= 2  access= 1
  --  Device j= 4  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 4
  --  Device j= 0  access= 1
  --  Device j= 1  access= 1
  --  Device j= 2  access= 1
  --  Device j= 3  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 5
  --  Device j= 0  access= 1
  --  Device j= 1  access= 1
  --  Device j= 2  access= 1
  --  Device j= 3  access= 1
  --  Device j= 4  access= 1
  --  Device j= 6  access= 1
  --  Device j= 7  access= 1
Device i= 6
  --  Device j= 0  access= 1
Device i= 7
  --  Device j= 0  access= 1
  --  Device j= 1  access= 1
  --  Device j= 2  access= 1
  --  Device j= 3  access= 1
  --  Device j= 4  access= 1
  --  Device j= 5  access= 1
  --  Device j= 6  access= 1
P2P Access is enabled

  [0] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [1] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [2] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [3] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [4] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [5] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [6] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]
  [7] Platform[ Nvidia ] Type[ GPU ] Device[ NVIDIA A100-SXM4-40GB ]

Running on 8 GPUs!
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

 GPU-GPU Reduction  il= 0  ngpus_active= 8  ngpu_recv= 4
    4 --> 0
    5 --> 1
    6 --> 2
    7 --> 3

 GPU-GPU Reduction  il= 1  ngpus_active= 4  ngpu_recv= 2
    2 --> 0
    3 --> 1

 GPU-GPU Reduction  il= 2  ngpus_active= 2  ngpu_recv= 1
    1 --> 0

  r[0]= 8.000000  host_result= 8.000000

Result from all GPUs is CORRECT!! :)
```
# Compilation w/ NVHPC SDK & ALCF provided OpenMPI
```
qsub -I -n 1 -t 60 -q single-gpu -A Catalyst --attrs filesystems=home,grand,eagle

module load nvhpc-nompi
module switch openmpi openmpi/openmpi-4.0.5_ucx-1.10.0_nvhpc-21.7

make -f Makefile.nvhpc clean
make -f Makefile.nvhpc

./submit.sh
```
