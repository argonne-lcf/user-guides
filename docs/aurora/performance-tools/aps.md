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
[APS HTML report for this example](./results/aps_report_20250519_001450.html)


#### `aps-report` CLI interface

```
$ aps-report --metrics=? aps_report_amr-wind_4N/
| Available Metrics:
|--------------------
Elapsed Time
MPI Time
MPI Time
MPI Imbalance
MPI Hotspot 1 - MPI_Waitall
MPI Hotspot 1 - MPI_Waitall
MPI Hotspot 2 - MPI_Isend
MPI Hotspot 2 - MPI_Isend
MPI Hotspot 3 - MPI_Allreduce
MPI Hotspot 3 - MPI_Allreduce
MPI Hotspot 4 - MPI_Testall
MPI Hotspot 4 - MPI_Testall
MPI Hotspot 5 - MPI_Barrier
MPI Hotspot 5 - MPI_Barrier
Disk I/O Bound
Disk I/O Bound
Disk read
Disk write
Resident Memory Usage per Rank
Resident Memory Usage per Node
Virtual Memory Usage per Rank
Virtual Memory Usage per Node
Instructions Per Cycle Rate
Average CPU Frequency
Physical Core Utilization
Average Physical Core Utilization
Memory Stalls
Cache Stalls
DRAM Stalls
Average DRAM Bandwidth
DRAM Bandwidth Peak
DRAM Bandwidth Average
DRAM Bandwidth Bound
SP GFLOPS
DP GFLOPS
Vectorization
SP FLOPs
DP FLOPs
Non-FP
FP Arith/Mem Rd Instr. Ratio
FP Arith/Mem Wr Instr. Ratio
GPU Accumulated Time
GPU Stack Utilization
XVE State: Active
XVE State: Stalled
XVE State: Idle
GPU Occupancy
GPU Inbound PCIe Read
GPU Inbound PCIe Write
GPU Outbound PCIe Read
GPU Outbound PCIe Write
Network Controller Inbound PCIe Read
Network Controller Inbound PCIe Write
Network Controller Outbound PCIe Read
Network Controller Outbound PCIe Write
Inbound PCIe Read Per Device
Inbound PCIe Write Per Device
Outbound PCIe Read Per Device
Outbound PCIe Write Per Device
GPU Accumulated Time Per Device
GPU Stack Utilization Per Device
XVE State: Active Per Device
XVE State: Stalled Per Device
XVE State: Idle Per Device
GPU Occupancy Per Device


$ aps-report --metrics="GPU Stack Utilization Per Device, OpenMP Offload Time, GPU Accumulated Time Per Device, MPI Time" aps_report_amr-wind_4N
| Metric Table%
|--------------------------------------------------------------------------------------------------------------------------------
Metric Name                                Node Name           Rank    Device Type      Device Name   Metric Value   Outlier Type
MPI Time, s                            x4013c2s1b0n0             17            N/A              N/A        230.487           None
MPI Time, s                            x4013c2s6b0n0             14            N/A              N/A        228.453           None
MPI Time, s                            x4013c2s1b0n0             33            N/A              N/A        226.592           None
MPI Time, s                            x4013c2s7b0n0             15            N/A              N/A         226.49           None
MPI Time, s                            x4013c2s0b0n0             40            N/A              N/A        226.197           None
MPI Time, s                            x4013c2s1b0n0             41            N/A              N/A         226.18           None
MPI Time, s                            x4013c2s4b0n0             28            N/A              N/A        225.857           None
MPI Time, s                            x4013c2s4b0n0             20            N/A              N/A        225.454           None
MPI Time, s                            x4013c2s6b0n0             46            N/A              N/A        225.207           None
MPI Time, s                            x4013c2s4b0n0             12            N/A              N/A        225.057           None
MPI Time, s                            x4013c2s0b0n0             32            N/A              N/A        225.031           None
MPI Time, s                            x4013c2s3b0n0             19            N/A              N/A        225.017           None
MPI Time, s                            x4013c2s2b0n0             34            N/A              N/A        224.819           None
MPI Time, s                            x4013c2s4b0n0             36            N/A              N/A        224.488           None
MPI Time, s                            x4013c2s3b0n0             43            N/A              N/A        224.431           None
MPI Time, s                            x4013c2s4b0n0             44            N/A              N/A        223.787           None
MPI Time, s                            x4013c2s6b0n0             30            N/A              N/A         223.77           None
MPI Time, s                            x4013c2s7b0n0             23            N/A              N/A        223.284           None
MPI Time, s                            x4013c2s6b0n0             38            N/A              N/A        222.985           None
MPI Time, s                            x4013c2s3b0n0             35            N/A              N/A        222.908           None
MPI Time, s                            x4013c2s5b0n0             29            N/A              N/A        222.797           None
MPI Time, s                            x4013c2s7b0n0             31            N/A              N/A        222.197           None
MPI Time, s                            x4013c2s7b0n0             39            N/A              N/A        222.187           None
MPI Time, s                            x4013c2s5b0n0             21            N/A              N/A        222.004           None
MPI Time, s                            x4013c2s0b0n0             24            N/A              N/A        221.966           None
MPI Time, s                            x4013c2s5b0n0             13            N/A              N/A        221.496           None
MPI Time, s                            x4013c2s0b0n0             16            N/A              N/A         221.42           None
MPI Time, s                            x4013c2s6b0n0              6            N/A              N/A        221.271           None
MPI Time, s                            x4013c2s5b0n0             45            N/A              N/A        221.088           None
MPI Time, s                            x4013c2s5b0n0             37            N/A              N/A        219.799           None
MPI Time, s                            x4013c2s7b0n0              7            N/A              N/A        219.199           None
MPI Time, s                            x4013c2s2b0n0             18            N/A              N/A        218.941           None
MPI Time, s                            x4013c2s0b0n0              8            N/A              N/A        218.837           None
MPI Time, s                            x4013c2s1b0n0              9            N/A              N/A        217.493           None
MPI Time, s                            x4013c2s2b0n0             26            N/A              N/A        217.038           None
MPI Time, s                            x4013c2s2b0n0             10            N/A              N/A        216.905           None
MPI Time, s                            x4013c2s3b0n0              3            N/A              N/A        216.463           None
MPI Time, s                            x4013c2s3b0n0             11            N/A              N/A        214.698           None
MPI Time, s                            x4013c2s1b0n0              1            N/A              N/A        211.921           None
MPI Time, s                            x4013c2s2b0n0              2            N/A              N/A        210.556           None
MPI Time, s                            x4013c2s1b0n0             25            N/A              N/A        207.048           None
MPI Time, s                            x4013c2s2b0n0             42            N/A              N/A        203.637           None
MPI Time, s                            x4013c2s3b0n0             27            N/A              N/A        202.455           None
MPI Time, s                            x4013c2s7b0n0             47            N/A              N/A        201.554           None
MPI Time, s                            x4013c2s6b0n0             22            N/A              N/A        198.629           None
MPI Time, s                            x4013c2s4b0n0              4            N/A              N/A          195.9           None
MPI Time, s                            x4013c2s5b0n0              5            N/A              N/A        189.878      Statistic
MPI Time, s                            x4013c2s0b0n0              0            N/A              N/A        180.196      Statistic
MPI Time, % of Elapsed Time            x4013c2s1b0n0             17            N/A              N/A        49.1639           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0             14            N/A              N/A        48.7335           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0             15            N/A              N/A        48.3632           None
MPI Time, % of Elapsed Time            x4013c2s1b0n0             33            N/A              N/A        48.3331           None
MPI Time, % of Elapsed Time            x4013c2s0b0n0             40            N/A              N/A        48.2486           None
MPI Time, % of Elapsed Time            x4013c2s1b0n0             41            N/A              N/A        48.2453           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0             28            N/A              N/A        48.1787           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0             20            N/A              N/A        48.0928           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0             46            N/A              N/A         48.041           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0             19            N/A              N/A        48.0165           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0             12            N/A              N/A        48.0082           None
MPI Time, % of Elapsed Time            x4013c2s0b0n0             32            N/A              N/A        47.9998           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0             34            N/A              N/A        47.9635           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0             43            N/A              N/A        47.8914           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0             36            N/A              N/A        47.8867           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0             44            N/A              N/A        47.7371           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0             30            N/A              N/A        47.7344           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0             23            N/A              N/A        47.6786           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0             38            N/A              N/A        47.5669           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0             35            N/A              N/A        47.5666           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0             29            N/A              N/A        47.5233           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0             31            N/A              N/A        47.4464           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0             39            N/A              N/A        47.4443           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0             21            N/A              N/A        47.3541           None
MPI Time, % of Elapsed Time            x4013c2s0b0n0             24            N/A              N/A         47.346           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0             13            N/A              N/A        47.2459           None
MPI Time, % of Elapsed Time            x4013c2s0b0n0             16            N/A              N/A        47.2297           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0              6            N/A              N/A        47.2014           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0             45            N/A              N/A        47.1587           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0             37            N/A              N/A        46.8839           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0              7            N/A              N/A        46.8063           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0             18            N/A              N/A        46.7096           None
MPI Time, % of Elapsed Time            x4013c2s0b0n0              8            N/A              N/A        46.6787           None
MPI Time, % of Elapsed Time            x4013c2s1b0n0              9            N/A              N/A        46.3922           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0             26            N/A              N/A        46.3036           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0             10            N/A              N/A        46.2752           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0              3            N/A              N/A        46.1912           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0             11            N/A              N/A        45.8147           None
MPI Time, % of Elapsed Time            x4013c2s1b0n0              1            N/A              N/A        45.2039           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0              2            N/A              N/A        44.9206           None
MPI Time, % of Elapsed Time            x4013c2s1b0n0             25            N/A              N/A        44.1644           None
MPI Time, % of Elapsed Time            x4013c2s2b0n0             42            N/A              N/A        43.4445           None
MPI Time, % of Elapsed Time            x4013c2s3b0n0             27            N/A              N/A        43.2021           None
MPI Time, % of Elapsed Time            x4013c2s7b0n0             47            N/A              N/A        43.0384           None
MPI Time, % of Elapsed Time            x4013c2s6b0n0             22            N/A              N/A        42.3713           None
MPI Time, % of Elapsed Time            x4013c2s4b0n0              4            N/A              N/A        41.7886           None
MPI Time, % of Elapsed Time            x4013c2s5b0n0              5            N/A              N/A        40.5016      Statistic
MPI Time, % of Elapsed Time            x4013c2s0b0n0              0            N/A              N/A        38.4364      Statistic
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 0 Stack 0        303.694           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 0 Stack 0        300.652           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 0 Stack 0        299.451           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 0 Stack 0         294.79           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 0 Stack 1         293.95           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 0 Stack 1        293.677           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 0 Stack 0         290.41           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 0 Stack 1        290.318           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 0 Stack 1        289.743           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 0 Stack 0        289.463           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 0 Stack 0        284.887           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 0 Stack 0        284.826           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 1 Stack 1        276.046           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 1 Stack 0        273.294           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 0 Stack 1        272.365           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 1 Stack 1         271.89           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 2 Stack 0        271.825           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 2 Stack 0        271.599           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 1 Stack 0        271.522           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 1 Stack 0        271.499           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 2 Stack 1        271.448           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 1 Stack 1        271.348           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 0 Stack 1        271.318           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 1 Stack 1         271.11           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 2 Stack 0        270.924           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 2 Stack 1        270.449           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 1 Stack 1        270.418           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 1 Stack 1        270.406           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 2 Stack 0        270.349           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 0 Stack 1        270.251           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 2 Stack 0        270.194           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 2 Stack 1        270.167           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 1 Stack 0        269.773           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 0 Stack 1         269.59           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 1 Stack 0        269.505           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 2 Stack 0        269.164           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 1 Stack 0        269.037           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 2 Stack 1        268.696           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 2 Stack 0        268.338           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 2 Stack 1        268.169           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 2 Stack 0        268.071           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 1 Stack 0         268.01           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 1 Stack 1        267.924           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 2 Stack 1        267.462           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 2 Stack 1        267.385           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 1 Stack 1         267.25           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 2 Stack 1        266.918           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 1 Stack 0        266.015           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Accumulated Time Per Device, s     x4013c2s7b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s6b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s5b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s4b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s3b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s2b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s1b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Accumulated Time Per Device, s     x4013c2s0b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 0 Stack 0           64.8           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 0 Stack 0           64.1           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 0 Stack 0           63.9           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 0 Stack 0           62.9           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 0 Stack 1           62.7           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 0 Stack 1           62.7           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 0 Stack 1           61.9           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 0 Stack 0           61.9           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 0 Stack 1           61.8           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 0 Stack 0           61.7           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 0 Stack 0           60.8           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 0 Stack 0           60.8           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 1 Stack 1           58.9           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 1 Stack 0           58.3           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 0 Stack 1           58.1           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 2 Stack 0             58           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 1 Stack 1             58           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 2 Stack 1           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 2 Stack 0           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 1 Stack 1           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 1 Stack 1           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 1 Stack 0           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 1 Stack 0           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 0 Stack 1           57.9           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 2 Stack 0           57.8           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 2 Stack 1           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 2 Stack 0           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 2 Stack 0           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 1 Stack 1           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 1 Stack 1           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 0 Stack 1           57.7           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 2 Stack 1           57.6           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 1 Stack 0           57.6           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 1 Stack 0           57.5           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 0 Stack 1           57.5           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 2 Stack 0           57.4           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 1 Stack 0           57.4           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 2 Stack 1           57.3           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 2 Stack 1           57.2           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 2 Stack 0           57.2           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 2 Stack 0           57.2           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 1 Stack 0           57.2           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 2 Stack 1           57.1           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 1 Stack 1           57.1           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 2 Stack 1             57           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 1 Stack 1             57           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 2 Stack 1           56.9           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 1 Stack 0           56.7           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 5 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 5 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 4 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 4 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 3 Stack 1              0           None
GPU Stack Utilization Per Device, %    x4013c2s7b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s6b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s5b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s4b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s3b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s2b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s1b0n0            N/A            GPU    GPU 3 Stack 0              0           None
GPU Stack Utilization Per Device, %    x4013c2s0b0n0            N/A            GPU    GPU 3 Stack 0              0           None

```



## References  
[Intel Application Performance Snapshot User's Guide](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide-application-snapshot-linux/2025-0/overview.html)

[2025 ALCF INCITE Hackathon virtual week](./Presentations/2025_INCITE_Hackathon_Part_2_Intel_Analyzers.pdf)

