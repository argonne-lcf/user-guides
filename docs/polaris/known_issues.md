#Known Issues

This is a collection of known issues that have been encountered during Polaris' early user phase. Documentation will be updated as issues are resolved.

1. Issues are encountered with GPU-enabled MPI applications communicating between nodes. Users are likely to encounter MPICH Errors and MPIDI_OFI_send_normal:Bad address type of errors in their applications. It is recommend to disable GPU-MPI for the time being if possible.

2. Switching to the `GNU` programming environment whereby the Cray MPI wrappers reference the `GNU` compilers `gcc`, `g++`, and `gfortran` and loading `NVIDIA` compilers currently requires manually loading the nvhpc module and then reloading `PrgEnv-gnu` as follows.
```
module swap PrgEnv-nvhpc PrgEnv-gnu
module load nvhpc
module swap PrgEnv-nvhpc PrgEnv-gnu
```