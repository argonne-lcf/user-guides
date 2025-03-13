# Aurora Programming Environment

## Overview

The Aurora Programming Environment (Aurora PE) is Aurora's default software environment and consists of the OneAPI SDK, MPICH, and the Spack PE. The Aurora PE is loaded in the user environment through a default module set. Alternative versions of the Aurora PE, as well as Aurora PE components not loaded by default, are available through the module interface. Each version of the PE may have multiple compilers, runtimes, and/or MPICH installations which are interchangeable via modules. In addition to the OneAPI and MPICH installations, the Aurora PE contains the Spack PE, a software stack which includes general-purpose and scientific computing packages. See the [Spack PE page](./applications-and-libraries/libraries/spack-pe.md) for more details.

## Switching PE or SDK versions

Aurora PE and OneAPI SDK versions can be viewed with `module avail` and switched with `module load` commands. The Aurora PE modules utilize Lmod's hierarchical module system to allow seamless switching between versions of the Aurora PE and its components. For example, if the user loads a different version of the `oneapi` module, other loaded modules from the Aurora PE, such as `intel_compute_runtime`, `mpich`, and packages from the Spack PE, will be reloaded to guarantee compatibility. In short, the hierarchical modulefile system ensures that the module environment is self-consistent with minimal user intervention.

## Aurora PE and /soft

The Aurora PE is installed in `/opt/aurora` and is mounted as a read-only squashfs. `/soft` is also available to provide software not present in the Aurora PE. However, loading libraries from shared filesystems like `/soft` can cause performance issues at scale. Modules in `/soft/modulefiles` are not part of the default environment in order to avoid adverse impacts. Users wishing to use software in `/soft` will need to first run `module use /soft/modulefiles` to access the modules.

The Aurora PE has a longer-term upgrade cadence (on the order of months), so ad-hoc software requests will be fulfilled through software installations in `/soft`. `/soft` installations will be considered for incorporation into the Aurora PE during upgrade cycles.

Modules in `/soft` may conflict with modules in the Aurora PE. Lmod has some limitations in automatically handling module conflicts, so the user may need to manually resolve conflicts arising from modules outside of the Aurora PE. Users are also advised to sanitize module paths that they add from other non-standard locations, such as `/home`, to avoid conflicts.
