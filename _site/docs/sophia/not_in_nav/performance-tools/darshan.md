# Darshan on ThetaGPU

## Overview

Darshan instrumentation on ThetaGPU is not automatically included in the binary like on Theta. The user must set the `LD_PRELOAD` variable as part of running the job.

Logs will be generated in the directory: `/lus/eagle/logs/darshan/thetagpu/<YEAR>/<MONTH>/<DAY>`

To view a log, use the `darshan-parser` utility.

```bash
module load darshan 
mpirun ... -x LD_PRELOAD=$DARSHAN_PRELOAD
```

## More information

<!-- Needs updating for Sophia: -->
<!-- See the [Theta documentation](../../theta/performance-tools/darshan.md) for more ALCF-specific Darshan usage. -->

---