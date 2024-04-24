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


### Changes to the user software environment

In addition to the system upgrades, several changes have been made to the user
software environment which may impact user workflows.

#### Older PE versions are deprecated
Older versions of the Cray PE are deprecated as they are incompatible with the
upgraded system stack.

#### `/soft` refresh and default `$MODULEPATH` change
Due to the new system software stack, `/soft` has been purged to allow for
software to be rebuilt. In addition, `/soft/modulefiles` is no longer in the
default `$MODULEPATH`. To access modules installed in `/soft`, users should run
`module use /soft/modulefiles`.

The following modules have been removed:

```
   aocl/3.2.0                                                        hpctoolkit/2023.03.27                                                    (D)
   aocl/4.0                                                   (D)    imagemagick/imagemagick-7.1.1-11
   ascent/develop/2024-01-08-492f9b0                                 kokkos/kokkos-3.6.01
   boost/1.80.0                                                      kokkos/3.7.00-cuda
   boost/1.81.0                                               (D)    kokkos/3.7.00-sycl
   cabana/cabana-20220723                                            kokkos/3.7.01-cuda
   cabana/PrgEnv-gnu/8.3.3/gnu/11.2.0/cuda_cudatoolkit_11.8.0 (D)    kokkos/4.2.00/shared/PrgEnv-gnu/8.3.3/gnu/11.2.0/cuda_cudatoolkit_11.8.0 (D)
   cmake/3.23.2                                                      llvm/release-15.0.0
   conda/2022-07-19                                                  llvm/release-16.0.0
   conda/2022-09-08-hvd-nccl                                         llvm/release-17.0.0                                                      (D)
   conda/2022-09-08                                                  magma/2.6.2
   conda/2023-01-10-unstable                                         magma/2.7.0                                                              (D)
   conda/2023-10-04-openmpi                                          mpiwrappers/cray-mpich-llvm
   conda/2023-10-04                                           (D)    mpiwrappers/cray-mpich-oneapi                                            (D)
   cudatoolkit-standalone/11.2.2                                     oneapi/release/2023.2.1
   cudatoolkit-standalone/11.4.4                                     oneapi/release/2024.0
   cudatoolkit-standalone/11.6.2                                     oneapi/upstream                                                          (D)
   cudatoolkit-standalone/11.7.1                                     paraview/paraview-5.11.1-mesa
   cudatoolkit-standalone/11.8.0                              (D)    paraview/paraview-5.11.2-EGL-test
   cudatoolkit-standalone/12.0.0                                     paraview/paraview-5.11.2-mesa
   e4s/22.05/mvapich2                                                paraview/paraview-5.12.0-RC1-mesa
   e4s/22.05/PrgEnv-gnu                                       (D)    paraview/paraview-5.12.0-mesa                                            (D)
   e4s/22.08/mvapich2                                                singularity/3.8.7
   e4s/22.08/PrgEnv-gnu                                       (D)    tau/2.31.1
   ffmpeg/ffmpeg-6.0                                                 tau/2.32
   forge/23.0.4                                                      tau/2.33.1                                                               (D)
   ginkgo/mpi/20230314/ginkgo                                        visit/visit
   ginkgo/20230314/ginkgo                                     (D)    vmd/vmd-1.9.4a55
   gnu-parallel/2021-09-22                                           xalt/3.0.1-202308151751
   gsl/2.7                                                           xalt/3.0.1-202308261842                                                  (D)
   hpctoolkit/2022.07.27
```

The following modules have been newly installed:

```
   cabana/dev-9a1ad605/kokv/4.2.01/PrgEnv-gnu/8.5.0/gnu/12.3/cuda_cudatoolkit_12.2.91
   cuda-PrgEnv-nvidia/12.2.91
   cudatoolkit-standalone/12.2.2                                                      (D)
   cudatoolkit-standalone/12.3.2
   cudatoolkit-standalone/12.4.0
   cudnn/9.0.0
   forge/23.1.2
   kokkos/4.2.01/shared/PrgEnv-gnu/8.5.0/gnu/12.3/cuda_cudatoolkit_12.2.91
   spack-pe-base/0.6.1
   spack-pe-gnu/0.6.1
```

Note that `spack-pe-base` and `spack-pe-gnu` are metamodules which contain
further software offerings. See the [Spack](#spack) section below for details.

#### `/soft` is mounted read-only
`/soft` is now mounted as a read-only filesystem. This means that users can no
longer directly install software onto `/soft` on Polaris. Authorized users
should install and test software on `/soft` on Sirius prior to requesting a
`/soft` sync with Polaris.

### Spack

We have newly installed Spack deployments in `/soft`. Spack is an HPC-oriented
package manager which ALCF uses to install software for the user environment.
However, no knowledge of Spack is necessary to use these software offerings. All
ALCF-managed software is accessible to users via modules.

The base suite of software tools and libraries can be accessed by loading the
`spack-pe-base` module. This adds a path to `$MODULEPATH` which contains
numerous modules. 

For example, to load `cmake` starting from the default environment, a user
should run the following commands:
```
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```
Other modules in `spack-pe-base` can be browsed by running `module avail` or
`module --show-hidden avail`. The latter shows hidden modules which are
installed as dependencies of the un-hidden modules.

In addition to the base stack, a suite of higher-level libraries are installed
in the `spack-pe-gnu` module. These are built with and are dependent on
`PrgEnv-gnu`. A `PrgEnv-nvidia`-compatible stack will be available in the
future.

Note that not all software is installed through Spack; many applications and
libraries are installed as standalone packages in `/soft`. Users are encouraged
to browse the available modules with `module avail` to see what software is
installed on the system.
