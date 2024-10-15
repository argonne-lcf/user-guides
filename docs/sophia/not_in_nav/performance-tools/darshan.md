# Darshan on ThetaGPU
## Overview
Darshan instrumentation on ThetaGPU is not automatically included into the binary like Theta. The user must set the ```LD_PRELOAD``` variable as part of running the job.

Logs will be generated in the directory: /lus/eagle/logs/darshan/thetagpu/<YEAR>/<MONTH>/<DAY>

In order to view a log, use the darshan-parser utility.
```
module load darshan 
mpirun ... -x LD_PRELOAD=$DARSHAN_PRELOAD
```

## More information
<!-- Needs updating for sophia: -->
<!-- See the [Theta documentation](../../theta/performance-tools/darshan.md) for more ALCF specific Darshan usage.   -->
  
  
