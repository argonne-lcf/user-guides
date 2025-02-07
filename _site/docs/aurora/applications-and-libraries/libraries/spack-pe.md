# Spack PE

The Spack PE is a software stack that provides various build tools, utilities, and libraries. The Spack PE consists of two parts: `spack-pe-gcc` and `spack-pe-oneapi`. `spack-pe-gcc` contains commonly used software packages compiled for CPU. `spack-pe-oneapi` is based on the [E4S Project](https://e4s-project.github.io/) and provides performant HPC libraries built with the OneAPI SDK. `spack-pe-oneapi` depends on both `spack-pe-gcc` and the OneAPI SDK; in combination, the Spack PE with the OneAPI SDK and MPICH constitute the Aurora PE.

## Using software from the Spack PE

The Spack PE is loaded into the environment by default as part of the Aurora PE. To view the available modules, run `module avail`. A full listing of software, including hidden dependencies, can be viewed with `module --show-hidden avail`. The Spack PE modules will be in paths under `/opt/aurora/<AURORA_PE_VERSION>/spack`. These can be loaded like any other module, for example, with `module load cmake`.

## Inspecting packages

When a module within the Spack PE is loaded, several environment variables are updated to integrate the package into the user's environment. Additionally, the `PACKAGE_ROOT` variable is set to contain the path to the installation prefix of the package. For example, after loading `cmake`:

```bash
$ echo $CMAKE_ROOT
/opt/aurora/23.275.2/spack/gcc/0.6.1/install/linux-sles15-x86_64/gcc-12.2.0/cmake-3.27.7-mbl7dvgbiblpavhu53h5cheyrmpaikdz
$ ls -a $CMAKE_ROOT
.  ..  bin  doc  share  .spack
```

This variable can be used to inspect software installations. Additionally, Spack packages have a `.spack` directory in the installation prefix, which contains build logs and information on configure and build options.

## RPATH linking

Spack PE packages are built with `RPATH` linking. `RPATH` hardcodes a default search path for dynamic runtime linking of binaries. By setting `RPATH`, the loader only needs to search a single path for each library, reducing the number of filesystem calls performed when loading libraries. However, this means `LD_LIBRARY_PATH` will be ignored when loading binaries installed in the Spack PE. This has been set both to provide a performance benefit and to guarantee proper compatibility of linked libraries.

Software installed outside of the Spack PE tree, such as in `/soft`, will typically be installed with `RUNPATH` linking or with no runtime search path, both of which respect `LD_LIBRARY_PATH`. `RUNPATH` linking, like `RPATH`, hardcodes a default path, but it does not have precedence over `LD_LIBRARY_PATH`. Spack PE preview deployments in `/soft` are installed with `RUNPATH` linking.

## Building software with Spack

[Spack](https://spack.io/about/) is a powerful package manager designed for HPC. The Spack PE is installed and managed with Spack; users can also install Spack in their own home or project directory to manage their software builds. Spack has a steep learning curve, but it may benefit workflows involving frequent builds with complex dependencies.

For users who wish to use Spack to install their own software, we provide configuration files corresponding to the Spack PE deployments. These configuration files can be found in `/opt/aurora/<AURORA_PE_VERSION>/spack` in `config` directories organized by Spack PE version. Not all of these settings will be useful for all builds, and it is not recommended to adopt these wholesale as global settings. The recommended method is to include these settings ad hoc in a Spack environment to control what information Spack uses for its builds. However, we do recommend using the provided configurations for the compilers, OneAPI SDK components, and MPICH, as these can be difficult to configure properly.

Support requests and feedback for ALCF-specific issues should be directed to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). For general Spack questions, users are encouraged to consult the following resources:

- [Spack development website](https://github.com/spack/spack)
- [Spack documentation](https://spack.readthedocs.io/en/latest/index.html)
- [Spack tutorial](https://spack-tutorial.readthedocs.io/en/latest)
- [Spack Slack channel](https://slack.spack.io)