# Polaris System Updates

## 2024-04-22

We will have a multi-day outage on Polaris to upgrade management software to HPCM 1.10 from April 22-25, 2024.
These version changes will take place with the upgrade to HPCM 1.10:

- HPE Cray Programming Environment (CPE) 23.12
- NVIDIA SDK 23.9
- NVIDIA driver version 535.154.05
- CUDA 12.2
- SUSE 15 SP5

We intend to keep Grand and Eagle operational during the upgrade. 
Users should use Globus to access their project data. 
For more information, visit: https://docs.alcf.anl.gov/data-management/data-transfer/using-globus/


### Changes to ```/soft``` & ```/modules```

More to come

### Spack

We have newly installed Spack deployments in `/soft`. Spack is an HPC-oriented
package manager which ALCF uses to install software for the user environment.
However, no knowledge of Spack is necessary to use these software offerings. All
ALCF-managed software is accessible to users via modules.

The base suite of software tools and libraries can be accessed by loading the
`spack-pe-base` module. This adds a path to `$MODULEPATH` which contains
numerous modules that can be viewed with `module avail`. There are also
many hidden modules which can be viewed with `module --show-hidden avail`; these
hidden modules are dependencies of installed packages which can be loaded
independently.

For example, to load `cmake` starting from the default environment, a user would
run the following commands:
```
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```
Running `module avail` would reveal further available modules. 

In addition to the base suite, a suite of higher-level libraries are installed
in the `spack-pe-gnu` module. These are built with and are dependent on
`PrgEnv-gnu`. A `PrgEnv-nvidia`-compatible stack will be available in the
future.

Note that not all software is installed through Spack; many applications and
libraries will still be installed as standalone packages in `/soft`. Users are
encouraged to browse the available modules with `module avail` to see what
software is installed on the system.
