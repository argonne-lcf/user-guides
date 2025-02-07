# Spack PE

Spack is an HPC-oriented package manager that ALCF uses to install software for the user environment.

Users should depend on libraries in the Spack PE over libraries on the system when possible, as system libraries may slightly differ between compute nodes and login nodes.

ALCF's Spack PE is a Spack-managed software stack that provides various build tools, utilities, and libraries. It consists of a base stack (`spack-pe-base`) and PrgEnv-dependent stacks (currently `spack-pe-gnu`).

`spack-pe-base` contains commonplace software compiled for CPU with the system GCC compilers. Accordingly, the software in `spack-pe-base` can be used regardless of the programming environment.

`spack-pe-gnu` is based on the [E4S Project](https://e4s-project.github.io/) and provides performant HPC libraries built with `PrgEnv-gnu` and the `nvcc` CUDA compiler driver for GPU code. `spack-pe-gnu` is dependent on both `spack-pe-base` and `PrgEnv-gnu`.

## Using software from the Spack PE

The base suite of software tools and libraries can be accessed by loading the `spack-pe-base` module. This adds a path to `$MODULEPATH` which contains numerous modules.

For example, to load `cmake` starting from the default environment, a user should run the following commands:

```
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```

The `spack-pe-base` module adds paths to the user's `MODULEPATH`; individual packages are subsequently loaded through the newly available modules. The full list of available packages can be viewed by running `module avail` or `module --show-hidden avail` for a complete listing. Packages are loaded in the same way from `spack-pe-gnu`.

## Inspecting packages

When a module in the Spack PE is loaded, several environment variables are updated to integrate the package into the user's environment. Additionally, the `PACKAGE_ROOT` variable is set to the path to the installation prefix of the package. For example, after loading `cmake` as above:

```
$ echo $CMAKE_ROOT
/soft/spack/gcc/0.6.1/install/linux-sles15-x86_64/gcc-12.3.0/cmake-3.27.7-a435jtzvweeos2es6enirbxdjdqhqgdp/
$ ls -a $CMAKE_ROOT
.  ..  bin  doc  share  .spack
```

This variable can be used to inspect software installations and find header or library paths. Additionally, Spack packages have a `.spack` directory in the installation prefix which contains build information and logs.

## Building software with Spack

[Spack](https://spack.io/about/) is a powerful package manager designed for HPC. The Spack PE is installed and managed with Spack; users can also install Spack in their own home or project directory to manage their software builds. Spack has a steep learning curve, but it may benefit workflows involving frequent builds with complex dependencies.

For users who wish to use Spack to install their own software, we provide configuration files corresponding to the Spack PE deployments. These configuration files can be found in `config` directories in `/soft/spack` within the respective Spack PE installation directories. For example, the `spack-pe-base/0.6.1` configurations are in `/soft/spack/gcc/0.6.1/config`. Not all of these settings will be useful for all builds, and it is not recommended to adopt these wholesale as global settings. The recommended method is to include these settings ad hoc in a Spack environment to control what information Spack uses for its builds.

Support requests and feedback should be directed to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). For general Spack questions, users are encouraged to consult the following resources:

- [Spack development website](https://github.com/spack/spack)
- [Spack documentation](https://spack.readthedocs.io/en/latest/index.html)
- [Spack tutorial](https://spack-tutorial.readthedocs.io/en/latest)
- [Spack Slack channel](https://slack.spack.io)