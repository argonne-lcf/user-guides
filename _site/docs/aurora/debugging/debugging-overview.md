# Debugging on Aurora - Overview

There are 3 debuggers available on Aurora:

1. [gdb-oneapi](./gdb-oneapi.md) - This is Intel's version of gdb augmented to allow debugging kernels executing on the PVC GPUs.
2. [DDT](./ddt-aurora.md) - The Linaro parallel debugger. This is the same parallel debugger that we have on Polaris. It supports a client-server mode and (via using `gdb-oneapi` internally) debugging kernels executing on the PVC GPUs.
3. [gdb4hpc](./gdb4hpc.md) - An alternative for CPU debugging only that will apply commands to all threads in the MPI process group.