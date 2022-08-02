# Library and Compiler Tracking on ThetaGPU

## Overview
The ALCF **trackdeps** software monitors which compilers and libraries are being used to build executables on our systems.  The information collected is used to inform ALCF and DOE decisions on support and research priorities.

Currently, tracking is enabled by default.  It can be disabled by running:

```
module unload trackdeps
```

## Recorded Executable Information
When executables are built with tracking enabled, the input files used to build the executable are recorded by the system. To view what input files the system has recorded for any particular executable, with the softenv key enabled, run:

```
trackdeps-show /path/to/the/executable
```
