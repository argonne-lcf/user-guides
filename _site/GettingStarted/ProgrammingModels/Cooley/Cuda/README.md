Cooley has Nvidia Kepler K80 GPUs (compute capability `sm_35`).
The last version of CUDA support on Kepler is 10.2.

On Cooley we have CUDA toolkit installed up to 10.0.
Here is the full list.
```
    +cuda-4.2.9                    CUDA 4.2.9
    +cuda-5.0.35                   CUDA 5.0.35
    +cuda-5.5.22                   CUDA 5.5.22
    +cuda-6.0.37                   CUDA 6.0.37
    +cuda-6.5.14                   CUDA 6.5.14
    +cuda-7.0.28                   CUDA 7.0.28
    +cuda-7.5.18                   CUDA 7.5.18
    +cuda-8.0.61                   CUDA 8.0.61
    +cuda-9.0.176                  CUDA 9.0.176
    +cuda-9.1                      CUDA 9.1
    +cuda-10.0                     CUDA 10.0
```
Users can load a softkey by `soft add +cuda-10.0` or put the softkey in the .soft.cooley file.

nvcc compiler relies on a host compiler which can be GCC/LLVM/Intel Compiler listed on https://docs.nvidia.com/cuda/archive/10.0/cuda-installation-guide-linux/index.html.
Load these compiler as needed.

The K80 GPU has two phisical GPU dies and each one has its dedicated memory.
On each compute node, they are treated as two distinct GPUs by the operating system.
If applications cannot utilize more than one GPU per process, it is recommended to assign one GPU per at least one MPI process.
A typical example is an application using MPI + OpenMP + CUDA, maximal node utilization can be achieved with 
one MPI process per socket which holds 6 OpenMP threads and 1 GPU per process.
However, the two K80 GPUs share the same PCI-express X16 connection to the host CPU on one socket,
the best process, thread, GPU affinity for maxmial throughput depends on the application computational pattern.
