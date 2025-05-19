# Application Performance Snapshot

## Introduction

Use Application Performance Snapshot for a quick scan of performance aspects that relate to compute-intensive applications:

* MPI usage
* OpenMP usage
* Intel® oneAPI Collective Communications Library (oneCCL) usage
* CPU/GPU usage
* Efficiency of memory access
* Vectorization
* I/O and memory footprint

Application Performance Snapshot displays key optimization areas and suggests specialized tools for tuning particular performance aspects, such as Intel VTune Profiler and Intel® Advisor. You can run Application Performance Snapshot on large MPI workloads to analyze scalability issues. 


## Instruction

### Loading the module
```
$ module load oneapi
$ aps --version
Intel(R) VTune(TM) Profiler 2025.0.1 (build 629235) Command Line Tool
Copyright (C) 2009 Intel Corporation. All rights reserved.

```

### Usage of `aps`
```
$ aps -h
Intel(R) VTune(TM) Profiler 2025.0.1 (build 629235) Command Line Tool
Copyright (C) 2009 Intel Corporation. All rights reserved.
Usage: 1. aps [--result-dir=<dir> | -r=<dir>] [--start-paused] [--collection-mode=<mode> | -c=<mode>] <app>
       2. <mpi_launcher> <mpi_parameters> aps [--result-dir=<dir> | -r=<dir>] [--collection-mode=<mode> | -c=<mode>] [--stat-level=<0-5> | -L <0-5>] [--mpi-imbalance=<0-2> | -I <0-2>] [--storage-format=<format> | -F <format>] <app>
       3. aps --report | -O <option> <dir>
       4. aps <option>
1. Run analysis for an application or script <app> and store results in <dir>
2. Run analysis for an MPI application <app> and store results in <dir>
     --collection-mode=<mode>       Specify a comma separated list of data to collect. Possible values: hwc - hardware counters, omp - OpenMP statistics, mpi - MPI statistics, all - all possible data (default).
     --start-paused                 "Start data collection in the paused mode. Data collection resumes when the application calls __itt_resume or MPI_Pcontrol(1).
     --stat-level                   Set MPI statistic collection level.
     --mpi-imbalance                Set MPI imbalance collection mode.
     --tmp-dir=<path>               Specify the directory path where temporary data (generated during the collection run) is saved.
     --storage-format=<format>      Specify the trace format: simple (a small number of MPI ranks per node) or compact (numerous MPI ranks per node).
3. Show analysis report based on data from <dir>
     Tip: use
       aps-report --help
     to review report options. Additional details include statistics view by ranks, message sizes, collective operations, and communicators, as well as the ability to explore rank-to-rank and node-to-node communication statistics.
4. Show additional product info. Where <option> can be:
     --help, -h show this help and exit
     --version show version information and exit



```

### Running with an MPI application

#### Collection on compute nodes
```
$ mpirun <mpi arguments> aps -r <aps_result_dir> <application> <args>
```

#### HTML report generation
```
$ aps --report <aps_result_dir>
```

## Simple examples

### `aps` collection for `amr-wind` application with 48 MPI ranks on four Aurora nodes

#### Collection

```
$ mpirun -n 48 gpu_tile_compact.sh aps -r aps_report_amr-wind_4N ../amr_wind abl_godunov_4N_ST100.inp 
==============================================================================
                AMR-Wind (https://github.com/exawind/amr-wind)

  AMR-Wind version :: v3.4.2
  AMR-Wind Git SHA :: ed475a0533dfacf1fdff0b707518ccf99040d9f9
  AMReX version    :: 25.04-9-g30a9768150c4

...<skipping application standard output>...

AMReX (25.04-9-g30a9768150c4) finalized

Intel(R) VTune(TM) Profiler 2025.0.1 collection completed successfully. Use the "aps --report /lus/flare/projects/Aurora_deployment/jkwack/JK_AT_Tools/Apps/amr-wind_v3.4.2/build_20250513/jk_test/aps_report_amr-wind_4N" command to generate textual and HTML reports for the profiling session.


```

During the collection, the following warning can be ignored:
```
vtune: Warning: EMON Collector Message: Warning: PMU counter(s) [FP0 FP1 FP2 ] is (are) already in use by other software and the data collected on these counter(s) may be incorrect
```

#### Report

```
$ aps --report /lus/flare/projects/Aurora_deployment/jkwack/JK_AT_Tools/Apps/amr-wind_v3.4.2/build_20250513/jk_test/aps_report_amr-wind_4N
| Summary information
|--------------------------------------------------------------------
  Application                   : amr_wind
  Report creation date          : 2025-05-19 00:04:28
  Number of ranks               : 48
  Ranks per node                : 6
  HW Platform                   : Intel(R) Xeon(R) Processor code named Sapphirerapids
  Frequency                     : 2.00 GHz
  Logical core count per node   : 208
  Collector type                : Event-based sampling driver,Event-based counting driver,User-mode sampling and tracing
  Used statistics               : /lus/flare/projects/Aurora_deployment/jkwack/JK_AT_Tools/Apps/amr-wind_v3.4.2/build_20250513/jk_test/aps_report_amr-wind_4N
|
| Your application may underutilize the GPU.
|  Run a GPU Offload or a GPU Compute/Media Hotspots analysis with VTune Profiler to discover how to better utilize the GPU.
|
  Elapsed Time:                             468.82 s
  SP GFLOPS:                                  0.00
  DP GFLOPS:                                  0.01
  Average CPU Frequency:                      2.40 GHz
  IPC Rate:                                   0.17
| The IPC value may be too low.
| This could be caused by issues such as memory stalls, instruction starvation,
| branch misprediction or long latency instructions.
| Use Intel(R) VTune(TM) Profiler Microarchitecture Exploration analysis to
| specify particular reasons of low IPC.
  GPU Accumulated Time:                    1654.45 s
  MPI Time:                                 218.00 s            46.51% of Elapsed Time
| Your application is MPI bound. This may be caused by high busy wait time
| inside the library (imbalance), non-optimal communication schema or MPI
| library settings. Explore the MPI Imbalance metric if it is available or use
| MPI profiling tools like Intel(R) Trace Analyzer and Collector to explore
| possible performance bottlenecks.
| Some of the individual values contributing to this average metric are
| statistical outliers that can significantly distort the average metric value.
| They can also be a cause of performance degradation.
| Please use --counters or --metrics="MPI Time" reports for details.
    MPI Imbalance:                            N/A*
| * No information about MPI Imbalance time is available. Set APS_IMBALANCE_TYPE
| to 1 or 2 to collect it.
    Top 5 MPI functions (avg time):
        MPI_Waitall:                        126.34 s            26.95% of Elapsed Time
| Some of the individual values contributing to this average metric are
| statistical outliers that can significantly distort the average metric value.
| They can also be a cause of performance degradation.
| Please use --counters or --metrics="MPI Hotspot 1 - MPI_Waitall" reports for
| details.
        MPI_Isend:                           50.44 s            10.76% of Elapsed Time
        MPI_Allreduce:                       23.88 s             5.10% of Elapsed Time
        MPI_Testall:                          9.70 s             2.07% of Elapsed Time
        MPI_Barrier:                          3.44 s             0.73% of Elapsed Time
| Some of the individual values contributing to this average metric are
| statistical outliers that can significantly distort the average metric value.
| They can also be a cause of performance degradation.
| Please use --counters or --metrics="MPI Hotspot 5 - MPI_Barrier" reports for
| details.
  Physical Core Utilization:                 93.66%
  Average Physical Core Utilization:         97.42 out of 104 Physical Cores
  GPU Stack Utilization:                     29.40%
| The percentage of time when the XVEs were stalled or idle is high, which has a
| negative impact on compute-bound applications.
    XVE State:
       Active:                               20.77%
       Stalled:                               5.75%
       Idle:                                 73.46%
| A significant portion of GPU time is spent idle. This is usually caused by
| imbalance or thread scheduling problems.
  GPU Occupancy:                             22.65% of Peak Value
  Memory Stalls:                             17.76% of Pipeline Slots
    Cache Stalls:                            22.05% of Cycles
| A significant proportion of cycles are spent on data fetches from cache. Use
| Intel(R) VTune(TM) Profiler Memory Access analysis to see if accesses to L2 or
| L3 cache are problematic and consider applying the same performance tuning as
| you would for a cache-missing workload. This may include reducing the data
| working set size, improving data access locality, blocking or partitioning the
| working set to fit in the lower cache levels, or exploiting hardware
| prefetchers.
    DRAM Stalls:                              0.10% of Cycles
    DRAM Bandwidth
       Peak:                                149.06 GB/s
| Some of the individual values contributing to this average metric are
| statistical outliers that can significantly distort the average metric value.
| They can also be a cause of performance degradation.
| Please use --counters or --metrics="DRAM Bandwidth Peak" reports for details.
       Average:                               5.20 GB/s
       Bound:                                 3.45%
  Vectorization:                             14.38%
     Instruction Mix:
       SP FLOPs:                              0.00% of uOps
       DP FLOPs:                              0.00% of uOps
       Non-FP:                              100.00% of uOps
     FP Arith/Mem Rd Instr. Ratio:            0.00
     FP Arith/Mem Wr Instr. Ratio:            0.00
 Average PCI Bandwidth:
   Average PCIe Bandwidth Usage by GPU:
     Inbound PCIe Read:                     330.40 MB/s
     Inbound PCIe Write:                   2963.48 MB/s
   Average PCIe Bandwidth Usage by Network Controller Devices:
     Inbound PCIe Read:                    2793.15 MB/s
     Inbound PCIe Write:                   2204.37 MB/s
 Disk I/O Bound:                              0.09 s             0.02% of Elapsed Time
      Disk read:                             153.1 KB
      Disk write:                             50.1 GB
 Memory Footprint:
 Resident:
       Per node:
           Peak resident set size    :         5221.00 MB (node x4013c2s7b0n0)
           Average resident set size :         5127.75 MB
       Per rank:
           Peak resident set size    :         1033.00 MB (rank 7)
           Average resident set size :          854.62 MB
 Virtual:
       Per node:
           Peak memory consumption    :     32094024.00 MB (node x4013c2s7b0n0)
           Average memory consumption :     32093921.75 MB
       Per rank:
           Peak memory consumption    :      5349211.00 MB (rank 7)
           Average memory consumption :      5348986.96 MB

Graphical representation of this data is available in the HTML report: /lus/flare/projects/Aurora_deployment/jkwack/JK_AT_Tools/Apps/amr-wind_v3.4.2/build_20250513/jk_test/aps_report_20250519_001450.html

```


Download the HTML file and open it on a web browser.

![APS HTML report snapshot](images/aps-01.png "APS HTML report snapshot")  
[APS HTML report for this example](results/aps_report_20250519_001450.html)


## References  
[Intel Application Performance Snapshot User's Guide](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide-application-snapshot-linux/2025-0/overview.html)

[2025 ALCF INCITE Hackathon virtual week](./Presentations/2025_INCITE_Hackathon_Part_2_Intel_Analyzers.pdf)

