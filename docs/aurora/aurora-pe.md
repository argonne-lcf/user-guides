# Aurora Programming Environment

## Overview

The Aurora Programming Environment (Aurora PE) is Aurora's default software environment and consists of the OneAPI, MPICH, and the Spack PE. The Aurora PE is loaded in the user environment through a default module set. Alternative versions of the Aurora PE, as well as Aurora PE components not loaded by default, are available through the module interface.

As of May 2024, the default modules are
```
  1) spack-pe-gcc/0.6.1-23.275.2   7) mpich-config/collective-tuning/1024
  2) gmp/6.2.1-pcxzkau             8) intel_compute_runtime/release/agama-devel-736.25
  3) mpfr/4.2.0-w7v7yjv            9) oneapi/eng-compiler/2023.12.15.002
  4) mpc/1.3.1-dfagrna            10) libfabric/1.15.2.0
  5) gcc/12.2.0                   11) cray-pals/1.3.3
  6) mpich/icc-all-pmix-gpu/52.2  12) cray-libpals/1.3.3
```
Besides the latter three modules, this set of modules is loaded from the Aurora PE. The `oneapi`, `intel_compute_runtime`, and `mpich` modules are part of version `23.275.2` of the Aurora PE. Each version of the PE may have multiple compilers, runtimes, and/or MPICH installations; these are interchangeable within a particular PE version. In addition to the OneAPI and MPICH installations, the Aurora PE contains the Spack PE, which contains a myriad of general and scientific computing software. See the [Spack PE page](./applications-and-libraries/libraries/spack-pe.md) for more details.

## Switching PE or SDK versions

Aurora PE and OneAPI SDK versions can be viewed with `module avail` and switched with `module load` commands. The Aurora PE modules utilize Lmod's hierarchical module system to allow seamless switching between versions of the Aurora PE and its components. For example, if the user loads a different version of the `oneapi` SDK, the other modules in the Aurora PE, such as `intel_compute_runtime`, `mpich`, and `spack-pe-gcc`, will be reloaded to guarantee compatibility. In short, the hierarchical modulefile system ensures that the module environment is self-consistent with minimal user input.

## Aurora PE and /soft 

The Aurora PE is installed in `/opt/aurora` and is mounted as a read-only squashfs. `/soft` is also available to provide software not present in the Aurora PE. Modules in `/soft/modulefiles` are not part of the default environment in order to avoid filesystem metadata overhead, which can have significant performance impacts. Users wishing to use software in `/soft` will need to first run `module use /soft/modulefiles` to access the modules.

The Aurora PE has a longer-term upgrade cadence, so ad-hoc software requests will be fulfilled through software installations in `/soft`. Such software installations will be considered for incorporation into the Spack PE during upgrade cycles.

Modules in `/soft` may conflict with modules in the Aurora PE. Lmod has some limitations in automatically handling module conflicts, so the user may need to manually resolve module conflicts arising from modules outside of the Aurora PE. In order to reduce the likelihood of accidental module clashes, a separate `preview-modules/<PE_VERSION>` modulepath is available for each Aurora PE version which contains modules that directly conflict with a given PE version.
