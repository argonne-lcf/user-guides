---
description: "Build codes on Crux: Cray PE wrappers, PrgEnv modules, and compiling on compute nodes."
---

# Compiling and Linking on Crux

## Overview

Crux has AMD EPYC 7543 (Milan) processors on the login nodes (`crux-uan-0001`, `crux-uan-0002`) and dual AMD EPYC 7742 (Rome) processors on the compute nodes (see [Machine Overview](../index.md)). The login nodes can be used to compile software and launch jobs. For large parallel builds, compile on a compute node to avoid impacting other users on the shared login nodes.

To request an interactive compute node for compiling:

```bash
qsub -I -l select=1 -l walltime=01:00:00 -l filesystems=home:eagle -q debug -A <project>
```

See also [Running Jobs on Crux](../queueing-and-running-jobs/running-jobs.md) for queue limits and [Running Jobs with PBS at the ALCF](../../running-jobs/index.md) for the full `qsub` syntax.

The default programming environment on Crux is `PrgEnv-cray` (CPE 26.03, CCE 21.0, `craype/2.7.36`, `cray-mpich/9.1.0` with `craype-x86-rome` and `craype-network-ofi`). The GNU programming environment `PrgEnv-gnu` is also available. Use the Cray MPI wrappers for MPI applications:

- `cc` — C compiler
- `CC` — C++ compiler
- `ftn` — Fortran compiler

Each wrapper selects the vendor compiler for the loaded `PrgEnv`. The following options help inspect what the wrapper invokes:

- `--craype-verbose`: print the underlying compiler command
- `--cray-print-opts=libs`: print library information
- `--cray-print-opts=cflags`: print include information

Further documentation is available via `man cc` and similar. Additional software installed in `/soft` is exposed via `module use /soft/modulefiles` (see [Getting Started](../getting-started.md)).

## Modules on Crux

Available modules can be listed via:

```bash
module avail
```

Loaded modules in your environment can be listed via:

```bash
module list
```

To load a new module, use:

```bash
module load <module_name>
```

Available targets such as `craype-x86-rome` are loaded by default; see `module avail craype` for alternatives.
