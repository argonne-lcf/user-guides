# Spack PE

The Spack PE is a software stack which provides various build tools, utilities, and libraries. The Spack PE consists of two parts: `spack-pe-gcc` and `spack-pe-oneapi`. `spack-pe-gcc` contains commonly used software packages compiled with GCC. `spack-pe-oneapi` is based on the [E4S Project](https://e4s-project.github.io/) and provides performant HPC libraries built with the OneAPI PE. `spack-pe-oneapi` is dependent on both `spack-pe-gcc` and the OneAPI PE. 

## Using software from the Spack PE

Currently the Spack PE is installed in ```/soft```. Packages in the Spack PE can be accessed via modulefile:

```
$ module use /soft/modulefiles
$ module load spack-pe-gcc
$ module load cmake
$ which cmake
/soft/packaging/spack/gcc/0.5-rc1/install/linux-sles15-x86_64/gcc-11.4.0/cmake-3.26.4-jiwocshcdaghfyjb6jzyhj7zyorgkfkh/bin/cmake
```

The above example loads `cmake` into the current environment. The `spack-pe-gcc` module adds paths to the user's `MODULEPATH`; individual packages are subsequently loaded through the newly available modules. The full list of available packages can be viewed by running `module avail`. Packages are loaded in the same way from `spack-pe-oneapi`, with the caveat that `spack-pe-oneapi` is tied to specific versions of `oneapi`.


## Inspecting packages

When a module within the Spack PE is loaded, several environment variables are updated to integrate the package into the user's environment. Additionally, the `PACKAGE_ROOT` variable is set to contain the path to the installation prefix of the package. For example, continuing from the `cmake` example above:

```
$ echo $CMAKE_ROOT
/soft/packaging/spack/gcc/0.5-rc1/install/linux-sles15-x86_64/gcc-11.4.0/cmake-3.26.4-jiwocshcdaghfyjb6jzyhj7zyorgkfkh
$ ls $CMAKE_ROOT
bin  doc  share
```

This variable can be used to inspect software installations and find header or library paths. Additionally, Spack packages have a `.spack` directory in the installation prefix which contains build logs and information on configure and build options.

## Building software with Spack

[Spack](https://spack.io/about/) is a powerful package manager designed for HPC. The Spack PE is installed and managed with Spack; users can also install Spack in their own home or project directory to manage their software builds. Spack has a steep learning curve, but it may benefit workflows involving frequent builds with complex dependencies.

For users who wish to use Spack to install their own software, we provide configuration files corresponding to the Spack PE deployments. These configuration files can be found in `/soft/packaging/spack/settings`, organized into directories by Aurora PE and Spack PE versions. Not all of these settings will be useful for all builds and it is not recommended to adopt these wholesale as global settings. The recommended method is to include these settings ad hoc in a spack environment to control what information spack uses for its builds. However, we highly recommend using or adapting the `aurora_packages_{spack-pe-version}.yaml` and `compilers_{spack-pe-version}.yaml` configurations, since these configurations are essential for Spack to recognize the OneAPI compiler and associated Aurora PE components such as MPICH.

