---
search:
  exclude: true
---

# Darshan
## Introduction
Darshan is a lightweight I/O instrumentation library that can be used to investigate the I/O behavior of production applications. It records statistics, such as the number of files opened, time spent performing I/O, and the amount of data accessed by an application.

## References
- [Darshan Project Site](http://www.mcs.anl.gov/research/projects/darshan/)
- [Guide to Darshan-util Usage](http://www.mcs.anl.gov/research/projects/darshan/docs/darshan-util.html)

## Overview
The Theta environment includes the Darshan module by default.
```
$ module list 2>&1 | grep darshan
21) darshan/3.1.4
```
In most cases, no additional steps are needed to enable Darshan instrumentation. Code compiled with the Cray compiler wrappers {cc, CC, ftn} will include the Darshan library by default. Dynamically linked applications are the most notable exception. See the “Dynamic Linking” section later in this document for instructions on how to enable Darshan instrumentation if you plan to use dynamic libraries.

When a Darshan-enabled job completes, it will generate a single output file containing I/O characterization results. Each output file is placed in the following directory based on the start time of the job: ```/lus/theta-fs0/logs/darshan/theta/<YEAR>/<MONTH>/<DAY>```

The name of the output file will be in the format: ```<USERNAME>_<BINARY_NAME>_id<COBALT_JOB_ID>_<DATE>-<UNIQUE_ID>_<TIMING>.darshan```

A graphical summary of I/O behavior can be generated using the darshan-job-summary.pl utility. The utility should be available in your default path, but if not, it can be loaded using the module command:
```
$ module load darshan
```
The following example shows how to execute the utility:
```
$ darshan-job-summary.pl /lus/theta-fs0/logs/darshan/theta/carns_my-app_id114525_7-27-58921_19.darshan --output ~/job-summary.pdf
```
The entire contents of the output file can be translated into text format for more detailed analysis using the following command:
```
$ darshan-parser /lus/theta-fs0/logs/darshan/theta/carns_my-app_id114525_7-27-58921_19.darshan > ~/job-characterization.txt
```

**Note:** The resulting text file will be verbose. To interpret its contents, use the guidelines in the [Guide to Darshan-parser Output](http://www.mcs.anl.gov/research/projects/darshan/docs/darshan-util.html#_guide_to_darshan_parser_output).

## Dynamic Linking
Darshan can also be used with applications that have been dynamically linked, but in this case you must set explicit environment variables in your job script and your qsub command in order to enable Darshan. See the following example. The DARSHAN_PRELOAD variable will be set automatically when the Darshan module is loaded; the commands below just relay it to the application runtime environment.

```
# job_script.sh
aprun –n <n> -N <N> -e LD_PRELOAD=$DARSHAN_PRELOAD <binary> <args>

$ qsub <..> --env DARSHAN_PRELOAD=$DARSHAN_PRELOAD job_script.sh
```

## Possible Reasons for Missing Output Files
Darshan will not produce output files in the following scenarios:

- Use of languages besides C, C++, or FORTRAN
- Use of non-standard MPI libraries or linkers
- Use of other MPI profilers that conflict with Darshan
- Use of dynamic linking without using LD_PRELOAD
- Job did not call MPI_Finalize(). Reasons may include:
- Job hit wall time limit
- Abnormal termination
- The executable is not an MPI program

In such cases, contact [ALCF Support](mailto:support@alcf.anl.gov) for help. Depending on your situation, it may still be possible to use Darshan.

## Disabling Darshan
We do not recommend disabling Darshan unless you have a specific problem or have been instructed by the ALCF support team to do so. Disabling Darshan limits the ALCF’s ability to assist in supporting your application, and Darshan instrumentation does not add significant overhead to execution time.

### Disabling at Compile Time

The Darshan module can be unloaded, and when an application is linked, the intercept library will no longer be included.
```
$ module unload darshan
$ make
```

### Disabling at Runtime

Darshan can be disabled by setting the DARSHAN_DISABLE=1 environment variable on the aprun command. This does not require relinking the application, and Darshan can be deactivated on a case-by-case basis for existing executables.

```
# job_script.sh
aprun –n <n> -N <N> -e DARSHAN_DISABLE=1 <binary> <args>
```

