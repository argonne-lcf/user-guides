# Spack

Spack is a package manager developed for HPC which supports combinatorial versioning, i.e. it allows for multiple versions of packages to be built. These builds can vary with canonical version number, build options, compilers, and processor architectures. Each of a package’s dependencies can be similarly versioned, and a built version is fully specified by a concretized spack spec, and referenced by a hash generated from the specified options.

The learning curve for spack can be steep and new users are encouraged to experiment with their own installation of spack (easily available for cloning at [https://github.com/spack/spack](https://github.com/spack/spack)) and to look at configuration settings used in the ALCF installation for hints as to what settings may be appropriate and useful. 

Example settings for Theta are available at /soft/spack/alcf/theta. Not all of these settings will be useful for all builds and some may foil spack’s concretization process, and it is not recommended to adopt these wholesale as global settings. The recommended method is to include these settings ad hoc in a spack environment to more tightly control what information spack’s concretizer uses for its builds.

There are a growing number of packages installed to the ALCF spack instance at /soft/spack/root, and this instance can be used as a spack upstream resource as described here:

[https://spack.readthedocs.io/en/latest/chain.html#using-multiple-upstream-spack-instances](https://spack.readthedocs.io/en/latest/chain.html#using-multiple-upstream-spack-instances). Pointing to the ALCF spack instance as an upstream repository is trivial. Simply create an ‘upstreams.yaml’ file in any of the configuration scopes (e.g. $SPACK_ROOT/etc/spack/upstreams.yaml) will allow your instance to leverage any builds in the upstream, and will appear when running ‘spack find’.

An example upstreams.yaml:
```
upstreams: 
    alcf-spack: 
        install_tree: /soft/spack/root/opt/spack
```
Support requests and feedback for ALCF-specific issues should be directed to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). For general spack questions, users are encouraged to consult the following resources:

- [Spack development website](https://github.com/spack/spack)
- [Spack documentation](https://spack.readthedocs.io/en/latest/index.html)
- [Spack tutorial](https://spack.readthedocs.io/en/latest/tutorial.html)
- [Spack slack](https://slack.spack.io)

