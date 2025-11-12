# Debugging on Aurora with `gdb-oneapi`

The `gdb-oneapi` tool is part of Intel's oneAPI software and is available via the default modules loaded on Aurora. It provides the ability to debug kernels offloaded to the PVC GPUs, as well as CPU code debugging. It does _not_ provide multiprocess or multinode debugging; it is not integrated with MPI. For parallel debugging, we recommend using [DDT](./ddt-aurora.md). You may also use noninteractive debugging for all or selected MPI ranks.

You may find it useful to peruse the [Intel® Distribution for GDB* Documentation](https://www.intel.com/content/www/us/en/developer/tools/oneapi/distribution-for-gdb-documentation.html). For generic documentation on `gdb`, refer to the FSF guide [Debugging with GDB](https://cdrdv2.intel.com/v1/dl/getContent/671177).

## Preliminaries

To use `gdb-oneapi` effectively, you need to compile and link your application with `-g`. To get anywhere with GPU debugging, the current best practice is to compile and link with `-g -O0`.

Before you debug with `gdb-oneapi`, you must explicitly enable GPU debugging on all the PVC GPUs you are using, on all the nodes you are using. One way to do this is to create a script and execute it across all your compute nodes using `mpiexec`. Here is an example script, which takes an argument `1` to enable debugging or `0` to disable it:

```bash linenums="1" title="helper_toggle_eu_debug.sh"
#!/usr/bin/env bash

export MY_RANK=${PMIX_RANK}
export MY_NODE=${PALS_NODEID}
export MY_LOCAL_RANK=${PALS_LOCAL_RANKID}

eu_debug_toggle() {
  for f in /sys/class/drm/card*/prelim_enable_eu_debug
  do
    echo $1 > $f
  done
  echo "INFO: EU debug state on rank-${MY_RANK}: $(cat /sys/class/drm/card*/prelim_enable_eu_debug | tr '\n' ' ')"
  # sleep 10
}

# One rank per node toggles eu debug:
if [ ${MY_LOCAL_RANK} -eq 0 ]; then
    eu_debug_toggle $1
fi
```

From the interactive prompt on your lead Aurora compute node, issue

```bash linenums="1"
export NNODES=`wc -l < $PBS_NODEFILE`
mpiexec -n $NNODES ./helper_toggle_eu_debug.sh 1
ZET_ENABLE_PROGRAM_DEBUGGING=1
```

## Notes on GPU Debugging

The man page and `gdb-oneapi --help` do not include any information about GPU debugging—only the generic `gdb` information. The current build of `gdb-oneapi` does support the TUI (Text User Interface) mode via the `--tui` command-line switch. The `help` command from the `(gdb)` command prompt command-line interface does not offer any insights into GPU debugging, since the commands to use are really just the normal gdb commands. The key is that it provides access to GPU threads, not just CPU threads. If you query the threads, you will see CPU threads (such as OpenMP threads) and example GPU threads if there are any scheduled. The GPU threads look like the last line in this example output, in which 2.481 is a single GPU thread id running on that GPU. All the other threads in this example are CPU threads, which are mostly waiting for GPU kernels to complete:
    
``` { .bash .no-copy }	
(gdb) info threads -s
Id      Target Id                                          Frame
1.1     Thread 0x155523298880 (LWP 25335) "xgc-es-cpp-gpu" 0x000015552d310407 in sched_yield () from /lib64/libc.so.6
1.3     Thread 0x15551b307700 (LWP 27775) "xgc-es-cpp-gpu" 0x000015552d2efba1 in clock_nanosleep@GLIBC_2.2.5 () from /lib64/libc.so.6
1.4     Thread 0x155515e9b700 (LWP 27809) "xgc-es-cpp-gpu" 0x000015552d32bcdf in epoll_wait () from /lib64/libc.so.6
1.5     Thread 0x155505c17780 (LWP 28039) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.6     Thread 0x155505815800 (LWP 28046) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.7     Thread 0x155505413880 (LWP 28056) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.8     Thread 0x155505011900 (LWP 28062) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.9     Thread 0x155504c0f980 (LWP 28065) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.10    Thread 0x15550480da00 (LWP 28070) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.11    Thread 0x15550440ba80 (LWP 28075) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.12    Thread 0x155504009b00 (LWP 28080) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.13    Thread 0x155503c07b80 (LWP 28096) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.14    Thread 0x155503805c00 (LWP 28110) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.15    Thread 0x155503403c80 (LWP 28121) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.16    Thread 0x155503001d00 (LWP 28137) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.17    Thread 0x155502bffd80 (LWP 28151) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.18    Thread 0x1555027fde00 (LWP 28153) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.19    Thread 0x1555023fbe80 (LWP 28155) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
1.20    Thread 0x155501ffa700 (LWP 28160) "xgc-es-cpp-gpu" 0x000015552d41a70c in pthread_cond_wait@@GLIBC_2.3.2 () from /lib64/libpthread.so.0
* 2.481:0 ZE 0.7.4.0                                       get_f0_grid<Kokkos::Device<Kokkos::Experimental::SYCL, Kokkos::Experimental::SYCLDeviceUSMSpace> >
 (grid=..., magnetic_field=..., species=..., vgrid=..., pol_decomp=...,
 part=..., grid_wts0=..., f0_ptl=...) at getf0.tpp:22
```

You may use the `thread apply` command followed by a specific thread number, followed by a `gdb` command, to execute that command on the specific thread. For example:

```bash
thread apply 2.481 where
```

This will show the call stack for that GPU thread, which should show the GPU kernel function calls.

To set a mode where the stepping commands such as `next` and `stepi` only apply to a single thread, use:

```bash
set scheduler-locking step
```

You may find it useful to look at PVC assembly code. In stepping through GPU code, you may use:

```bash
disassemble $pc - 0x20, $pc + 0x20
```

This shows the assembly code for a range of instructions before and after the current step (program counter). Adjust the hex value larger or smaller than `0x20` to increase/decrease the range of assembly instructions displayed.

When debugging in GPU code, you should be able to use the usual `gdb` inspection commands such as `print` to look at GPU data structures, variables, and registers.

### Stopping at GPU Segmentation Faults (a.k.a. "Page Faults")

GPU segmentation faults are a common reason for debugging. To make `gdb-oneapi` stop where they occur, use

```bash
handle all stop print
```

before the first `run` command (or sometime before you expect the fault to happen).

### Noninteractive Debugging

For MPI programs run using a wrapper script to map ranks to GPUs, you may use a modified wrapper script to invoke a set of predetermined `gdb-oneapi` commands on some or all of the ranks. For example:
```bash linenums="1" title="mpi-wrapper-gdb-oneapi.sh"
#!/bin/bash
display_help() {
  echo " Will map MPI ranks to gpu tiles in compact and then round-robin fashion"
  echo " Usage:"
  echo "   mpiexec --np N $gpu_tile_compact_gdb-oneapi ./a.out"
  exit 1
}

num_gpu=6
num_tile=2

if [ "$#" -eq 0 ] || [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ "$num_gpu" = 0 ]; then
  display_help
fi

gpu_id=$(( (PALS_LOCAL_RANKID / num_tile ) % num_gpu ))
tile_id=$((PALS_LOCAL_RANKID % num_tile))

unset EnableWalkerPartition
export EnableImplicitScaling=0
export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1
export ZE_AFFINITY_MASK=$gpu_id.$tile_id

export ZET_ENABLE_PROGRAM_DEBUGGING=1 # needed for gdb-oneapi:

gdb-oneapi -batch -ex "handle all stop print" -ex run -ex "thread apply all bt" --args $* >out.${PBS_JOBID%.*}.$PALS_RANKID 2>err.${PBS_JOBID%.*}.$PALS_RANKID
```

This example prints a backtrace where GPU segmentation violations or other types of errors occur, and pipes the output into file names including the MPI rank number.
