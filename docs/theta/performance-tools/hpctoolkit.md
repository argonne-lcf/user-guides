# HPCToolkit on Theta
## Introduction
HPCToolkit is an open-source suite of tools for profile-based performance analysis of applications. Below is a brief description on how to use the HPCToolkit on the ALCF XC40 system. For more detailed information on using HPCToolkit and its capabilities, see the [HPCToolkit website](http://hpctoolkit.org/).

## References
[HPCToolkit Website](http://hpctoolkit.org)
[HPCT Documentation Page](http://hpctoolkit.org/documentation.html)

## Installation on the ALCF Theta System
The HPCToolkit is installed in:
```
/projects/Tools/hpctoolkit/pkgs-theta/hpctoolkit
```
## Using HPCToolkit
### Environment Setup
To avoid conflicts with the default loaded I/O performance monitoring tool, unload the darshan moduile:
```
module unload darshan
```
Add the HPCToolkit bin directory to your path as follows:
```
export PATH=$PATH:/projects/Tools/hpctoolkit/pkgs-theta/hpctoolkit/bin (bash) 
setenv PATH $PATH:/projects/Tools/hpctoolkit/pkgs-theta/hpctoolkit/bin (csh)
```
### Compile the Code to Use HPCToolkit
It is not necessary to modify your source code to use the HPCToolkit. However, for statically linked executables; the executable must be built using the HPCToolkit hpclink command as the linker.

The build procedure when using HPCToolkit is:
- Compile the individual source files as usual, adding the flag for debugging symbols if not already used. For example:
```
cc -g -O3 -c routine1.c 
ftn -g -O3 -c routine2.for
```
This example shows compilation using a medium level of optimization (-O3). Any level of optimization is supported by HPCToolkit. However, to avoid possible problems, programs should be compiled without inter-procedural optimization (IPA) by specifying the "-qnoipa" option.

- Link using the hpclink command before the name of the compiler normally used to link the program. For example:
```
hpclink cc -o myprog routine1.o routine2.o ...
```
**Note:** The link step may take longer than using the usual linker as additional steps are performed during the linking process.

### Run the Code
To run the program, submit to the queues as usual (using the executable built with the hpclink command) but set the HPCT environment variables (HPCRUN_TRACE, HPCRUN_EVENT_LIST, HPCRUN_PROCESS_FRACTION) to specify the data to be collected. For example:
```
$ qsub -n 8 -t 30 -A project jobscript.sh
$ cat jobscript.sh 

#!/bin/bash 
export HPCRUN_EVENT_LIST="REALTIME@10000 PAPI_TOT_CYC@14000000 CYCLES@f100" 
export HPCRUN_TRACE=1 
aprun -n 512 -N 64 <executable>
```
Possible profiling events include:

Time - profile the program using a time interval. Time-based profiling is specified by using the event:
- ```REALTIME@<value>``` - profile based on wallclock time with a sampling interval <value> in micro-seconds
  Hardware Performance Counter Events - profile based on hardware events accessed through PAPI. Lists of the available PAPI standard and native events can be found by running ```/opt/cray/pe/papi/5.6.0.4/bin/papi_avail and /opt/cray/pe/papi/5.6.0.4/bin/papi_native_avail``` 

  Setting the environment variable HPCRUN_PROCCESS_FRACTION will limit the number of process that write HPCToolkit output files to the specified fraction of processes. For runs using greater than 50-100 nodes it recommended that a fraction be set to limit the volume of output data.

HPCToolkit supports gathering of trace information, tracing can produce large volumes of information and it is recommended that it be performed only after profile data has been successfully gathered and reviewed. Tracing is off by default and may be enabled by adding the additional the environment variable:
 - HPCRUN_TRACE=1 (enabled)
  
A few notes on profiling events and options:
 - Use a space-separated list to sample multiple events in the same run.
  After execution, a directory ```hpctoolkit-myprogram-measurements-XXXXXX/``` will have been created containing the collected performance data.

### Analyze the Binary and Correlate with Performance Data
Before looking at the gathered performance data, it is also necessary to gather data from the executable by running the hpcstruct command on the executable used to gather the performance data. To do this, simply run the hpcstruct command followed by the program name. For example:
```
hpcstruct --loop-fwd-subst=no <executable>
```
This step can be carried out at any point after the executable is built with the hpclink command: before, after, or during the program execution. The hpcstruct command will produce the file myprogram.hpcstruct. Note that this step may take a little while depending on the size of the binary file.

Once the profile data has been gathered by running the program and the information from the executable has been gathered using the hpcstruct the two sets of data need to be correlated by running the hpcprof command. 

An example of this would be:
```
hpcprof -S myprogram.hpcstruct -I path-to-myprogram-src/+ \ 
hpctoolkit-myprogram-measurements-XXXXXX
```
If the source is in multiple files, the specification of the path to the source should be given in the form "path-to-myprogram-src/+". Multiple -I source paths can be specified.

If a large number of profile files have been generated (for instance, if the code runs on many cores), it is best to select only a subset of profile files to view. 

This can be done as:  
```
hpcprof -S myprogram.hpcstruct -I /home/mydir/myprogram-dir/+ \ 
hpctoolkit-myprogram-measurements-XXXXXX/{myprogram-000446-000-83cac10-546.hpcrun,myprogram-000328-000-83cac10-428.hpcrun}
```
  
where the brackets {} contain a list of individual profile files from specific MPI ranks. The hpcsummary can be used to see summary information about individual profile files to help you decide which ones to include.

Running the hpcprof command will produce a database hpctoolkit-myprogram-database containing all of the information a viewer needs.

### View the Performance Data
The performance data may be viewed using the hpcviewer command.
```
hpcviewer hpctoolkit-myprogram-database
```
The hpcviewer command may be run on the ALCF system with the display exported back to your local machine (login should be with ssh -X), or the hpcviewer program may be installed on your local machine (see HPCToolkit website) and the results database downloaded to your local machine and viewed locally.

If trace data was collected (with HPCRUN_TRACE=1), the program hpctraceviewer may be used to view the trace data:
```
hpctraceviewer
```

## Resources 
<iframe width="560" height="315" src="https://www.youtube.com/embed/Y1Ew51tZYOk" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<br/><br/>  
  
<iframe width="560" height="315" src="https://www.youtube.com/embed/uXRD3PfsaJE" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  
  

