# Darshan on ThetaGPU
## Overview
Darshan instrumentation on ThetaGPU is not automatically included into the binary like Theta. The user must set the ```LD_PRELOAD``` variable as part of running the job.

Logs will be generated in the directory: /lus/grand/logs/darshan/thetagpu/<YEAR>/<MONTH>/<DAY>

In order to view a log, use the darshan-parser utility.
```
module load darshan 
mpirun ... -x LD_PRELOAD=$DARSHAN_PRELOAD
```

## More information
See the [Theta documentation](https://www.alcf.anl.gov/support/user-guides/theta/performance-tools/darshan/index.html) for more ALCF specific Darshan usage.  
  
  
