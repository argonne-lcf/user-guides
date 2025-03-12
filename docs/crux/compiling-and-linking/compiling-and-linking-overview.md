# Compiling and Linking on Crux

## Overview
Crux has AMD processors on the login nodes (crux-login-01,02) and AMD processors on the compute nodes (see [Machine Overview](../index.md) page). The login nodes can be used to compile software, create containers, and launch jobs. For larger, parallel builds, it will be beneficial to compile those directly on the compute nodes.

To launch an interactive job and acquire a compute node for compiling, use:

```bash
qsub -I -q workq -A myProjectShortName -n 1 -t HH:MM:SS
```

The default programming environment on the Crux compute nodes is currently Cray: `PrgEnv-cray`. The GNU programming environment `PrgEnv-gnu` is also available to users. It is recommended that the Cray MPI wrappers are used for building applications.

- `cc` - C compiler
- `CC` - C++ compiler
- `ftn` - Fortran compiler

Each of these wrappers will select the corresponding vendor compiler based on the PrgEnv module loaded in the environment. The following are some helpful options to understand what the compiler wrapper is invoking:

- `--craype-verbose`: Print the command which is forwarded to the compiler invocation
- `--cray-print-opts=libs`: Print library information
- `--cray-print-opts=cflags`: Print include information

Further documentation and options are available via `man cc` and similar.

## Modules on Crux

Available modules can be listed via the command:

```bash
module avail
```

Loaded modules in your environment can be listed via the command:

```bash
module list
```

To load new modules, use:

```bash
module load <module_name>
```
