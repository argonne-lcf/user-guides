# XPU System Management Interface

## Introduction

This a tool for monitoring and managing Intel data center GPUs and it is designed to simplify administration, maximize reliability and uptime, and improve utilization. Intel(R) XPU System Management Interface (XPU-SMI) is the daemon-less version of XPU Manager and it only provides the local interface.


## Instruction

### Loading the module
```
$ module load xpu-smi
$ xpu-smi --version
xpu-smiCLI:
    Version: 1.2.39.20240906
    Build ID: 11f3c29a

Service:
    Version: 1.2.39.20240906
    Build ID: 11f3c29a
    Level Zero Version: 1.20.2

```

### Usage of `xpu-smi`
```
$ xpu-smi -h
Intel XPU System Management Interface -- v1.2
Intel XPU System Management Interface provides the Intel datacenter GPU model. It can also be used to update the firmware.
Intel XPU System Management Interface is based on Intel oneAPI Level Zero. Before using Intel XPU System Management Interface, the GPU driver and Intel oneAPI Level Zero should be installed rightly.

Supported devcies:
 - Intel Data Center GPU

Usage: xpu-smi [Options]
  xpu-smi -v
  xpu-smi -h
  xpu-smi discovery

Options:
  -h,--help                   Print this help message and exit
  -v,--version                Display version information and exit.

Subcommands:
  discovery                   Discover the GPU devices installed on this machine and provide the device info.
  topology                    Get the system topology.
  diag                        Run some test suites to diagnose GPU.
  health                      Get the GPU device component health status.
  updatefw                    Update GPU firmware
  config                      Get and change the GPU settings.
  ps                          List status of processes.
  vgpu                        Create and remove virtual GPUs in SRIOV configuration.
  stats                       List the GPU statistics.
  dump                        Dump device statistics data.
  log                         Collect GPU debug logs.


$ xpu-smi -h dump
Dump device statistics data.

Usage: xpu-smi dump [Options]
  xpu-smi dump -d [deviceIds] -t [deviceTileIds] -m [metricsIds] -i [timeInterval] -n [dumpTimes]
  xpu-smi dump -d [pciBdfAddress] -t [deviceTileIds] -m [metricsIds] -i [timeInterval] -n [dumpTimes]

Options:
  -h,--help                   Print this help message and exit
  -j,--json                   Print result in JSON format
                              
  -d,--device                 The device IDs or PCI BDF addresses to query. The value of "-1" means all devices.
  -t,--tile                   The device tile IDs to query. If the device has only one tile, this parameter should not be specified.
  -m,--metrics                Metrics type to collect raw data, options. Separated by the comma.
                              0. GPU Utilization (%), GPU active time of the elapsed time, per tile or device. Device-level is the average value of tiles for multi-tiles.
                              1. GPU Power (W), per tile or device.
                              2. GPU Frequency (MHz), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              3. GPU Core Temperature (Celsius Degree), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              4. GPU Memory Temperature (Celsius Degree), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              5. GPU Memory Utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              6. GPU Memory Read (kB/s), per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              7. GPU Memory Write (kB/s), per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              8. GPU Energy Consumed (J), per tile or device.
                              9. GPU EU Array Active (%), the normalized sum of all cycles on all EUs that were spent actively executing instructions. Per tile or device. Device-level is the average value of tiles for multi-tiles.
                              10. GPU EU Array Stall (%), the normalized sum of all cycles on all EUs during which the EUs were stalled.
                                  At least one thread is loaded, but the EU is stalled. Per tile or device. Device-level is the average value of tiles for multi-tiles.
                              11. GPU EU Array Idle (%), the normalized sum of all cycles on all cores when no threads were scheduled on a core. Per tile or device. Device-level is the average value of tiles for multi-tiles.
                              12. Reset Counter, per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              13. Programming Errors, per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              14. Driver Errors, per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              15. Cache Errors Correctable, per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              16. Cache Errors Uncorrectable, per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              17. GPU Memory Bandwidth Utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              18. GPU Memory Used (MiB), per tile or device. Device-level is the sum value of tiles for multi-tiles.
                              19. PCIe Read (kB/s), per device.
                              20. PCIe Write (kB/s), per device.
                              21. Xe Link Throughput (kB/s), a list of tile-to-tile Xe Link throughput.
                              22. Compute engine utilizations (%), per tile.
                              23. Render engine utilizations (%), per tile.
                              24. Media decoder engine utilizations (%), per tile.
                              25. Media encoder engine utilizations (%), per tile.
                              26. Copy engine utilizations (%), per tile.
                              27. Media enhancement engine utilizations (%), per tile.
                              28. 3D engine utilizations (%), per tile.
                              29. GPU Memory Errors Correctable, per tile or device. Other non-compute correctable errors are also included. Device-level is the sum value of tiles for multi-tiles.
                              30. GPU Memory Errors Uncorrectable, per tile or device. Other non-compute uncorrectable errors are also included. Device-level is the sum value of tiles for multi-tiles.
                              31. Compute engine group utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              32. Render engine group utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              33. Media engine group utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              34. Copy engine group utilization (%), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              35. Throttle reason, per tile.
                              36. Media Engine Frequency (MHz), per tile or device. Device-level is the average value of tiles for multi-tiles.
                              
  -i                          The interval (in seconds) to dump the device statistics to screen. Default value: 1 second.
  -n                          Number of the device statistics dump to screen. The dump will never be ended if this parameter is not specified.
                              
  --file                      Dump the raw statistics to the file.
  --ims                       The interval (in milliseconds) to dump the device statistics to file for high-frequency monitoring.
                              The recommended metrics types for high-frequency sampling: GPU power, GPU frequency, GPU utilization,
                              GPU temperature, GPU memory read/write/bandwidth, GPU PCIe read/write, GPU engine utilizations, Xe Link throughput.
  --time                      Dump total time in seconds.
  --date                      Show date in timestamp.


```


## Simple examples

### Discover the GPU devices on an Aurora compute node

```
$ xpu-smi discovery
+-----------+--------------------------------------------------------------------------------------+
| Device ID | Device Information                                                                   |
+-----------+--------------------------------------------------------------------------------------+
| 0         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-926a-83c28645e226                                       |
|           | PCI BDF Address: 0000:18:00.0                                                        |
|           | DRM Device: /dev/dri/card0                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+
| 1         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-41ba-014c9029529c                                       |
|           | PCI BDF Address: 0000:42:00.0                                                        |
|           | DRM Device: /dev/dri/card1                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+
| 2         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-ef03-1b4749c20cb0                                       |
|           | PCI BDF Address: 0000:6c:00.0                                                        |
|           | DRM Device: /dev/dri/card2                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+
| 3         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-a9e9-a41ad019b491                                       |
|           | PCI BDF Address: 0001:18:00.0                                                        |
|           | DRM Device: /dev/dri/card3                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+
| 4         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-1198-22ef5c1e3288                                       |
|           | PCI BDF Address: 0001:42:00.0                                                        |
|           | DRM Device: /dev/dri/card4                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+
| 5         | Device Name: Intel(R) Data Center GPU Max 1550                                       |
|           | Vendor Name: Intel(R) Corporation                                                    |
|           | SOC UUID: 00000000-0000-0000-cec2-eaa8508708c5                                       |
|           | PCI BDF Address: 0001:6c:00.0                                                        |
|           | DRM Device: /dev/dri/card5                                                           |
|           | Function Type: physical                                                              |
+-----------+--------------------------------------------------------------------------------------+

```


### Monitoring GPU utilization, GPU power, GPU frequency and GPU memory utilization of six GPUs on an Aurora compute node

```
$ xpu-smi dump -d 0 1 2 3 4 5 -m 0 1 2 5
Timestamp, DeviceId, GPU Utilization (%), GPU Power (W), GPU Frequency (MHz), GPU Memory Utilization (%)
23:48:25.043,    0,  N/A, 280.50, 1600.00,  N/A
23:48:25.043,    1,  N/A, 286.91, 1600.00,  N/A
23:48:25.043,    2,  N/A, 277.71, 1600.00,  N/A
23:48:25.043,    3,  N/A, 281.91, 1600.00,  N/A
23:48:25.043,    4,  N/A, 279.70, 1600.00,  N/A
23:48:25.043,    5,  N/A, 285.25, 1600.00,  N/A
23:48:26.043,    0,  N/A, 279.97, 1600.00, 11.15
23:48:26.043,    1,  N/A, 287.57, 1600.00, 11.53
23:48:26.043,    2,  N/A, 278.04, 1600.00, 12.56
23:48:26.043,    3,  N/A, 282.30, 1600.00, 12.00
23:48:26.043,    4,  N/A, 279.89, 1600.00, 11.78
23:48:26.043,    5,  N/A, 285.01, 1600.00, 11.78
23:48:27.043,    0,  N/A, 280.53, 1600.00, 11.15
23:48:27.043,    1,  N/A, 286.90, 1600.00, 11.53
23:48:27.043,    2,  N/A, 277.65, 1600.00, 12.56
23:48:27.043,    3,  N/A, 282.60, 1600.00, 12.00
23:48:27.043,    4,  N/A, 279.76, 1600.00, 11.78
23:48:27.043,    5,  N/A, 284.93, 1600.00, 11.78
23:48:28.043,    0,  N/A, 280.52, 1600.00, 11.15
23:48:28.043,    1,  N/A, 286.88, 1600.00, 11.53
23:48:28.043,    2,  N/A, 277.71, 1600.00, 12.56
23:48:28.043,    3,  N/A, 282.09, 1600.00, 12.00
23:48:28.043,    4,  N/A, 280.09, 1600.00, 11.78
23:48:28.043,    5,  N/A, 285.01, 1600.00, 11.78
23:48:29.043,    0,  N/A, 279.89, 1600.00, 11.15
23:48:29.043,    1,  N/A, 287.58, 1600.00, 11.53
23:48:29.043,    2,  N/A, 277.54, 1600.00, 12.56
23:48:29.043,    3,  N/A, 282.07, 1600.00, 12.00
23:48:29.043,    4,  N/A, 279.88, 1600.00, 11.78
23:48:29.043,    5,  N/A, 285.72, 1600.00, 11.78
23:48:30.043,    0,  N/A, 280.54, 1600.00, 11.15
23:48:30.043,    1,  N/A, 286.89, 1600.00, 11.53
23:48:30.043,    2,  N/A, 277.63, 1600.00, 12.56
23:48:30.043,    3,  N/A, 282.55, 1600.00, 12.00
23:48:30.043,    4,  N/A, 279.78, 1600.00, 11.78
23:48:30.043,    5,  N/A, 285.25, 1600.00, 11.78
23:48:31.043,    0,  N/A, 280.05, 1600.00, 11.15
23:48:31.043,    1,  N/A, 286.92, 1600.00, 11.53
23:48:31.043,    2,  N/A, 278.32, 1600.00, 12.56
23:48:31.043,    3,  N/A, 282.17, 1600.00, 12.00
23:48:31.043,    4,  N/A, 280.62, 1600.00, 11.78
23:48:31.043,    5,  N/A, 285.00, 1600.00, 11.78
23:48:32.043,    0,  N/A, 279.88, 1600.00, 11.15
23:48:32.043,    1,  N/A, 287.18, 1600.00, 11.53
23:48:32.043,    2,  N/A, 277.83, 1600.00, 12.56
23:48:32.043,    3,  N/A, 281.86, 1600.00, 12.00
23:48:32.043,    4,  N/A, 280.41, 1600.00, 11.78
23:48:32.043,    5,  N/A, 285.70, 1600.00, 11.78
23:48:33.043,    0,  N/A, 280.38, 1600.00, 11.15
23:48:33.043,    1,  N/A, 286.77, 1600.00, 11.53
23:48:33.043,    2,  N/A, 277.72, 1600.00, 12.56
23:48:33.043,    3,  N/A, 282.43, 1600.00, 12.00
23:48:33.043,    4,  N/A, 279.98, 1600.00, 11.78
23:48:33.043,    5,  N/A, 285.77, 1600.00, 11.78
23:48:34.043,    0,  N/A, 280.11, 1600.00, 11.15
23:48:34.043,    1,  N/A, 287.02, 1600.00, 11.53
23:48:34.043,    2,  N/A, 278.22, 1600.00, 12.56
23:48:34.043,    3,  N/A, 282.26, 1600.00, 12.00
23:48:34.043,    4,  N/A, 280.68, 1600.00, 11.78
23:48:34.043,    5,  N/A, 285.06, 1600.00, 11.78
23:48:35.043,    0,  N/A, 280.21, 1600.00, 11.15
23:48:35.043,    1,  N/A, 287.58, 1600.00, 11.53
23:48:35.043,    2,  N/A, 277.82, 1600.00, 12.56
23:48:35.043,    3,  N/A, 281.84, 1600.00, 12.00
23:48:35.043,    4,  N/A, 280.40, 1600.00, 11.78
23:48:35.043,    5,  N/A, 285.54, 1600.00, 11.78
^C
```

## References
[XPU manager github repository](https://github.com/intel/xpumanager)  
[2025 ALCF INCITE Hackathon virtual week](./Presentations/2025_INCITE_Hackathon_Part_1_light-weight-tools-iprof_unitrace_xpu-smi.pdf)


