# Polaris System Updates

## 2024-09-09

### XALT
The XALT library tracking software has been enabled for all Polaris users. More information can be found on the [XALT](./applications-and-libraries/libraries/xalt.md) page.

## 2024-04-22

The management software on Polaris has been upgraded to HPCM 1.10. The following version changes are in place with the upgrade to HPCM 1.10:

- HPE Cray Programming Environment (CPE) 23.12
- SlingShot version 2.1.2
- NVIDIA SDK 23.9
- NVIDIA driver version 535.154.05
- CUDA 12.2
- SUSE 15 SP5

### Releasing Jobs

Jobs that were queued before the upgrade have been restored to the appropriate queues but are placed on user hold. Jobs are not expected to complete successfully due to the changes made to the system and software environments resulting from the upgrade. We recommend you review your jobs and either release the hold (`qrls <jobid>`) or delete it (`qdel <jobid>`) and resubmit as appropriate.

- Users need to rebuild for the new PE environment and major OS upgrade. Existing binaries are unlikely to run successfully.
- We have held all jobs submitted prior to the upgrade as a user hold. Users may release their existing jobs with `qrls` to run after they have rebuilt their binaries.
- PBS does cache the job execution script. If a change to the script is required due to a path changing post-rebuild, the job will have to be resubmitted.
- All application binaries should be rebuilt prior to further job submissions.

### Re-building User Codes

Many user codes will need to be re-built and/or re-linked against the newer version of the programming environment (23.12) and Spack-provided dependencies.

### Changes to the User Software Environment

In addition to the system upgrades, several changes have been made to the user software environment which may impact user workflows.

#### Older PE Versions Removed

Older versions of the Cray PE (older than 23.12) are deprecated as they are incompatible with the upgraded system stack and are no longer available for use.

### Datascience Anaconda Module Updates

We have updated the datascience Anaconda module and built various packages and libraries with CUDA 12.4.1 to be compatible with the new Polaris NVIDIA GPU hardware driver (CUDA 12.2) and to use the latest MPI, NCCL, cuDNN, TensorRT, etc. libraries. PyTorch 2.3.0 and TensorFlow 2.16.1 are now available as part of this module.

To use the new environment, type:
```bash linenums="1"
module use /soft/modulefiles 
module load conda; conda activate
```

#### `/soft` Refresh and Default `$MODULEPATH` Change

Due to the new system software stack, `/soft` has been purged to allow for software to be rebuilt. In addition, `/soft/modulefiles` is no longer in the default `$MODULEPATH`. To access modules installed in `/soft`, users should run `module use /soft/modulefiles`.

Adding `module use /soft/modulefiles` to your profile should approximate the old behavior.

#### Modules Removed

The following modules have been removed:

```output
   aocl/3.2.0                                                        hpctoolkit/2022.07.27
   aocl/4.0                                                   (D)    hpctoolkit/2023.03.27                                                    (D)
   ascent/develop/2024-01-08-492f9b0                                 imagemagick/imagemagick-7.1.1-11
   boost/1.80.0                                                      kokkos/kokkos-3.6.01
   boost/1.81.0                                               (D)    kokkos/3.7.00-cuda
   cabana/cabana-20220723                                            kokkos/3.7.00-sycl
   cabana/PrgEnv-gnu/8.3.3/gnu/11.2.0/cuda_cudatoolkit_11.8.0 (D)    kokkos/3.7.01-cuda
   cmake/3.23.2                                                      kokkos/4.2.00/shared/PrgEnv-gnu/8.3.3/gnu/11.2.0/cuda_cudatoolkit_11.8.0 (D)
   conda/2022-07-19                                                  llvm/release-15.0.0
   conda/2022-09-08-hvd-nccl                                         llvm/release-16.0.0
   conda/2022-09-08                                                  magma/2.6.2
   conda/2023-01-10-unstable                                         magma/2.7.0                                                              (D)
   conda/2023-10-04-openmpi                                          mpiwrappers/cray-mpich-oneapi                                            (D)
   conda/2023-10-04                                           (D)    oneapi/release/2023.2.1
   cudatoolkit-standalone/11.2.2                                     oneapi/release/2024.0
   cudatoolkit-standalone/11.4.4                                     oneapi/upstream
   cudatoolkit-standalone/11.6.2                                     paraview/paraview-5.11.1-mesa
   cudatoolkit-standalone/11.7.1                                     paraview/paraview-5.11.2-EGL-test
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
```

#### Modules Newly Installed

The following modules have been newly installed:

```output
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

Note that `spack-pe-base` and `spack-pe-gnu` are metamodules which contain further software offerings. See the [Spack](#spack) section below for details.

### Spack

We have newly installed Spack deployments in `/soft`. Spack is an HPC-oriented package manager which ALCF uses to install software for the user environment. However, no knowledge of Spack is necessary to use these software offerings. All ALCF-managed software is accessible to users via modules.

The base suite of software tools and libraries can be accessed by loading the `spack-pe-base` module. This adds a path to `$MODULEPATH` which contains numerous modules.

For example, to load `cmake` starting from the default environment, a user should run the following commands:
```bash linenums="1"
module use /soft/modulefiles
module load spack-pe-base
module load cmake
```
Other modules in `spack-pe-base` can be browsed by running `module avail` or `module --show-hidden avail`. The latter shows hidden modules which are installed as dependencies of the un-hidden modules.

In addition to the base stack, a suite of higher-level libraries are installed in the `spack-pe-gnu` module. These are built with and are dependent on `PrgEnv-gnu`. A `PrgEnv-nvidia`-compatible stack will be available in the future.

Note that not all software is installed through Spack; many applications and libraries are installed as standalone packages in `/soft`. Users are encouraged to browse the available modules with `module avail` to see what software is installed on the system.

### ParaView and VisIt
The ParaView module has been updated. For more information, see [ParaView Documentation](./visualization/paraview.md) and [ParaView Manual Launch](./visualization/paraview-manual-launch.md).

The VisIt module is in the process of being updated.

### Changes to Memory Limits on Login Nodes

Memory limits were lowered on the login nodes due to resource contention to 8GB of memory and 8 cores per user. This might result in error messages indicating abnormal process termination for user processes run on logins.

Examples of the error messages people might see are:

- `nvcc error   : 'cudafe++' died due to signal 9 (Kill signal)`
- `g++-12: fatal error: Killed signal terminated program cc1plus`

These errors are likely due to exhausting the per-user resources on a login node as each user is allocated 8 cores and 8GB memory. To avoid this, you can either:

- Reduce the parallelism of your compile, such as using `-j` or `-j4` flags
- Request a debug node and run your compile there where you will have the full resources of the node at your disposal

---
