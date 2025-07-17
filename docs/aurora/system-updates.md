# Aurora System Updates

## 2025-06-13 (Lowering the memory limit on Aurora compute nodes on June 23, 2025)

ALCF is going to reduce user-accessible memory on Aurora compute nodes, by the equivalent of a node's HBM capacity (128GB), on June 23, 2025, to approximately 960GB between DDR5 + HBM, regardless of how applications utilize each memory tier. The 768GB of GPU memory is unaffected and is not being restricted in any way.

Note that, currently on Aurora, ALCF enforces a memory limit on compute nodes such that users are allowed between MemTotal - 64GB (soft limit) and MemTotal - 32GB (hard limit), effectively reserving between 32GB and 64GB for system use. MemTotal refers to approximately 1TB of DDR5 plus 128GB of HBM.

However, with our current approach due to limitations in cgroup-based enforcement, we are unable to constrain memory usage at the per-NUMA-node level. In other words we can't limit the memory used on DDR5 separately from HBM NUMA nodes. The kernel only gives the capability to limit via overall bytes allocated.

This results in often out-of-memory (OOM) conditions in DDR5 NUMA nodes 0 and 1, Reducing our ability to protect system services and prevent node-wide panics triggered by OOMs.

## 2025-04-28

Due to significant changes resulting from the PM,  recompiling codes is strongly recommended.

### Release notes

- Intel Agama KMD 1099.12
- Intel Agama UMD 1099.12
    - Release notes: [https://dgpu-docs.intel.com/releases/packages.html?release=Rolling+2507.12&os=SLES+15SP4](https://dgpu-docs.intel.com/releases/packages.html?release=Rolling+2507.12&os=SLES+15SP4)
- Intel Vtune Sepdk KMD from 2025.0.5
- Slingshot Host Software 11.1
- libfabric 1.22.0 compiled from HPE SHS-12.0
- mcelog v204
- Added named (bind) as local caching DNS resolver
- Migrate to HPCM 1.10 node packages / filebeat dropping journalbeat
- Increase somaxconn and tcp_max_syn_backlog to 10624_12_2 to help address Aurora PyTorch init and socket limitations
- Update libstdc++6-13.2.1+git7813-150000.1.6.1.x86_64 to support Intel UMD 1099.12 release.
- Add libnuma-devel to UAN and Compute image
- lmod 8.7.59
- Pinned telegraf to cores 0,52,104,156
- Pinned filebeat to cores 0,52,104,156
- Add udev rule to set the register/bit for the PVC IFWI to unlock VSP credit

**FW**  

- BMC - "2916" - bdk-0.0.2916-71bfb1c-8bfb67d-51d61d7-eng
- BIOS (SPR IFWI) - "113.D55" - EGSDCRB1.NWM.0113.D55.2501241829
- PVC IFWI - "25WW083" - 24WW083PSIFWI_14MHzQuadDAMen_CSC201051901_FSP10000733_HBMIO21c0_HSPHY10462011_OOBMSM23WW26A_PCODE44C_ITDa2p75ITDb1p5_IFRv1332PSCv0811

**PE 24.347.0**  

- Intel Compiler update in AuroraSDK to 2025.0.5
    - Compiler/MKL 2025.0.1
        - basekit-2025.0.1
        - hpckit-2025.0.1
        - mkl-core-2025.0-2025.0.1
    - Frameworks 2025.0.5
        - torch==2.5.1+cxx11.abi
        - intel_extension_for_pytorch==2.5.10+xpu
        - oneccl_bind_pt==2.5.0+xpu
        - torchvision==0.20.1+cxx11.abi
        - intel-extension-for-tensorflow[xpu]==2.15.0.2
        - intel-extension-for-openxla==0.5.0 
- Support libraries
    - Updated OpenCL headers, loader 2022.05.18 to 2023.12.14
        - To match [https://github.com/intel/compute-runtime/tree/master/third_party/opencl_headers](https://github.com/intel/compute-runtime/tree/master/third_party/opencl_headers)
    - gpu wrapper scripts tile and dev compact revamp with fixes
    - Dropped gpu_check script
    - Added gemm node test
    - Added valgrind suppression config
- Fixed mpich-config lua module to load correctly
- Updated forge 24.1.1 to 24.1.2
    - Added symlink to latest forge path.
    - Users can now access a default forge install by, /opt/aurora/default/support/tools/forge/latest
- PTI GPU tools update d3639de to 0.11.0
- Updated to Spack PE v0.9.2
    - apptainer: build git commit 39e5a8f
    - buildah: 1.38.1
    - double-batched-fft-library: fix build flags and deps for +sycl
    - hwloc: v2.11-mpich, add patch, force autoreconf
    - hypre: v2.33.0 +sycl +mixedint
    - kokkos: add 4.5.01
    - kokkos-kernels: add 4.5.01
        - Dropped 4.5.01 sycl variant: Build failed on OneAPI 2025.0.5
    - libceed: add sycl variant, track alcf fork
    - mpich: aurora branch (6037a7a); variants: ze, filesystem, daos, misc configuration; add patches from ALCF + Intel; hardcode some configs.
    - petsc: add patches, sycl-arch variant, kokkos dependencies
        - Dropped sycl variant: Build failed on OneAPI 2025.0.5
    - reframe: v4.7.4
    - adios2: fix python env location
    - thapi: 9f2ed86b
    - Umpire: Add MPI variant
 
### Test set results on the new SDK

We ran the test set on the new SDK and overall the results had more fixes than regressions:

Regressions and Fixes:
- 37 Fixes 
- 9 regressions

One important note is that **kokkos kernels fail to compile** in this SDK. If this impacts you, please let us know.

The list of all regressions and fixes is below. If you see any new issues with this compute image, let us know.

**List of regressions:**

- source/reproducers/openmp/simd_loops [CMPLRLLVM-38420] P0
- source/reproducers/ifx/CMPLRLLVM-35621
- source/reproducers/ifx/XDEPS-5191:XDEPS-5191_simd
- source/reproducers/ifx/phasta_target_simd [CMPLRLLVM-35621,CMPLRLLVM-40195,CMPLRLLVM-52024,GSD-6634] P0
- source/reproducers/icx/assert_problem [CMPLRLLVM-62420]
- source/reproducers/dpcpp/catch2_segfault:catch2_segfault_all_at_once_ath_run [CMPLRLLVM-40048,GSD-10857] P0
- source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_amazon0302_ath_run [MKLD-12835,MKLD-14715,GSD-10930]
- source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_europe_osm_ath_run [MKLD-12835,MKLD-14715,GSD-10930]
- source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_wb-edu_ath_run [MKLD-12835,MKLD-14715,GSD-10930]

**List of Fixes:**

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
