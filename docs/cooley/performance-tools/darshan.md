# Darshan on Cooley
## Overview
Darshan is a lightweight I/O instrumentation library that can be used to investigate the I/O behavior of production applications. It records statistics, such as the number of files opened, time spent performing I/O, and the amount of data accessed by an application.

## Enabling Darshan on Cooley
Darshan is not automatically enabled for all jobs on Cooley. Unlike Theta, all applications on Cooley are dynamically linked by default, which means that Darshan must be loaded at runtime using the LD_PRELOAD environment variable. In order to instrument a job on Cooley, you must first add the SoftEnv key +darshan to your ~/.soft.cooley file and run the “resoft” command. 

Then add the following to the mpirun command line in your job script:
```
 --env LD_PRELOAD=$DARSHAN_PRELOAD
 ```
 
Example:
```
# within Cooley job script
mpirun --env LD_PRELOAD=$DARSHAN_PRELOAD -np <number of processes> -f $COBALT_NODEFILE ./app.exe <arguments>
```

After your job completes, you can find the Darshan output file in the following directory:
```
/lus/theta-fs0/logs/darshan/cooley/<year>/<month>/<day>
```

The same tools described in the Theta documentation can be used to interpret Darshan output files generated on Cooley.

