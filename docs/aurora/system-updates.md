# Aurora System Updates

## 2025-04-29

We ran the test set on the new SDK and overall the results had more fixes than regressions.

### Regressions and Fixes

-37 fixes<br>
-9 regressions

One important note is that kokkos kernels fails to compile in this SDK. If this impacts you, please let us know.

The list of all regressions and fixes is below.

If you see any new issues with this compute image, let us know.

### List of Regressions
source/reproducers/openmp/simd_loops [CMPLRLLVM-38420] P0<r>
source/reproducers/ifx/CMPLRLLVM-35621<br>
source/reproducers/ifx/XDEPS-5191:XDEPS-5191_simd<br>
source/reproducers/ifx/phasta_target_simd [CMPLRLLVM-35621,CMPLRLLVM-40195,CMPLRLLVM-52024,GSD-6634] P0<br>
source/reproducers/icx/assert_problem [CMPLRLLVM-62420]<br>
source/reproducers/dpcpp/catch2_segfault:catch2_segfault_all_at_once_ath_run [CMPLRLLVM-40048,GSD-10857] P0<br>
source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_amazon0302_ath_run [MKLD-12835,MKLD-14715,GSD-10930]<br>
source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_europe_osm_ath_run [MKLD-12835,MKLD-14715,GSD-10930]<br>
source/reproducers/mkl/sparse_openmp_kokkos_kernels:sparse_openmp_kokkos_kernels_wb-edu_ath_run [MKLD-12835,MKLD-14715,GSD-10930]

### List of Fixes:
source/reproducers/dpcpp/FMM [CMPLRLLVM-28325] P0<br>
source/reproducers/dpcpp/VirtualFunction:VirtualFunction_newminimal_ath_compile [CMPLRLLVM-35295,XDEPS-6157,CMPLRLLVM-48349,CMPLRLLVM-50632] P0<br>
source/reproducers/dpcpp/VirtualFunction:VirtualFunction_newminimal_ath_run [CMPLRLLVM-35295,XDEPS-6157,CMPLRLLVM-48349,CMPLRLLVM-50632] P0<br>
source/reproducers/dpcpp/device_copyable_dpl [CMPLRLLVM-57788,CMPLRLLVM-58384]<br>
source/reproducers/dpcpp/hang_inline_workgroup:hang_inline_workgroup_ath_run [CMPLRLLVM-47914,CMPLRLLVM-54117]<br>
source/reproducers/dpcpp/library_jit_main:library_jit_main_aot_shared_jit_main [CMPLRLLVM-41597]<br>
source/reproducers/dpcpp/library_jit_main:library_jit_main_aot_static_jit_main [CMPLRLLVM-41597]<br>
source/reproducers/dpcpp/madgraph4gpu-SYCL-gg_ttggg-nocompile:madgraph4gpu-SYCL-gg_ttggg-nocompile_gg-ttggg_ath_compile [CMPLRLLVM-35981,XDEPS-3923] P0<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_bad_opcode [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_bad_operand_syntax [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_duplicate_label [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_illegal_exec_size [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_missing_label [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_missing_region [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_simple [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_undefined_decl [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_undefined_pred [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/ms371-InlineAsm:ms371-InlineAsm_asm_wrong_declare [cmplrllvm-46097,GSD-7606,GSD-7621,CMPLRLLVM-57331,URLZA-308]<br>
source/reproducers/dpcpp/sincos<br>
source/reproducers/dpcpp_ct/binary [OTFIP-248]<br>
source/reproducers/dpcpp_ct/segfault_build<br>
source/reproducers/dpcpp_ct/vector_trans [OTFIP-449]<br>
source/reproducers/hybrid/oneConcurency:oneConcurency_omp_nowait_ath_run [CMPLRLLVM-34779,CMPLRLLVM-38250,CMPLRLLVM-40729,XDEPS-2202,XDEPS-3493,XDEPS-5689,CMPLRLIBS-35258] P0<br>
source/reproducers/icx/global_bool_isoc_binding:global_bool_isoc_binding_read [CMPLRLLVM-57643,CMPLRLLVM-57935]<br>
source/reproducers/ifx/bgw_compare_wfns P0<br>
source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_four [GSD-8346,CMPLRLLVM-63003]<br>
source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_one [GSD-8346,CMPLRLLVM-63003]<br>
source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_three [GSD-8346,CMPLRLLVM-63003]<br>
source/reproducers/ifx/fopenmp-target-simd-data:fopenmp-target-simd-data_two [GSD-8346,CMPLRLLVM-63003]<br>
source/reproducers/mkl/fft2d_scale [MKLD-13250]<br>
source/reproducers/mkl/slow_batch_getrs [MKLD-15079]<br>
source/reproducers/mkl/zgetrs_batch_slowdown [MKLD-15212,MKLD-15906,MKLD-16680]<br>
source/reproducers/openmp/ddpp_gamess_mini_wrong_answer_fp_precise [CMPLRLLVM-45082,GSD-7772]<br>
source/reproducers/openmp/oneapi_device_selector_test:oneapi_device_selector_test_ath_run [CMPLRLLVM-60986]<br>
source/reproducers/openmp/performance_increading_kernels<br>
source/reproducers/openmp/workshare_performance [CMPLRLLVM-43487]<br>
source/reproducers/tools/advisor_length [ADV-10315]
