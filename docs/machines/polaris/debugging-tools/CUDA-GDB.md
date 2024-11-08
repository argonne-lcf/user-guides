# CUDA-GDB

## References  
[NVIDIA CUDA-GDB Documentation](https://docs.nvidia.com/cuda/cuda-gdb/index.html)  

## Introduction
CUDA-GDB is the NVIDIA tool for debugging CUDA applications running on Polaris. CUDA-GDB is an extension to GDB, the GNU Project debugger. The tool provides developers with a mechanism for debugging CUDA applications running on actual hardware. This enables developers to debug applications without the potential variations introduced by simulation and emulation environments.

## Step-by-step guide
### Debug Compilation
NVCC, the NVIDIA CUDA compiler driver, provides a mechanism for generating the debugging information necessary for CUDA-GDB to work properly. The -g -G option pair must be passed to NVCC when an application is compiled for ease of debugging with CUDA-GDB; for example,
```
nvcc -g -G foo.cu -o foo
```
Using this line to compile the CUDA application `foo.cu`
* forces `-O0` compilation, with the exception of very limited dead-code eliminations and register-spilling optimizations.
* makes the compiler include debug information in the executable


### Running CUDA-gdb on Polaris compute nodes
Start an interactive job mode on Polaris as follows:  
```
$ qsub -I -l select=1 -l walltime=1:00:00

$ cuda-gdb --version
NVIDIA (R) CUDA Debugger
11.4 release
Portions Copyright (C) 2007-2021 NVIDIA Corporation
GNU gdb (GDB) 10.1
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

$ cuda-gdb foo
```

## A quick example with a stream benchmark on a Polaris compute node

```
jkwack@polaris-login-02:~> qsub -I -l select=1 -l walltime=1:00:00
qsub: waiting for job 308834.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov to start
qsub: job 308834.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov ready


Currently Loaded Modules:
  1) craype-x86-rome          4) perftools-base/22.05.0   7) cray-dsmml/0.2.2   10) cray-pmi-lib/6.0.17  13) PrgEnv-nvhpc/8.3.3
  2) libfabric/1.11.0.4.125   5) nvhpc/21.9               8) cray-mpich/8.1.16  11) cray-pals/1.1.7      14) craype-accel-nvidia80
  3) craype-network-ofi       6) craype/2.7.15            9) cray-pmi/6.1.2     12) cray-libpals/1.1.7

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -g -G -c ../src/cuda/CUDAStream.cu  -I ../src/

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -g -G -c ../src/main.cpp -DCUDA -I ../src/cuda/ -I ../src/

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -g -G main.o CUDAStream.o -o cuda-stream-debug

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> ./cuda-stream-debug 
BabelStream
Version: 4.0
Implementation: CUDA
Running kernels 100 times
Precision: double
Array size: 268.4 MB (=0.3 GB)
Total size: 805.3 MB (=0.8 GB)
Using CUDA device NVIDIA A100-SXM4-40GB
Driver: 11040
Function    MBytes/sec  Min (sec)   Max         Average     
Copy        1313940.694 0.00041     0.00047     0.00047     
Mul         1302000.791 0.00041     0.00048     0.00047     
Add         1296217.720 0.00062     0.00070     0.00069     
Triad       1296027.887 0.00062     0.00070     0.00069     
Dot         823405.227  0.00065     0.00076     0.00075     

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> cuda-gdb ./cuda-stream-debug 
NVIDIA (R) CUDA Debugger
11.4 release
Portions Copyright (C) 2007-2021 NVIDIA Corporation
GNU gdb (GDB) 10.1
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./cuda-stream-debug...
(cuda-gdb) b CUDAStream.cu:203
Breakpoint 1 at 0x412598: CUDAStream.cu:203. (2 locations)
(cuda-gdb) r      
Starting program: /home/jkwack/BabelStream/build_polaris_debug/cuda-stream-debug 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".
BabelStream
Version: 4.0
Implementation: CUDA
Running kernels 100 times
Precision: double
Array size: 268.4 MB (=0.3 GB)
Total size: 805.3 MB (=0.8 GB)
[Detaching after fork from child process 58459]
[New Thread 0x15554c6bb000 (LWP 58475)]
Using CUDA device NVIDIA A100-SXM4-40GB
Driver: 11040
[New Thread 0x15554c4ba000 (LWP 58476)]
[Switching focus to CUDA kernel 0, grid 5, block (0,0,0), thread (0,0,0), device 0, sm 0, warp 3, lane 0]

Thread 1 "cuda-stream-deb" hit Breakpoint 1, triad_kernel<double><<<(32768,1,1),(1024,1,1)>>> (a=0x155506000000, b=0x1554f6000000, c=0x1554e6000000)
    at ../src/cuda/CUDAStream.cu:203
203   a[i] = b[i] + scalar * c[i];
(cuda-gdb) c
Continuing.
[Switching focus to CUDA kernel 0, grid 5, block (1,0,0), thread (0,0,0), device 0, sm 0, warp 32, lane 0]

Thread 1 "cuda-stream-deb" hit Breakpoint 1, triad_kernel<double><<<(32768,1,1),(1024,1,1)>>> (a=0x155506000000, b=0x1554f6000000, c=0x1554e6000000)
    at ../src/cuda/CUDAStream.cu:203
203   a[i] = b[i] + scalar * c[i];
(cuda-gdb) info locals
i = 1024
(cuda-gdb) p b[i]
$1 = 0.040000000000000008
(cuda-gdb) p scalar
$2 = 0.40000000000000002
(cuda-gdb) p c[i]
$3 = 0.14000000000000001
(cuda-gdb) d 1
(cuda-gdb) c
Continuing.
Function    MBytes/sec  Min (sec)   Max         Average     
Copy        1314941.553 0.00041     0.00041     0.00041     
Mul         1301022.680 0.00041     0.00042     0.00041     
Add         1293858.147 0.00062     0.00063     0.00063     
Triad       1297681.929 0.00062     0.00063     0.00062     
Dot         828446.963  0.00065     0.00066     0.00065     
[Thread 0x15554c4ba000 (LWP 58476) exited]
[Thread 0x15554c6bb000 (LWP 58475) exited]
[Inferior 1 (process 58454) exited normally]
(cuda-gdb) q

jkwack@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> 

```