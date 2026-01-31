# CUDA-GDB

## Introduction
[CUDA-GDB](https://docs.nvidia.com/cuda/cuda-gdb/index.html) is the NVIDIA tool for debugging CUDA applications running on Polaris. CUDA-GDB is an extension to [GDB](https://www.sourceware.org/gdb/), the GNU Project debugger. The tool provides developers with a mechanism for debugging CUDA applications running on actual hardware. This enables developers to debug applications without the potential variations introduced by simulation and emulation environments.

## Step-by-step guide
### Compilation
NVCC, the NVIDIA CUDA compiler driver, provides a mechanism for generating the debugging information necessary for CUDA-GDB to work properly. The `-g -G` option pair must be passed to NVCC when an application is compiled for ease of debugging with CUDA-GDB. For example:
```bash
nvcc -arch=sm_80 -g -G foo.cu -o foo
```
Using this line to compile the CUDA application `foo.cu`:

- Forces `-O0` compilation, with the exception of very limited dead-code eliminations and register-spilling optimizations.
- Makes the compiler include debug information in the executable.

### Running CUDA-GDB on Polaris compute nodes
Start an interactive job mode on Polaris with the following command:
```console
$ qsub -A <account> -q debug -I -l select=1,walltime=0:60:00,filesystems=swift

$ cuda-gdb --version
exec: /opt/nvidia/hpc_sdk/Linux_x86_64/25.5/cuda/12.9/bin/cuda-gdb-minimal --version
NVIDIA (R) cuda-gdb 12.9
Portions Copyright (C) 2007-2025 NVIDIA Corporation
Based on GNU gdb 14.2
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

$ cuda-gdb foo
```
## Example
https://github.com/UoB-HPC/BabelStream/tree/main/src/cuda
```console "CUDA STREAM benchmark"

user@polaris-login-02:~> git clone https://github.com/UoB-HPC/BabelStream.git

user@polaris-login-02:~> qsub -A <account> -q debug -I -l select=1,walltime=0:60:00,filesystems=swift
qsub: waiting for job 308834.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov to start
qsub: job 308834.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov ready


user@x3008c0s13b1n0:~> cd BabelStream; mkdir build_polaris_debug; cd build_polaris_debug
user@x3008c0s13b1n0:~> module list

Currently Loaded Modules:
  1) nvidia/25.5        6) cray-pals/1.7.1                      11) PrgEnv-nvidia/8.6.0
  2) craype/2.7.35      7) cray-libpals/1.7.1                   12) libfabric/2.2.0rc1
  3) cray-dsmml/0.3.1   8) craype-x86-milan                     13) craype-network-ofi
  4) cray-mpich/9.0.1   9) darshan/3.4.4                        14) perftools-base/25.09.0
  5) cray-pmi/6.1.16   10) xalt/3.2.1-1-g67d304d4-202511121555
  
user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -arch=sm_80 -g -G -c ../src/cuda/CUDAStream.cu  -I ../src/
user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -arch=sm_80 -g -G -c ../src/main.cpp -DCUDA -I ../src/cuda/ -I ../src/
user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> nvcc -arch=sm_80 -g -G main.o CUDAStream.o -o cuda-stream-debug

user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> ./cuda-stream-debug
BabelStream
Version: 5.0
Implementation: CUDA
Running kernels 100 times
Precision: double
Array size: 268.4 MB (=0.3 GB)
Total size: 805.3 MB (=0.8 GB)
Using CUDA device NVIDIA A100-SXM4-40GB
Driver: 12080
Memory: DEFAULT
Reduction kernel config: 432 groups of (fixed) size 1024
Init: 0.060838 s (=13236.977099 MBytes/sec)
Read: 0.000967 s (=832573.138279 MBytes/sec)
Function    MBytes/sec  Min (sec)   Max         Average
Copy        1329066.043 0.00040     0.00046     0.00046
Mul         1309894.920 0.00041     0.00047     0.00046
Add         1289647.777 0.00062     0.00068     0.00068
Triad       1304742.056 0.00062     0.00068     0.00068
Dot         1117596.303 0.00048     0.00055     0.00053

user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug> cuda-gdb ./cuda-stream-debug

NVIDIA (R) cuda-gdb 12.9
Portions Copyright (C) 2007-2025 NVIDIA Corporation
Based on GNU gdb 14.2
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This CUDA-GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://forums.developer.nvidia.com/c/developer-tools/cuda-developer-tools/cuda-gdb>.
Find the CUDA-GDB manual and other documentation resources online at:
    <https://docs.nvidia.com/cuda/cuda-gdb/index.html>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./cuda-stream-debug...

(cuda-gdb) b CUDAStream.cu:219
Breakpoint 1 at 0x413730: CUDAStream.cu:219. (2 locations)
(cuda-gdb) r
Starting program: /home/felker/BabelStream/build/cuda-stream-debug
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".
BabelStream
Version: 5.0
Implementation: CUDA
Running kernels 100 times
Precision: double
Array size: 268.4 MB (=0.3 GB)
Total size: 805.3 MB (=0.8 GB)
[New Thread 0x155550a7f000 (LWP 3037800)]
[New Thread 0x15555087b000 (LWP 3037801)]
[Detaching after fork from child process 3037802]
[New Thread 0x15555032c000 (LWP 3037822)]
Using CUDA device NVIDIA A100-SXM4-40GB
Driver: 12080
Memory: DEFAULT
Reduction kernel config: 432 groups of (fixed) size 1024
[Switching focus to CUDA kernel 0, grid 5, block (0,0,0), thread (0,0,0), device 0, sm 0, warp 1, lane 0]

CUDA thread hit Breakpoint 1.2, triad_kernel<double><<<(32768,1,1),(1024,1,1)>>> (a=0x155506000000,
    b=0x1554f6000000, c=0x1554e6000000) at ../src/cuda/CUDAStream.cu:219
219	  a[i] = b[i] + scalar * c[i];

(cuda-gdb) c
Continuing.
[Switching focus to CUDA kernel 0, grid 5, block (209,0,0), thread (0,0,0), device 0, sm 101, warp 33, lane 0]

CUDA thread hit Breakpoint 1.2, triad_kernel<double><<<(32768,1,1),(1024,1,1)>>> (a=0x155506000000,
    b=0x1554f6000000, c=0x1554e6000000) at ../src/cuda/CUDAStream.cu:219
219	  a[i] = b[i] + scalar * c[i];

(cuda-gdb) info locals
i = 214016

(cuda-gdb) p b[i]
$1 = 0.040000000000000008

(cuda-gdb) p scalar
$2 = 0.40000000000000002

(cuda-gdb) p c[i]
$3 = 0.14000000000000001

(cuda-gdb) d 1

(cuda-gdb) c
Continuing.
Init: 0.110691 s (=7275.271268 MBytes/sec)
Read: 0.101447 s (=7938.200068 MBytes/sec)
Function    MBytes/sec  Min (sec)   Max         Average
Copy        1176329.522 0.00046     0.00052     0.00046
Mul         1167004.126 0.00046     0.00049     0.00046
Add         1209390.261 0.00067     0.00070     0.00067
Triad       1212016.778 0.00066     0.00070     0.00067
Dot         1010647.124 0.00053     0.00055     0.00054
[Thread 0x15555087b000 (LWP 3037801) exited]
[Thread 0x15555032c000 (LWP 3037822) exited]
[Thread 0x155550a7f000 (LWP 3037800) exited]
[Inferior 1 (process 3037796) exited normally]

(cuda-gdb) q

user@x3008c0s13b1n0:~/BabelStream/build_polaris_debug>
```
