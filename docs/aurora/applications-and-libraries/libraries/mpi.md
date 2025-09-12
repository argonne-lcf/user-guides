# Aurora MPICH

## Enabling coredump on MPI_Abort()

When running into an unexpected MPI_Abort() it may be useful to produce a core file to debug futher. Setting the environment variable `MPIR_CVAR_COREDUMP_ON_ABORT=1` causes mpich to call abort(): https://github.com/pmodels/mpich/blob/aurora/src/mpid/ch3/src/mpid_abort.c#L63C9-L63C36.

Thus to produce a core file, set the soft core ulimit to unlimited (defaults to 0 or disabled), set the environment variable `MPIR_CVAR_COREDUMP_ON_ABORT=1`, and share the environment variables (`--genvall` or simlar `--genv` arguments) and ulimit (`--rlimit`) to the MPI ranks like so:

```
jdoe@x4117c3s3b0n0:~> ulimit -S -c unlimited
jdoe@x4117c3s3b0n0:~> MPIR_CVAR_COREDUMP_ON_ABORT=1 mpiexec --genvall --rlimit CORE ./mpi_abort
Abort(15) on node 0 (rank 0 in comm 0): application called MPI_Abort(MPI_COMM_WORLD, 15) - process 0
Abort(15) on node 1 (rank 1 in comm 0): application called MPI_Abort(MPI_COMM_WORLD, 15) - process 1
x4117c3s3b0n0.hsn.cm.aurora.alcf.anl.gov: rank 0 died from signal 6 and dumped core
```

You'll find that mpiexec reports that a core file was dumped if enabled successfully. The core file will be found in the current working directory (CWD) of the MPI rank, and will be named with the pattern: `core.<hostname>.<pid>`.
