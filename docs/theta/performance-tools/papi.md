---
search:
  exclude: true
---

# PAPI on Theta
## Introduction
The Performance Application Programming Interface (PAPI) provides a standardized user level API for accessing processor and other system programmer counter information (such as instruction counts, cache misses, etc.).

PAPI details are provided on the [PAPI Website](http://icl.cs.utk.edu/papi/).

## References
- [PAPI Website](http://icl.cs.utk.edu/papi/)
- [PAPI Documentation](https://bitbucket.org/icl/papi/wiki/Home)

## PAPI on Theta
PAPI is available on Theta through the “papi” or “perftools-base” modules.

The preset and native events available on Theta may be listed by running the 'papi_avail' and 'papi_native_avail' utilities on a compute node via the qsub command. Accessing native events requires using the event names reported by 'papi_native_avail' as a string and converting them into an event code with the function PAPI_event_name_to_code().

### PAPI via papi module on Theta
```
jkwack@thetalogin5:~> qsub -I -n 1 -t 60 -q debug-cache-quad -A Performance

Connecting to thetamom3 for interactive qsub...

Job routed to queue "debug-cache-quad".

Memory mode set to cache quad for queue debug-cache-quad

Wait for job 414557 to start...

Opening interactive session to 3834

jkwack@thetamom3:~> module load papi

jkwack@thetamom3:~> module show papi

-------------------------------------------------------------------

/opt/cray/pe/modulefiles/papi/5.7.0.2:


conflict papi

conflict perftools

conflict perftools-lite

conflict perftools-lite-events

conflict perftools-lite-gpu

conflict perftools-lite-hbm

conflict perftools-lite-loops

conflict perftools-nwpc

conflict perftools-preload

conflict perftools-base

prepend-path PE_PKGCONFIG_LIBS papi:pfm

prepend-path PKG_CONFIG_PATH /opt/cray/pe/papi/5.7.0.2/lib64/pkgconfig

prepend-path PE_PKGCONFIG_PRODUCTS PE_PAPI

setenv PE_PAPI_PKGCONFIG_VARIABLES PE_PAPI_ACCEL_LIBS_@accelerator@ PE_PAPI_ACCEL_FAMILY_LIBS_@accelerator_family@

setenv PE_PAPI_ACCEL_LIBS_nvidia35 ,-lcupti,-lcudart,-lcuda

setenv PE_PAPI_ACCEL_LIBS

setenv PE_PAPI_ACCEL_FAMILY_LIBS_nvidia ,-lcupti,-lcudart,-lcuda

setenv PE_PAPI_ACCELL_FAMILY_LIBS

prepend-path CRAY_LD_LIBRARY_PATH /opt/cray/pe/papi/5.7.0.2/lib64

prepend-path PATH /opt/cray/pe/papi/5.7.0.2/bin

prepend-path MANPATH /opt/cray/pe/papi/5.7.0.2/share/man

setenv PAPI_VERSION 5.7.0.2

setenv LIBPFM_DISABLED_PMUS rapl,fam15h_nb

module-whatis PAPI - The Performance API (PAPI) project specifies a standard application programming interface (API) for accessing hardware performance counters available on most Cray systems.

-------------------------------------------------------------------


jkwack@thetamom3:~> papi_avail

Available PAPI preset and user defined events plus hardware information.

--------------------------------------------------------------------------------

PAPI version             : 5.7.0.2

Operating system         : Linux 4.4.103-6.38_4.0.154-cray_ari_s

Vendor string and code   : GenuineIntel (1, 0x1)

Model string and code    : Intel(R) Xeon(R) CPU E5-2695 v4 @ 2.10GHz (79, 0x4f)

CPU revision             : 1.000000

CPUID                    : Family/Model/Stepping 6/79/1, 0x06/0x4f/0x01

CPU Max MHz              : 2101

CPU Min MHz              : 1200

Total cores              : 72

SMT threads per core     : 2

Cores per socket         : 18

Sockets                  : 2

Cores per NUMA region    : 36

NUMA regions             : 2

Running in a VM          : no

Number Hardware Counters : 11

Max Multiplex Counters   : 384

Fast counter read (rdpmc): no

--------------------------------------------------------------------------------


================================================================================

  PAPI Preset Events

================================================================================

    Name        Code    Avail Deriv Description (Note)

PAPI_L1_DCM  0x80000000  Yes   No   Level 1 data cache misses

PAPI_L1_ICM  0x80000001  Yes   No   Level 1 instruction cache misses

PAPI_L2_DCM  0x80000002  Yes   Yes  Level 2 data cache misses

PAPI_L2_ICM  0x80000003  Yes   No   Level 2 instruction cache misses

PAPI_L3_DCM  0x80000004  No    No   Level 3 data cache misses

PAPI_L3_ICM  0x80000005  No    No   Level 3 instruction cache misses

PAPI_L1_TCM  0x80000006  Yes   Yes  Level 1 cache misses

PAPI_L2_TCM  0x80000007  Yes   No   Level 2 cache misses

PAPI_L3_TCM  0x80000008  Yes   No   Level 3 cache misses

PAPI_CA_SNP  0x80000009  Yes   No   Requests for a snoop

PAPI_CA_SHR  0x8000000a  Yes   No   Requests for exclusive access to shared cache line

PAPI_CA_CLN  0x8000000b  Yes   No   Requests for exclusive access to clean cache line

PAPI_CA_INV  0x8000000c  Yes   No   Requests for cache line invalidation

PAPI_CA_ITV  0x8000000d  Yes   No   Requests for cache line intervention

PAPI_L3_LDM  0x8000000e  Yes   No   Level 3 load misses

PAPI_L3_STM  0x8000000f  No    No   Level 3 store misses

PAPI_BRU_IDL 0x80000010  No    No   Cycles branch units are idle

PAPI_FXU_IDL 0x80000011  No    No   Cycles integer units are idle

PAPI_FPU_IDL 0x80000012  No    No   Cycles floating point units are idle

PAPI_LSU_IDL 0x80000013  No    No   Cycles load/store units are idle

PAPI_TLB_DM  0x80000014  Yes   Yes  Data translation lookaside buffer misses

PAPI_TLB_IM  0x80000015  Yes   No   Instruction translation lookaside buffer misses

PAPI_TLB_TL  0x80000016  No    No   Total translation lookaside buffer misses

PAPI_L1_LDM  0x80000017  Yes   No   Level 1 load misses

PAPI_L1_STM  0x80000018  Yes   No   Level 1 store misses

PAPI_L2_LDM  0x80000019  Yes   No   Level 2 load misses

PAPI_L2_STM  0x8000001a  Yes   No   Level 2 store misses

PAPI_BTAC_M  0x8000001b  No    No   Branch target address cache misses

PAPI_PRF_DM  0x8000001c  Yes   No   Data prefetch cache misses

PAPI_L3_DCH  0x8000001d  No    No   Level 3 data cache hits

PAPI_TLB_SD  0x8000001e  No    No   Translation lookaside buffer shootdowns

PAPI_CSR_FAL 0x8000001f  No    No   Failed store conditional instructions

PAPI_CSR_SUC 0x80000020  No    No   Successful store conditional instructions

PAPI_CSR_TOT 0x80000021  No    No   Total store conditional instructions

PAPI_MEM_SCY 0x80000022  No    No   Cycles Stalled Waiting for memory accesses

PAPI_MEM_RCY 0x80000023  No    No   Cycles Stalled Waiting for memory Reads

PAPI_MEM_WCY 0x80000024  Yes   No   Cycles Stalled Waiting for memory writes

PAPI_STL_ICY 0x80000025  Yes   No   Cycles with no instruction issue

PAPI_FUL_ICY 0x80000026  Yes   Yes  Cycles with maximum instruction issue

PAPI_STL_CCY 0x80000027  Yes   No   Cycles with no instructions completed

PAPI_FUL_CCY 0x80000028  Yes   No   Cycles with maximum instructions completed

PAPI_HW_INT  0x80000029  No    No   Hardware interrupts

PAPI_BR_UCN  0x8000002a  Yes   Yes  Unconditional branch instructions

PAPI_BR_CN   0x8000002b  Yes   No   Conditional branch instructions

PAPI_BR_TKN  0x8000002c  Yes   Yes  Conditional branch instructions taken

PAPI_BR_NTK  0x8000002d  Yes   No   Conditional branch instructions not taken

PAPI_BR_MSP  0x8000002e  Yes   No   Conditional branch instructions mispredicted

PAPI_BR_PRC  0x8000002f  Yes   Yes  Conditional branch instructions correctly predicted

PAPI_FMA_INS 0x80000030  No    No   FMA instructions completed

PAPI_TOT_IIS 0x80000031  No    No   Instructions issued

PAPI_TOT_INS 0x80000032  Yes   No   Instructions completed

PAPI_INT_INS 0x80000033  No    No   Integer instructions

PAPI_FP_INS  0x80000034  No    No   Floating point instructions

PAPI_LD_INS  0x80000035  Yes   No   Load instructions

PAPI_SR_INS  0x80000036  Yes   No   Store instructions

PAPI_BR_INS  0x80000037  Yes   No   Branch instructions

PAPI_VEC_INS 0x80000038  No    No   Vector/SIMD instructions (could include integer)

PAPI_RES_STL 0x80000039  Yes   No   Cycles stalled on any resource

PAPI_FP_STAL 0x8000003a  No    No   Cycles the FP unit(s) are stalled

PAPI_TOT_CYC 0x8000003b  Yes   No   Total cycles

PAPI_LST_INS 0x8000003c  Yes   Yes  Load/store instructions completed

PAPI_SYC_INS 0x8000003d  No    No   Synchronization instructions completed

PAPI_L1_DCH  0x8000003e  No    No   Level 1 data cache hits

PAPI_L2_DCH  0x8000003f  No    No   Level 2 data cache hits

PAPI_L1_DCA  0x80000040  No    No   Level 1 data cache accesses

PAPI_L2_DCA  0x80000041  Yes   No   Level 2 data cache accesses

PAPI_L3_DCA  0x80000042  Yes   Yes  Level 3 data cache accesses

PAPI_L1_DCR  0x80000043  No    No   Level 1 data cache reads

PAPI_L2_DCR  0x80000044  Yes   No   Level 2 data cache reads

PAPI_L3_DCR  0x80000045  Yes   No   Level 3 data cache reads

PAPI_L1_DCW  0x80000046  No    No   Level 1 data cache writes

PAPI_L2_DCW  0x80000047  Yes   No   Level 2 data cache writes

PAPI_L3_DCW  0x80000048  Yes   No   Level 3 data cache writes

PAPI_L1_ICH  0x80000049  No    No   Level 1 instruction cache hits

PAPI_L2_ICH  0x8000004a  Yes   No   Level 2 instruction cache hits

PAPI_L3_ICH  0x8000004b  No    No   Level 3 instruction cache hits

PAPI_L1_ICA  0x8000004c  No    No   Level 1 instruction cache accesses

PAPI_L2_ICA  0x8000004d  Yes   No   Level 2 instruction cache accesses

PAPI_L3_ICA  0x8000004e  Yes   No   Level 3 instruction cache accesses

PAPI_L1_ICR  0x8000004f  No    No   Level 1 instruction cache reads

PAPI_L2_ICR  0x80000050  Yes   No   Level 2 instruction cache reads

PAPI_L3_ICR  0x80000051  Yes   No   Level 3 instruction cache reads

PAPI_L1_ICW  0x80000052  No    No   Level 1 instruction cache writes

PAPI_L2_ICW  0x80000053  No    No   Level 2 instruction cache writes

PAPI_L3_ICW  0x80000054  No    No   Level 3 instruction cache writes

PAPI_L1_TCH  0x80000055  No    No   Level 1 total cache hits

PAPI_L2_TCH  0x80000056  No    No   Level 2 total cache hits

PAPI_L3_TCH  0x80000057  No    No   Level 3 total cache hits

PAPI_L1_TCA  0x80000058  No    No   Level 1 total cache accesses

PAPI_L2_TCA  0x80000059  Yes   Yes  Level 2 total cache accesses

PAPI_L3_TCA  0x8000005a  Yes   No   Level 3 total cache accesses

PAPI_L1_TCR  0x8000005b  No    No   Level 1 total cache reads

PAPI_L2_TCR  0x8000005c  Yes   Yes  Level 2 total cache reads

PAPI_L3_TCR  0x8000005d  Yes   Yes  Level 3 total cache reads

PAPI_L1_TCW  0x8000005e  No    No   Level 1 total cache writes

PAPI_L2_TCW  0x8000005f  Yes   No   Level 2 total cache writes

PAPI_L3_TCW  0x80000060  Yes   No   Level 3 total cache writes

PAPI_FML_INS 0x80000061  No    No   Floating point multiply instructions

PAPI_FAD_INS 0x80000062  No    No   Floating point add instructions

PAPI_FDV_INS 0x80000063  No    No   Floating point divide instructions

PAPI_FSQ_INS 0x80000064  No    No   Floating point square root instructions

PAPI_FNV_INS 0x80000065  No    No   Floating point inverse instructions

PAPI_FP_OPS  0x80000066  No    No   Floating point operations

PAPI_SP_OPS  0x80000067  Yes   Yes  Floating point operations; optimized to count scaled single precision vector operations

PAPI_DP_OPS  0x80000068  Yes   Yes  Floating point operations; optimized to count scaled double precision vector operations

PAPI_VEC_SP  0x80000069  Yes   Yes  Single precision vector/SIMD instructions

PAPI_VEC_DP  0x8000006a  Yes   Yes  Double precision vector/SIMD instructions

PAPI_REF_CYC 0x8000006b  Yes   No   Reference clock cycles

--------------------------------------------------------------------------------

Of 108 possible events, 60 are available, of which 16 are derived.



jkwack@thetamom3:~> which papi_avail

/opt/cray/pe/papi/5.7.0.2/bin/papi_avail
```
### PAPI via perftools-base module on Theta
```
jkwack@thetamom3:~> module unload papi

jkwack@thetamom3:~> module load perftools-base

jkwack@thetamom3:~> module show perftools-base

-------------------------------------------------------------------

/opt/cray/pe/modulefiles/perftools-base/7.0.5:


setenv PERFTOOLS_VERSION 7.0.5

module-whatis The Performance Tools module sets up environments for CrayPat, Apprentice2 and Reveal

conflict perftools-base

conflict perftools

conflict perftools-nwpc

conflict perftools-lite

conflict perftools-lite-loops

conflict perftools-lite-events

conflict perftools-lite-gpu

conflict perftools-lite-hbm

conflict perftools-preload

conflict xt-mpich2

conflict papi

conflict craypat

conflict cuda

conflict cudatools

module use /opt/cray/pe/perftools/7.0.5/modulefiles

prepend-path MODULEPATH /opt/cray/pe/perftools/7.0.5/modulefiles

setenv CRAYPAT_ALPS_COMPONENT /opt/cray/pe/perftools/7.0.5/sbin/pat_alps

setenv PAT_BUILD_PAPI_BASEDIR /opt/cray/pe/papi/5.6.0.5

prepend-path MANPATH /opt/cray/pe/papi/5.6.0.5/share/pdoc/man

prepend-path PATH /opt/cray/pe/papi/5.6.0.5/bin

prepend-path LD_LIBRARY_PATH /opt/cray/pe/papi/5.6.0.5/lib64

setenv CHPL_CG_CPP_LINES 1

setenv PAT_REPORT_PRUNE_NAME _cray$mt_execute_,_cray$mt_start_,__cray_hwpc_,f_cray_hwpc_,cstart,__pat_,pat_region_,PAT_,OMP.slave_loop,slave_entry,_new_slave_entry,_thread_pool_slave_entry,THREAD_POOL_join,__libc_start_main,_start,__start,start_thread,__wrap_,UPC_ADIO_,_upc_,upc_,__caf_,__pgas_,syscall,__device_stub

setenv OFFLOAD_INIT on_start

setenv CRAYPAT_LD_LIBRARY_PATH /opt/cray/pe/gcc-libs:/opt/cray/gcc-libs:/opt/cray/pe/perftools/7.0.5/lib64

prepend-path PATH /opt/cray/pe/perftools/7.0.5/bin

prepend-path MANPATH /opt/cray/pe/perftools/7.0.5/man

setenv CRAYPAT_OPTS_EXECUTABLE sbin/pat-opts

setenv CRAYPAT_ROOT /opt/cray/pe/perftools/7.0.5

setenv APP2_STATE 7.0.5

prepend-path CRAY_LD_LIBRARY_PATH /opt/cray/pe/perftools/7.0.5/lib64

append-path PE_PRODUCT_LIST PERFTOOLS

append-path PE_PRODUCT_LIST CRAYPAT

-------------------------------------------------------------------


jkwack@thetamom3:~> papi_avail

Available PAPI preset and user defined events plus hardware information.

--------------------------------------------------------------------------------

PAPI version             : 5.6.0.5

Operating system         : Linux 4.4.103-6.38_4.0.154-cray_ari_s

Vendor string and code   : GenuineIntel (1, 0x1)

Model string and code    : Intel(R) Xeon(R) CPU E5-2695 v4 @ 2.10GHz (79, 0x4f)

CPU revision             : 1.000000

CPUID                    : Family/Model/Stepping 6/79/1, 0x06/0x4f/0x01

CPU Max MHz              : 2101

CPU Min MHz              : 1200

Total cores              : 72

SMT threads per core     : 2

Cores per socket         : 18

Sockets                  : 2

Cores per NUMA region    : 36

NUMA regions             : 2

Running in a VM          : no

Number Hardware Counters : 11

Max Multiplex Counters   : 384

Fast counter read (rdpmc): no

--------------------------------------------------------------------------------


================================================================================

  PAPI Preset Events

================================================================================

    Name        Code    Avail Deriv Description (Note)

PAPI_L1_DCM  0x80000000  Yes   No   Level 1 data cache misses

PAPI_L1_ICM  0x80000001  Yes   No   Level 1 instruction cache misses

PAPI_L2_DCM  0x80000002  Yes   Yes  Level 2 data cache misses

PAPI_L2_ICM  0x80000003  Yes   No   Level 2 instruction cache misses

PAPI_L3_DCM  0x80000004  No    No   Level 3 data cache misses

PAPI_L3_ICM  0x80000005  No    No   Level 3 instruction cache misses

PAPI_L1_TCM  0x80000006  Yes   Yes  Level 1 cache misses

PAPI_L2_TCM  0x80000007  Yes   No   Level 2 cache misses

PAPI_L3_TCM  0x80000008  Yes   No   Level 3 cache misses

PAPI_CA_SNP  0x80000009  Yes   No   Requests for a snoop

PAPI_CA_SHR  0x8000000a  Yes   No   Requests for exclusive access to shared cache line

PAPI_CA_CLN  0x8000000b  Yes   No   Requests for exclusive access to clean cache line

PAPI_CA_INV  0x8000000c  Yes   No   Requests for cache line invalidation

PAPI_CA_ITV  0x8000000d  Yes   No   Requests for cache line intervention

PAPI_L3_LDM  0x8000000e  Yes   No   Level 3 load misses

PAPI_L3_STM  0x8000000f  No    No   Level 3 store misses

PAPI_BRU_IDL 0x80000010  No    No   Cycles branch units are idle

PAPI_FXU_IDL 0x80000011  No    No   Cycles integer units are idle

PAPI_FPU_IDL 0x80000012  No    No   Cycles floating point units are idle

PAPI_LSU_IDL 0x80000013  No    No   Cycles load/store units are idle

PAPI_TLB_DM  0x80000014  Yes   Yes  Data translation lookaside buffer misses

PAPI_TLB_IM  0x80000015  Yes   No   Instruction translation lookaside buffer misses

PAPI_TLB_TL  0x80000016  No    No   Total translation lookaside buffer misses

PAPI_L1_LDM  0x80000017  Yes   No   Level 1 load misses

PAPI_L1_STM  0x80000018  Yes   No   Level 1 store misses

PAPI_L2_LDM  0x80000019  Yes   No   Level 2 load misses

PAPI_L2_STM  0x8000001a  Yes   No   Level 2 store misses

PAPI_BTAC_M  0x8000001b  No    No   Branch target address cache misses

PAPI_PRF_DM  0x8000001c  Yes   No   Data prefetch cache misses

PAPI_L3_DCH  0x8000001d  No    No   Level 3 data cache hits

PAPI_TLB_SD  0x8000001e  No    No   Translation lookaside buffer shootdowns

PAPI_CSR_FAL 0x8000001f  No    No   Failed store conditional instructions

PAPI_CSR_SUC 0x80000020  No    No   Successful store conditional instructions

PAPI_CSR_TOT 0x80000021  No    No   Total store conditional instructions

PAPI_MEM_SCY 0x80000022  No    No   Cycles Stalled Waiting for memory accesses

PAPI_MEM_RCY 0x80000023  No    No   Cycles Stalled Waiting for memory Reads

PAPI_MEM_WCY 0x80000024  Yes   No   Cycles Stalled Waiting for memory writes

PAPI_STL_ICY 0x80000025  Yes   No   Cycles with no instruction issue

PAPI_FUL_ICY 0x80000026  Yes   Yes  Cycles with maximum instruction issue

PAPI_STL_CCY 0x80000027  Yes   No   Cycles with no instructions completed

PAPI_FUL_CCY 0x80000028  Yes   No   Cycles with maximum instructions completed

PAPI_HW_INT  0x80000029  No    No   Hardware interrupts

PAPI_BR_UCN  0x8000002a  Yes   Yes  Unconditional branch instructions

PAPI_BR_CN   0x8000002b  Yes   No   Conditional branch instructions

PAPI_BR_TKN  0x8000002c  Yes   Yes  Conditional branch instructions taken

PAPI_BR_NTK  0x8000002d  Yes   No   Conditional branch instructions not taken

PAPI_BR_MSP  0x8000002e  Yes   No   Conditional branch instructions mispredicted

PAPI_BR_PRC  0x8000002f  Yes   Yes  Conditional branch instructions correctly predicted

PAPI_FMA_INS 0x80000030  No    No   FMA instructions completed

PAPI_TOT_IIS 0x80000031  No    No   Instructions issued

PAPI_TOT_INS 0x80000032  Yes   No   Instructions completed

PAPI_INT_INS 0x80000033  No    No   Integer instructions

PAPI_FP_INS  0x80000034  No    No   Floating point instructions

PAPI_LD_INS  0x80000035  Yes   No   Load instructions

PAPI_SR_INS  0x80000036  Yes   No   Store instructions

PAPI_BR_INS  0x80000037  Yes   No   Branch instructions

PAPI_VEC_INS 0x80000038  No    No   Vector/SIMD instructions (could include integer)

PAPI_RES_STL 0x80000039  Yes   No   Cycles stalled on any resource

PAPI_FP_STAL 0x8000003a  No    No   Cycles the FP unit(s) are stalled

PAPI_TOT_CYC 0x8000003b  Yes   No   Total cycles

PAPI_LST_INS 0x8000003c  Yes   Yes  Load/store instructions completed

PAPI_SYC_INS 0x8000003d  No    No   Synchronization instructions completed

PAPI_L1_DCH  0x8000003e  No    No   Level 1 data cache hits

PAPI_L2_DCH  0x8000003f  No    No   Level 2 data cache hits

PAPI_L1_DCA  0x80000040  No    No   Level 1 data cache accesses

PAPI_L2_DCA  0x80000041  Yes   No   Level 2 data cache accesses

PAPI_L3_DCA  0x80000042  Yes   Yes  Level 3 data cache accesses

PAPI_L1_DCR  0x80000043  No    No   Level 1 data cache reads

PAPI_L2_DCR  0x80000044  Yes   No   Level 2 data cache reads

PAPI_L3_DCR  0x80000045  Yes   No   Level 3 data cache reads

PAPI_L1_DCW  0x80000046  No    No   Level 1 data cache writes

PAPI_L2_DCW  0x80000047  Yes   No   Level 2 data cache writes

PAPI_L3_DCW  0x80000048  Yes   No   Level 3 data cache writes

PAPI_L1_ICH  0x80000049  No    No   Level 1 instruction cache hits

PAPI_L2_ICH  0x8000004a  Yes   No   Level 2 instruction cache hits

PAPI_L3_ICH  0x8000004b  No    No   Level 3 instruction cache hits

PAPI_L1_ICA  0x8000004c  No    No   Level 1 instruction cache accesses

PAPI_L2_ICA  0x8000004d  Yes   No   Level 2 instruction cache accesses

PAPI_L3_ICA  0x8000004e  Yes   No   Level 3 instruction cache accesses

PAPI_L1_ICR  0x8000004f  No    No   Level 1 instruction cache reads

PAPI_L2_ICR  0x80000050  Yes   No   Level 2 instruction cache reads

PAPI_L3_ICR  0x80000051  Yes   No   Level 3 instruction cache reads

PAPI_L1_ICW  0x80000052  No    No   Level 1 instruction cache writes

PAPI_L2_ICW  0x80000053  No    No   Level 2 instruction cache writes

PAPI_L3_ICW  0x80000054  No    No   Level 3 instruction cache writes

PAPI_L1_TCH  0x80000055  No    No   Level 1 total cache hits

PAPI_L2_TCH  0x80000056  No    No   Level 2 total cache hits

PAPI_L3_TCH  0x80000057  No    No   Level 3 total cache hits

PAPI_L1_TCA  0x80000058  No    No   Level 1 total cache accesses

PAPI_L2_TCA  0x80000059  Yes   Yes  Level 2 total cache accesses

PAPI_L3_TCA  0x8000005a  Yes   No   Level 3 total cache accesses

PAPI_L1_TCR  0x8000005b  No    No   Level 1 total cache reads

PAPI_L2_TCR  0x8000005c  Yes   Yes  Level 2 total cache reads

PAPI_L3_TCR  0x8000005d  Yes   Yes  Level 3 total cache reads

PAPI_L1_TCW  0x8000005e  No    No   Level 1 total cache writes

PAPI_L2_TCW  0x8000005f  Yes   No   Level 2 total cache writes

PAPI_L3_TCW  0x80000060  Yes   No   Level 3 total cache writes

PAPI_FML_INS 0x80000061  No    No   Floating point multiply instructions

PAPI_FAD_INS 0x80000062  No    No   Floating point add instructions

PAPI_FDV_INS 0x80000063  No    No   Floating point divide instructions

PAPI_FSQ_INS 0x80000064  No    No   Floating point square root instructions

PAPI_FNV_INS 0x80000065  No    No   Floating point inverse instructions

PAPI_FP_OPS  0x80000066  No    No   Floating point operations

PAPI_SP_OPS  0x80000067  Yes   Yes  Floating point operations; optimized to count scaled single precision vector operations

PAPI_DP_OPS  0x80000068  Yes   Yes  Floating point operations; optimized to count scaled double precision vector operations

PAPI_VEC_SP  0x80000069  Yes   Yes  Single precision vector/SIMD instructions

PAPI_VEC_DP  0x8000006a  Yes   Yes  Double precision vector/SIMD instructions

PAPI_REF_CYC 0x8000006b  Yes   No   Reference clock cycles

--------------------------------------------------------------------------------

Of 108 possible events, 60 are available, of which 16 are derived.


jkwack@thetamom3:~> which papi_avail

/opt/cray/pe/papi/5.6.0.5/bin/papi_avail
```
