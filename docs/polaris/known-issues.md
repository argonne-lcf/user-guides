# Known Issues

This is a collection of known issues that have been encountered during Polaris's early user phase. Documentation will be updated as issues are resolved.

## Compiling & Running Applications

1. Since the Slingshot 11 and related software upgrade, users may encounter the following issue when running an application.

```
/opt/cray/pe/gcc-libs/libstdc++.so.6: version `GLIBCXX_3.4.29' not found (required by a.out)
```

At this time, it is suggested to update the `LD_PRELOAD` environment variable as follows.

```
export LD_PRELOAD=/opt/cray/pe/gcc/11.2.0/snos/lib64/libstdc++.so.6
```

2. With `PrgEnv-nvhpc/8.3.3`, if you are using `nvcc` to indirectly invoke `nvc++` and compiling C++17 code (as, for example, in building Kokkos via `nvcc_wrapper`), you will get compilation errors with C++17 constructs. See [our documentation on NVIDIA Compilers](./compiling-and-linking/nvidia-compiler-polaris.md#known-issues-and-workarounds) for a workaround.

3. `PrgEnv-nvhpc/8.3.3` currently loads the `nvhpc/21.9` module, which erroneously has the following lines:
```
setenv("CC","/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/compilers/bin/nvc")
setenv("CXX","/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/compilers/bin/nvc++")
setenv("FC","/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/compilers/bin/nvfortran")
setenv("F90","/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/compilers/bin/nvfortran")
setenv("F77","/opt/nvidia/hpc_sdk/Linux_x86_64/21.9/compilers/bin/nvfortran")
setenv("CC","cpp")
```
In particular, the final line can cause issues for C-based projects (e.g. CMake may complain because the `cpp` C preprocessor is not a compiler). We recommend running the following in such cases:
```
unset CC
unset F77
unset CXX
unset FC
unset F90
```

4. Cray MPICH may exhibit issues when MPI ranks call `fork()` and are distributed across multiple nodes. The process may hang or throw a segmentation fault. 

    In particular, this can manifest in hangs with PyTorch+Horovod with a `DataLoader` with multithreaded workers and distributed data parallel training on multiple nodes. We have built a module `conda/2022-09-08-hvd-nccl` which includes a Horovod built without support for MPI. It uses NCCL for GPU-GPU communication and Gloo for coordination across nodes.

    `export IBV_FORK_SAFE=1` may be a workaround for some manifestations of this bug; however it will incur memory registration overheads. It does not fix the hanging experienced with multithreaded dataloading in PyTorch+Horovod across multiple nodes with `conda/2022-09-08`, however (instead prompting a segfault). 

    This incompatibility also may affect Parsl; see details in the [Special notes for Polaris](./workflows/parsl.md#special-notes-for-polaris) section of the Parsl page.

## Profiling Applications

1. The `nsys` profiler packaged with `nvhpc/21.9` in some cases appears to be presenting broken timelines with start times not lined up. The issue does not appear to be present when `nsys` from `cudatoolkit-standalone/11.2.2` is used. We expect this to no longer be an issue once `nvhpc/22.5` is made available as the default version.

## Submitting Jobs

1. For batch job submissions, if the parameters within your submission script do not meet the parameters of any of the execution queues (`small`, ..., `backfill-large`) you might not receive the "Job submission" error on the command line at all, and the job will never appear in history `qstat -xu <username>` (current bug in PBS). E.g. if a user submits a script to the `prod` routing queue requesting 10 nodes for 24 hours, exceeding "Time Max" of 6 hrs of the `small` execution queue (which handles jobs with 10-24 nodes), then it may behave as if the job was never submitted. 

2. Job scripts are copied to temporary locations after `qsub` and any changes to the original script while the job is queued will not be reflected in the copied script. Furthermore, `qalter` requires `-A <allocation name>` when changing job properties. Currently, there is a request for a `qalter`-like command to trigger a re-copy of the original script to the temporary location. 
