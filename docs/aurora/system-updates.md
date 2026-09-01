# Aurora System Updates

This page is a reverse-chronological log of changes to Aurora's system software, firmware, and programming environment.

Aurora's largest changes are applied together as a **major update**: new Intel GPU drivers (Agama KMD/UMD), a new compute OS image, and a new programming environment (PE) release. A major update reaches users in stages over several weeks. It first appears in the `next-eval` test queue on a subset of nodes and UANs, is revised in response to user testing, and is finally rolled out to the full machine. Because those dated events are all part of one change, they are grouped below under a single `Major update` heading, newest stage first, with the full change log under the stage that introduced it. **Major updates typically require recompiling applications.**

Everything else appears as a standalone dated entry: firmware refreshes, fabric and storage updates, and policy changes.

| Date | Update |
| --- | --- |
| 2026-08-31 → 2026-09-01 | **Major update:** [Agama 1146.78 drivers and oneAPI 2026.1.0](#2026-08-31-available-in-the-next-eval-test-queue) |
| 2026-05-21 | [OS image and firmware update](#2026-05-21-os-image-and-firmware-update) |
| 2026-02-23 → 2026-03-10 | **Major update:** [Agama 1146.40 drivers and oneAPI 2025.3.1](#major-update-agama-114640-drivers-and-oneapi-202531-feb-mar-2026) |
| 2026-02-02 | [Flare upgrade and NEO 7.2-021 software update](#2026-02-02-flare-upgrade-and-neo-72-021-software-update) |
| 2025-10-27 | [Slingshot fabric manager and DAOS updates](#2025-10-27-slingshot-fabric-manager-and-daos-updates) |
| 2025-09-08 → 2025-10-13 | **Major update:** [Agama 1146.12 drivers and oneAPI 2025.2.0](#major-update-agama-114612-drivers-and-oneapi-202520-sep-oct-2025) |
| 2025-06-13 | [Lower memory limit on compute nodes](#2025-06-13-lower-memory-limit-on-compute-nodes-effective-2025-06-23) |
| 2025-04-28 | **Major update:** [Agama 1099.12 drivers and oneAPI 2025.0.5](#major-update-agama-109912-drivers-and-oneapi-202505-2025-04-28) |

## 2026-08-31: Available in the next-eval test queue
We have created a temporary test queue `next-eval` (open to all users) with up to 2,112 nodes that are using a new compute image. UANs `aurora-uan-0007` and `aurora-uan-0008` also have the new software image and can be used for compiling. The queue is available starting 9/1/2026.

**This is a significant update with a larger than normal chance for issues. Testing during this period is greatly appreciated.**

See [Running jobs on Aurora](running-jobs-aurora.md) for queue policies.

The new image includes updates to:
 - SLES 15 SP7 with kernel 6.4.0-150700.53.73-default
 - HPE's Slingshot Host Software 14.0.1
 - Intel's User (UMD) and Kernel Mode Drivers (KMD) (Agama 1146.78 / LTS release 2523.78)
 - libfabric 2.3.1
   - Default log levels increased as below. If log output is too high with lines starting with `libfabric` unset FI_LOG_LEVEL.
     `FI_LOG_LEVEL=warn`
     `FI_LOG_PROV=cxi`
     
 #### PE 26.181.0
- OneAPI Toolkit 2026.1
- Standalone oneDAL 2026.1
- Standalone advisor 2026.0
- Standalone shmem 1.5.0
- coregen: 2026.1.0.0
- Drops DPCT, replaced with syclomatic, available as a module
- Drop Kokkos 4.x
- hypre: 3.1.0 → 3.1.1.abaebcc
- ginkgo: patch for SYCL API deprecations
- mpich: 5.0.0.aurora_test.87e2045
- UMD: AICOE: 2026.06.19 release
- UMD: debuginfo for 1146.78
- Frameworks 2026.1.0
    - torch 2.13.0a0+gitcf30153 
    - torchao 0.17.0+git02105d46c 
    - torchcodec 0.15.0 
    - torchcomms 0.3.1 
    - torchdata 0.11.0+377e64c 
    - torchvision 0.28.0+8fb8771 
    - triton-xpu 3.7.2 
    - mpi4py 4.1.2 
    - vllm 0.26.1.dev0+g568afb3a1.d20260803.xpu 
    - vllm-xpu-kernels 0.1.11.2.dev0+ga692986.d20260803 
    - deepspeed 0.19.3 
    - dpctl 0.23.0.dev0+205.gb24f931fde 
    - dpnp 0.21.0.dev3+8.g987f2992697 
    - scikit-learn 1.9.0 
    - scikit-learn-intelex 20260728.214749
    - Known Issues
        - Workaround for frameworks module load:
            `export LD_LIBRARY_PATH=/opt/aurora/26.181.0/frameworks/aurora_frameworks-2026.1.0/lib:$LD_LIBRARY_PATH`
            `ml add frameworks`
        - For vllm XPUGraph capturing to work, in your job script:
            `unset CCL_OP_SYNC`
            `unset CCL_ATL_SYNC_COLL`
            `export CCL_OP_SYNC=0`
            `export CCL_ATL_SYNC_COLL=0`


#### PE 26.26.0
Largely matches current deployed PE on Aurora, but recompiled for SLES 15 SP7 and Intel UMD 1146.78. Includes fixes/changes:

- Fixes for libxml2 missing pkgconfig
- Update darshan 3.4.7 → 3.5.0 and include all optional extras
- move pti to a spack package
- Kokkos: 4.7.04 → 4.7.02, However, 5.1.1 is default
- mpich: 5.0.0.aurora_test.e358bbd → 5.0.0.aurora_test.87e2045
- py-torch: +gloo
- pti: +0.17.0
- mpi eager threshold: on by default
- petsc: 3.24.5-sycl → 3.25.2-sycl
- adios: 2.11.0 → 2.12.1
- UMD: AICOE: 2026.06.19 release
- UMD: debuginfo for 1146.78

## 2026-05-21: OS image and firmware update

OS image `compute_aurora_prod_20260520T215745_835edd6`:

- Kernel update
- vastnfs 4.0.40
- lustre-client cray-2.15.B24
- xpmem-2.7.17
- kdreg2 from SHS 14.0 (`kdreg2-1.0.0-0__2e1e777.SHS14.0.0`)
- msr-safe and sepdk KMDs rebuilt, but version unchanged

ECB firmware:

- BIOS 0116.D10
- PVC IFWI WW43.3_14

## Major update: Agama 1146.40 drivers and oneAPI 2025.3.1 (Feb-Mar 2026)

!!! warning "Recompile required"

    Due to the updates to Aurora's programming environment, users will need to recompile applications (if they already haven't been recompiled in the `next-eval` environment).

!!! abstract "At a glance"

    - **Intel GPU drivers (KMD/UMD):** Agama 1146.40 / [LTS release 2523.40](https://dgpu-docs.intel.com/overview/release-notes/lts-drivers-and-packages/2523.40.html)
    - **Programming environment:** PE 26.26.0 with oneAPI 2025.3.1
    - **First available:** [2026-02-23](#2026-02-23-available-in-the-next-eval-test-queue), in the `next-eval` test queue
    - **Rolled out to all nodes:** [2026-03-10](#2026-03-10-rolled-out-to-all-aurora-nodes)

### 2026-03-10: Rolled out to all Aurora nodes

The compute image with updates to Intel's User (UMD) and Kernel Mode Drivers (KMD) (Agama 1146.40 / LTS release 2523.40), and oneAPI 2025.3.1, which was previously available in the `next-eval` queue, is rolled out to all of the nodes across Aurora.

The full change log is in the [2026-02-23](#2026-02-23-available-in-the-next-eval-test-queue) entry below.

### 2026-02-23: Available in the `next-eval` test queue

We have a **temporary** test queue `next-eval` (open to all users) with upto 2,600 nodes that has a new compute image. **UANs aurora-uan-0007 and aurora-uan-0008 have the new software image and can be used for compiling.** Please prioritize use of `next-eval` queue for testing and evaluation. See [Running jobs on Aurora](running-jobs-aurora.md) for queue policies. The new image includes updates to Intel's User (UMD) and Kernel Mode Drivers (KMD) (Agama 1146.40 / LTS release 2523.40), and oneAPI 2025.3.1.

Details of the full change log are below (**`next-eval` test queue only**):

#### OS image

- Intel KMD/UMD 1146.40 / LTS 2523.40
- Intel sepdk KMDs from oneAPI 2025.3.0
- Lustre Client cray-2.15.B23
- GEOPM 3.2.2
- DAOS Client 2.6.4-11
- `/daos` is now a symlink to `/tmp` for use with DAOS dfuse mounts
- Legacy AuroraSDK / PE versions dropped:
    - 24.347.0 (oneAPI 2025.0.5)
    - 24.180.3 (oneAPI 2024.2.1)

#### PE 26.26.0

- oneAPI 2025.3.1
    - oneAPI Base Toolkit 2025.3.1
    - oneAPI HPC Toolkit 2025.3.1
    - Intel Deep Learning Essentials 2025.3.2
    - Intel Compiler 2025.3.2
    - See [Known Issues](bugs-table.md)
- Spack (configuration)
    - Spack 1.1 update with backported patches for externals and oneAPI
    - Base Python updated to 3.12.12
- Spack (packages, limited to oneAPI dependencies)
    - amrex - 26.02
    - ginkgo - 1.11
    - blaspp, lapackpp - 2025.05.28
    - hdf5 - 2.0.0, 1.14.6
    - kokkos - 5.0.1, 4.7.02
    - umpire - 2025.12.0 +sycl
    - raja - 2025.12.0
    - petsc - 3.24.3 +sycl
    - hypre - 3.0.0 +sycl
    - geopm - 3.2.2
    - boost - 1.88
    - py-torch - 2.10.0 and deps
    - xpu-smi - 1.2.42, 1.3.5
    - warpx - 26.02
- Forge
    - 25.1.1
- MPICH
    - `aurora_test` branch @ [3c70a61](https://github.com/pmodels/mpich/compare/6037a7a..3c70a61)
    - libfabric optimization variables set by default according to HPE's SHS guide
        - Can be checked by `ml show mpich`
    - New pipeline algorithm disabled by default
- `frameworks/2025.3.1` module
    - Major packages:
        - torch 2.10.0a0+git449b176
        - torchao 0.15.0+git9338966da
        - torchdata 0.11.0+377e64c
        - torchvision 0.25.0+8ac84ee
        - torchcomms 0.1.0
        - intel-extension-for-pytorch 2.10.10+gitd0f992f
        - pytorch-triton-xpu 3.6.0+git225cdbde
        - vllm 0.15.0+xpu
        - scikit_learn_intelex-20260205.124755 (tag: 2025.10.1)
        - dpnp 0.19.1
        - dpctl 0.21.1
    - Major change:
        - `ONEAPI_DEVICE_SELECTOR="opencl:gpu;level_zero:gpu"`
            - Exposing both to ensure functionality of `torch` , `triton-xpu` , `vLLM`, `ray` and `dpctl`
            - We warn the users to this change upon loading the module
            - Request switch to `ONEAPI_DEVICE_SELECTOR="level_zero:gpu"` and report unusual behaviors
            - **Temporary**, with proposed fixes included `triton-xpu` release we will switch back to `ONEAPI_DEVICE_SELECTOR="level_zero:gpu"`

## 2026-02-02: Flare upgrade and NEO 7.2-021 software update

Flare is scheduled to be upgraded Feb 2 - Feb 5, 2026 resulting in Aurora being unavailable during this time.

- NEO 7.2-021 software updates and hardware firmware updates.

## 2025-10-27: Slingshot fabric manager and DAOS updates

- Slingshot Fabric Manager update to 2.3.1
- System `daos_user` has been upgraded to `daos-2.6.4` GA, enabled in PBS

## Major update: Agama 1146.12 drivers and oneAPI 2025.2.0 (Sep-Oct 2025)

!!! warning "Recompile required"

    Due to the updates to Aurora's GPU drivers and programming environment, users will need to recompile applications (if they already haven't been recompiled in the `next-eval` environment).

!!! abstract "At a glance"

    - **Intel GPU drivers (KMD/UMD):** Agama 1146.12 / rolling release 2523.12
    - **Programming environment:** PE 25.190.0 with oneAPI 2025.2.0
    - **First available:** [2025-09-08](#2025-09-08-available-in-the-next-eval-test-queue), in the `next-eval` test queue
    - **Revised:** [2025-10-07](#2025-10-07-next-eval-updated-to-aurorasdk-251900-rc4), AuroraSDK 25.190.0 RC4 in `next-eval`
    - **Rolled out to most nodes:** [2025-10-13](#2025-10-13-rolled-out-to-most-aurora-nodes), with a `legacy` queue retaining the old image

### 2025-10-13: Rolled out to most Aurora nodes

The compute image with Intel's User (UMD) and Kernel Mode Drivers (KMD) (Agama 1146.12 / rolling release 2523.12), and oneAPI 2025.2.0, which was previously available in the `next-eval` queue, is rolled out to the majority of nodes across Aurora.

2,126 nodes have the old production image and are available in a queue called `legacy`, which will be available to all teams that are unable to run against the new image. Some teams will have higher priority to run in the legacy queue. Use `aurora-uan-000[7-8]` nodes for the `legacy` queue as they will have the same user environment. Users will not be able log in directly to `aurora-uan-000[7-8]` and will need to `ssh` to them after logging in to `aurora.alcf.anl.gov`.

See [Running jobs on Aurora](running-jobs-aurora.md).

The full changelog is in the [2025-10-07](#2025-10-07-next-eval-updated-to-aurorasdk-251900-rc4) and [2025-09-08](#2025-09-08-available-in-the-next-eval-test-queue) entries below.

### 2025-10-07: `next-eval` updated to AuroraSDK 25.190.0 RC4

The image in the `next-eval` queue, and `uan-0014`, has been updated to AuroraSDK version 25.190.0 RC4, with the following changes.

#### MPICH

- Roll back default MPICH to `mpich/opt/develop-git.6037a7a` (`5.0.0.aurora_test.06f012a` is still available)
- Use internal yaksa build for versions `develop-git.6037a7a` and `5.0.0.aurora_test.06f012a`
- Use yaksa v0.4 for versions 4.2.3 and 4.3.1

#### oneAPI

- Fix module environment to match oneAPI install's `setvars.sh`

#### Notable fixes

We had 32 fixes in the new SDK based on the bug reproducer test set. Notable fixes include:

- SYCL in-order queue fixed (affecting any application using in-order SYCL queues)
- Bug in SYCL `peer_access` fixed
- Runtime error in PyTorch with `CCL_BCAST` fixed
- Compile fail in Lattice App fixed
- Fails in Fortran if compiled with MKL and `-fpe0` fixed
- Issues with `-fopenmp-target-simd` usage fixed

### 2025-09-08: Available in the `next-eval` test queue

We have a **temporary** test queue `next-eval` (open to all users) with 2,688 nodes that has a new compute image. **UAN-0014 has the new software image and can be used for compiling.** Please prioritize use of `next-eval` queue for testing and evaluation. See [Running jobs on Aurora](running-jobs-aurora.md) for queue policies.

The new image includes updates to Intel's User (UMD) and Kernel Mode Drivers (KMD) (Agama 1146.12 / rolling release 2523.12), and oneAPI 2025.2.0.

Details of the full change log are below (**`next-eval` test queue only**):

#### OS image: compute_aurora_test_20250905T165210_95b26e6

- Intel KMD/UMD 1146.12 / Rolling Release 2523.12
- Intel sepdk KMDs from oneAPI 2025.2.0
- Cray PALS 1.8.0 - (Built from source on SLES 15 SP4 against PMIX 4.2.9)
- Cray PE 25.03, drop old Cray PE 23.03
- GEOPM 3.2.0
- DAOS Agent 2.6.4 RC1
- Lustre cray-2.15.B21
- vastnfs 4.0.34 (replaces inbox NFS client so used on OS boot, PE, `/soft`.)
- Pin `sssd` to CPU cores 0,52,104,156
- Set `kernel.hung_task_check_interval_secs = 120`
- Set `kernel.softlockup-all-cpu-backtrace = 1`
- Add `HWLOC_COMPONENTS="-levelzero"` to `palsd` systemd unit, disabling the level-zero plugin in hwloc for the `palsd` process itself, as `palsd` uses level-zero directly for GPU discovery.

#### ECB firmware

- pciesw[0-1] - 4.16.0.0
- PVC IFWI - `25WW204PSIFWI_14MHzQuadDAMen_CSC201051902_FSP10000735_HBMIO21c0_HSPHY10462011_OOBMSM23WW26A_PCODE18b_ITDa2p75ITDb1p5_IFRv1332PSCv0811`
- BMC - `bdk-0.0.2916b-71bfb1c-8bfb67d-51d61d7-eng`

#### PE 25.190.0

- oneAPI 2025.2.0
- Spack (configuration)
    - Spack configurations are now available in `/opt/aurora/25.190.0/spack/unified/0.10.0/config`
        - Compatible with Spack v0.23.1
- Spack (packages)
    - Package additions: subversion, zip, py-parsl, py-mpi4py, py-h5py
    - ML components added in Spack
        - py-torch: 2.7.1.a0, 2.8.0.a0, 2.9.0.dev20250804
        - py-torchaudio: 2.7.1.a0, 2.8.0.a0, 2.8.0.dev20250807
        - py-torchvision: 0.22.1a0, 0.23.0a0, 0.24.0.dev20250807
        - py-triton-xpu: 3.4.x, git.83367a9
        - py-oneccl-bind-pt: 2.7.0xpu, 2.8.0xpu, master
        - py-deepspeed: 0.17.4, master
        - py-ipex: 2.7.10xpu, 2.8.10xpu, xpu-main
    - GEOPM 3.2.0
    - reframe: include fast polling variant (`reframe-compute`; please only use this if running directly from a compute node)
    - darshan-runtime: 3.4.7, set MPICH profiles. Applications built with darshan-runtime loaded will be built with automatic instrumentation at runtime.
    - MPICH@aurora
        - Uses the [`aurora` branch](https://github.com/pmodels/mpich/tree/aurora) of upstream MPICH
        - Manually set `MPIR_CVAR_CH4_OFI_EAGER_THRESHOLD=1000000` so large message above 1MB will use the new auto rndv mode which includes the pipeline algorithm.
        - Default tuning files
    - petsc: use 64-bit indices
    - numpy: build with GCC to workaround compiler segfault
    - hdf5 +map
    - apptainer: 1.4.1
    - kokkos, kokkos-kernels: 4.7.00
    - hypre@435e042
    - stat@6c83af9
    - minor version updates to several other packages

#### `frameworks` (preview) module

- `miniforge` based `conda` environment with source builds of
    - torch 2.8.0a0+gitba56102
    - torchao 0.12.0+git442232fbf
    - torchdata 0.11.0+377e64c
    - torchtune 0.6.1 -- but the `conda/pip` list version appears as `0.0.0`
    - torchvision 0.23.0a0+824e8c8
    - intel-extension-for-pytorch 2.8.10+git09505bb
    - pytorch-triton-xpu 3.4.0+gitae324eea
    - deepspeed 0.17.5+047a7599
    - deepspeed-kernels 0.0.1.dev1698255861
    - scikit-learn-intelex 20250822.140259
    - numba_dpex 0.23.0+31.g63ac57378
    - dpnp 0.18.1
    - dpctl 0.20.2
    - vllm 0.10.1rc2.dev189+ge2db1164a.xpu
    - mpi4py 4.1.0
    - h5py 3.14.0
- Associated 302 dependency packages coming exclusively from `pip`
- `torchtitan==0.1.0` dependencies included
- Major changes:
    - Dropped JAX for this iteration. Expected to be added back in future updates.
    - Separated TensorFlow and Horovod in favor of a separate ecosystem
    - Removed `oneccl-bindings-for-pytorch` in favor of the `xccl` backend of the PyTorch-DDP. This is a **breaking change**:
        - PyTorch-DDP must be initialized with **`backend='xccl'`** instead of **`backend='ccl'`**
        - `import oneccl_bindings_for_pytorch` must be removed, otherwise `ModuleNotFoundError`
    - Introducing **`numpy==2.0.2`**
        - All of the PyTorch ecosystem has been compiled against `numpy==2.0.2`
        - Workloads that pin `numpy` 1.x should continue to work. Since NumPy 1.25, extensions built against a newer NumPy stay binary compatible with older 1.x releases at runtime, so these modules do not force you off `numpy` 1.x.
        - That guarantee covers the binary interface only. Python code calling names removed in NumPy 2.0, such as `np.float_`, `np.unicode_`, and `np.NaN`, must be updated regardless of which `numpy` is installed. See the [NumPy 2.0 migration guide](https://numpy.org/doc/stable/numpy_2_0_migration_guide.html).
- `frameworks` (preview) module's known issues:
    - `conda list` throws a warning about `setuptools` and freeing file handles.
    - `DeepSpeed` `JIT` compilation failure
    - `oneccl` collectives requiring explicit synchronization step
        - Workaround: `export CCL_OP_SYNC=1` (already set in `frameworks` module)
    - `oneccl` Rabenseifner algorithm for `Allreduce` failure (potential bug). Recommending `direct`
    - `vLLM` failure to start EngineCore on multiple ranks
        - Workaround: `unset CCL_PROCESS_LAUNCHER && export CCL_PROCESS_LAUNCHER=None && unset ONEAPI_DEVICE_SELECTOR`
    - Potential issues with `mlflow` -- a `torchtune` dependency -- used for tracing and hyper-parameter tracking, very similar to `wandb`. `mlflow` will be removed in future updates.

## 2025-06-13: Lower memory limit on compute nodes (effective 2025-06-23)

ALCF is going to reduce user-accessible memory on Aurora compute nodes, by the equivalent of a node's HBM capacity (128GB), on June 23, 2025, to approximately 960GB between DDR5 + HBM, regardless of how applications utilize each memory tier. The 768GB of GPU memory is unaffected and is not being restricted in any way.

Note that, on Aurora, ALCF currently enforces a memory limit on compute nodes such that users may allocate between (MemTotal minus 64GB) as a soft limit and (MemTotal minus 32GB) as a hard limit, effectively reserving between 32GB and 64GB for system services. MemTotal refers to approximately 1TB of DDR5 plus 128GB of HBM.

However, with our current approach due to limitations in cgroup-based enforcement, we are unable to constrain memory usage at the per-NUMA-node level. In other words we can't limit the memory used on DDR5 separately from HBM NUMA nodes. The kernel only gives the capability to limit via overall bytes allocated.

This results in often out-of-memory (OOM) conditions in DDR5 NUMA nodes 0 and 1, Reducing our ability to protect system services and prevent node-wide panics triggered by OOMs.

## Major update: Agama 1099.12 drivers and oneAPI 2025.0.5 (2025-04-28)

!!! warning "Recompile required"

    Due to the significant changes resulting from this PM, users will need to recompile applications.

!!! abstract "At a glance"

    - **Intel GPU drivers (KMD/UMD):** Agama 1099.12 / rolling release 2507.12
    - **Programming environment:** PE 24.347.0 with oneAPI 2025.0.5
    - **Rolled out to all nodes:** 2025-04-28, during the preventive maintenance

### OS image

- Intel Agama KMD 1099.12
- Intel Agama UMD 1099.12
    - Rolling release 2507.12 for SLES 15 SP4
- Intel VTune sepdk KMD from 2025.0.5
- Slingshot Host Software 11.1
- libfabric 1.22.0 compiled from HPE SHS-12.0
- mcelog v204
- Added named (bind) as local caching DNS resolver
- Migrate to HPCM 1.10 node packages / filebeat dropping journalbeat
- Increase `somaxconn` and `tcp_max_syn_backlog` to 10624_12_2 to help address Aurora PyTorch init and socket limitations
- Update `libstdc++6-13.2.1+git7813-150000.1.6.1.x86_64` to support Intel UMD 1099.12 release.
- Add libnuma-devel to UAN and Compute image
- lmod 8.7.59
- Pinned telegraf to cores 0,52,104,156
- Pinned filebeat to cores 0,52,104,156
- Add udev rule to set the register/bit for the PVC IFWI to unlock VSP credit

### ECB firmware

- BMC - "2916" - `bdk-0.0.2916-71bfb1c-8bfb67d-51d61d7-eng`
- BIOS (SPR IFWI) - "113.D55" - `EGSDCRB1.NWM.0113.D55.2501241829`
- PVC IFWI - "25WW083" - `24WW083PSIFWI_14MHzQuadDAMen_CSC201051901_FSP10000733_HBMIO21c0_HSPHY10462011_OOBMSM23WW26A_PCODE44C_ITDa2p75ITDb1p5_IFRv1332PSCv0811`

### PE 24.347.0

- Intel Compiler update in AuroraSDK to 2025.0.5
    - Compiler/MKL 2025.0.1
        - basekit-2025.0.1
        - hpckit-2025.0.1
        - mkl-core-2025.0-2025.0.1
    - `frameworks/2025.0.5` module 
        - torch==2.5.1+cxx11.abi
        - intel_extension_for_pytorch==2.5.10+xpu
        - oneccl_bind_pt==2.5.0+xpu
        - torchvision==0.20.1+cxx11.abi
        - intel-extension-for-tensorflow[xpu]==2.15.0.2
        - intel-extension-for-openxla==0.5.0
- Support libraries
    - Updated OpenCL headers, loader 2022.05.18 to 2023.12.14
        - To match the [OpenCL headers in Intel's compute-runtime](https://github.com/intel/compute-runtime/tree/master/third_party/opencl_headers)
    - gpu wrapper scripts tile and dev compact revamp with fixes
    - Dropped `gpu_check` script
    - Added gemm node test
    - Added valgrind suppression config
- Fixed `mpich-config` lua module to load correctly
- Updated forge 24.1.1 to 24.1.2
    - Added symlink to latest forge path.
    - Users can now access a default forge install by `/opt/aurora/default/support/tools/forge/latest`
- PTI GPU tools update d3639de to 0.11.0
- Updated to Spack PE v0.9.2
    - apptainer: build git commit 39e5a8f
    - buildah: 1.38.1
    - double-batched-fft-library: fix build flags and deps for +sycl
    - hwloc: v2.11-mpich, add patch, force autoreconf
    - hypre: v2.33.0 +sycl +mixedint
    - kokkos: add 4.5.01
    - kokkos-kernels: add 4.5.01
        - Dropped 4.5.01 sycl variant: Build failed on oneAPI 2025.0.5
    - libceed: add sycl variant, track alcf fork
    - mpich: aurora branch (6037a7a); variants: ze, filesystem, daos, misc configuration; add patches from ALCF + Intel; hardcode some configs.
    - petsc: add patches, sycl-arch variant, kokkos dependencies
        - Dropped sycl variant: Build failed on oneAPI 2025.0.5
    - reframe: v4.7.4
    - adios2: fix python env location
    - thapi: 9f2ed86b
    - Umpire: Add MPI variant

### Test set results on the new SDK

We ran the test set on the new SDK and overall the results had more fixes than regressions:

- 37 fixes
- 9 regressions

One important note is that **Kokkos Kernels fails to compile** in this SDK. If this impacts you, please let us know.

The full list of regressions and fixes is below. If you see any new issues with this compute image, let us know.

??? failure "List of regressions"

    - source/reproducers/openmp/simd_loops [CMPLRLLVM-38420] P0
    - source/reproducers/ifx/CMPLRLLVM-35621
    - source/reproducers/ifx/XDEPS-5191:XDEPS-5191_simd
    - source/reproducers/ifx/phasta_target_simd [CMPLRLLVM-35621,CMPLRLLVM-40195,CMPLRLLVM-52024,GSD-6634] P0
    - source/reproducers/icx/assert_problem [CMPLRLLVM-62420]
    - source/reproducers/dpcpp/catch2_segfault:catch2_segfault_all_at_once_ath_run [CMPLRLLVM-40048,GSD-10857] P0
    - source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_amazon0302_ath_run [MKLD-12835,MKLD-14715,GSD-10930]
    - source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_europe_osm_ath_run [MKLD-12835,MKLD-14715,GSD-10930]
    - source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_wb-edu_ath_run [MKLD-12835,MKLD-14715,GSD-10930]

??? success "List of fixes"

    - source/reproducers/dpcpp/FMM [CMPLRLLVM-28325] P0
    - source/reproducers/dpcpp/VirtualFunction:VirtualFunction_newminimal_ath_compile [CMPLRLLVM-35295,XDEPS-6157,CMPLRLLVM-48349,CMPLRLLVM-50632] P0
    - source/reproducers/dpcpp/VirtualFunction:VirtualFunction_newminimal_ath_run [CMPLRLLVM-35295,XDEPS-6157,CMPLRLLVM-48349,CMPLRLLVM-50632] P0
    - source/reproducers/dpcpp/device_copyable_dpl [CMPLRLLVM-57788,CMPLRLLVM-58384]
    - source/reproducers/dpcpp/hang_inline_workgroup:hang_inline_workgroup_ath_run [CMPLRLLVM-47914,CMPLRLLVM-54117]
    - source/reproducers/dpcpp/library_jit_main:library_jit_main_aot_shared_jit_main [CMPLRLLVM-41597]
    - source/reproducers/dpcpp/library_jit_main:library_jit_main_aot_static_jit_main [CMPLRLLVM-41597]
    - source/reproducers/dpcpp/madgraph4gpu-SYCL-gg_ttggg-nocompile:madgraph4gpu-SYCL-gg_ttggg-nocompile_gg-ttggg_ath_compile [CMPLRLLVM-35981,XDEPS-3923] P0
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_bad_opcode [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_bad_operand_syntax [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_duplicate_label [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_illegal_exec_size [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_missing_label [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_missing_region [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_simple [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_undefined_decl [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_undefined_pred [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_wrong_declare [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]
    - source/reproducers/dpcpp/sincos source/reproducers/dpcpp_ct/binary [OTFIP-248]
    - source/reproducers/dpcpp_ct/segfault_build source/reproducers/dpcpp_ct/vector_trans [OTFIP-449]
    - source/reproducers/hybrid/oneConcurency:oneConcurency_omp_nowait_ath_run [CMPLRLLVM-34779,CMPLRLLVM-38250,CMPLRLLVM-40729,XDEPS-2202,XDEPS-3493,XDEPS-5689,CMPLRLIBS-35258] P0
    - source/reproducers/icx/global_bool_isoc_binding:global_bool_isoc_binding_read [CMPLRLLVM-57643,CMPLRLLVM-57935]
    - source/reproducers/ifx/bgw_compare_wfns P0 source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_four [GSD-8346,CMPLRLLVM-63003]
    - source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_one [GSD-8346,CMPLRLLVM-63003]
    - source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_three [GSD-8346,CMPLRLLVM-63003]
    - source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_two [GSD-8346,CMPLRLLVM-63003]
    - source/reproducers/mkl/fft2d_scale [MKLD-13250] source/reproducers/mkl/slow_batch_getrs [MKLD-15079]
    - source/reproducers/mkl/zgetrs_batch_slowdown [MKLD-15212,MKLD-15906,MKLD-16680]
    - source/reproducers/openmp/ddpp_gamess_mini_wrong_answer_fp_precise [CMPLRLLVM-45082,GSD-7772]
    - source/reproducers/openmp/oneapi_device_selector_test:oneapi_device_selector_test_ath_run [CMPLRLLVM-60986]
    - source/reproducers/openmp/performance_increading_kernels source/reproducers/openmp/workshare_performance [CMPLRLLVM-43487]
    - source/reproducers/tools/advisor_length [ADV-10315]
