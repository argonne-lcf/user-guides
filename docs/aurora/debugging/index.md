# Debugging on Aurora - Overview

There are 3 debuggers available on Aurora:

1. [gdb-oneapi](./gdb-oneapi.md) - This is Intel's version of gdb augmented to allow debugging kernels executing on the PVC GPUs.
2. [DDT](./ddt-aurora.md) - The Linaro parallel debugger. This is the same parallel debugger that we have on Polaris. It supports a client-server mode and (via using `gdb-oneapi` internally) debugging kernels executing on the PVC GPUs.
3. [gdb4hpc](./gdb4hpc.md) - An alternative for CPU debugging only that will apply commands to all threads in the MPI process group.

There is also a correctness tool for Fortran:

1. [Codee](./codee.md) - This is a tool from Codee which can automatically analyze your code line-by-line to identify and fix opportunities for correctness, modernization, security and optimization. It is especially useful for legacy Fortran codes.


##  Preliminary steps

- You can use the module `mpich/dbg`. The module enables runtime checks during MPICH execution.
- The `mpich/dbg` module also allows you to run [valgrind](https://valgrind.org/) (from `module load valgrind`) as: `mpirun $OPT valgrind $BIN` 
- You can also recompile your code with `-fsanitize=address` to help detect memory access errors (will work for both CPU and GPU, see [Intel documentation](https://www.intel.com/content/www/us/en/developer/articles/technical/find-bugs-quickly-using-sanitizers-with-oneapi-compiler.html) for more details)
