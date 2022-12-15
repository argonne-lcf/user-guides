# Known Issues

This is a collection of known issues that have been encountered during Polaris's early user phase. Documentation will be updated as issues are resolved.

1. The `nsys` profiler packaged with `nvhpc/21.9` in some cases appears to be presenting broken timelines with start times not lined up. The issue does not appear to be present when `nsys` from `cudatoolkit-standalone/11.2.2` is used. We expect this to no longer be an issue once `nvhpc/22.5` is made available as the default version.

2. With `PrgEnv-nvhpc/8.3.3`, if you are using `nvcc` to indirectly invoke `nvc++` and compiling C++17 code (as, for example, in building Kokkos via `nvcc_wrapper`), you will get compilation errors with C++17 constructs. See [our documentation on NVIDIA Compilers](./compiling-and-linking/nvidia-compiler-polaris.md#known-issues-and-workarounds) for a workaround.

3. Cray MPICH may exhibit issues when MPI ranks call `fork()` and are distributed across multiple nodes. The process may hang or throw a segmentation fault. 

In particular, this can manifest in hangs with PyTorch+Horovod with a `DataLoader` with multithreaded workers and distributed data parallel training on multiple nodes. We have built a module `conda/2022-09-08-hvd-nccl` which includes a Horovod built without support for MPI. It uses NCCL for GPU-GPU communication and Gloo for coordination across nodes.

`export IBV_FORK_SAFE=1` may be a workaround for some manifestations of this bug; however it will incur memory registration overheads. It does not fix the hanging experienced with multithreaded dataloading in PyTorch+Horovod across multiple nodes with `conda/2022-09-08`, however (instead prompting a segfault). 

This incompatibility also may affect Parsl; see details in the [Special notes for Polaris](./workflows/parsl.md#special-notes-for-polaris) section of the Parsl page.
