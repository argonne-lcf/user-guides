# TAU
## Introduction
The TAU (Tuning and Analysis Utilities) Performance System is a portable profiling and tracing toolkit for performance analysis of parallel programs written in Fortran, C, C++, Java, and Python. TAU gathers performance information while a program executes through instrumentation of functions, methods, basic blocks, and statements. The instrumentation consists of calls to TAU library routines, which can be incorporated into a program in several ways:
- Automatic instrumentation of the code at the source level using the Program Database Toolkit (PDT)
- Automatic instrumentation of the code using the compiler
- Manual instrumentation using the instrumentation API
- At runtime using library call interception
- Dynamically using DyninstAPI
- At runtime in the Java virtual machine

For more information on TAU instrumentation options, see: [http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch01.html](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch01.html)

## References
- [TAU Project Site](http://www.cs.uoregon.edu/Research/tau/home.php)
- [TAU Instrumentation Methods](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch01.html)
- [TAU Compilation Options](http://www.cs.uoregon.edu/Research/tau/docs/newguide/re01.html)
- [TAU Fortran Instrumentation FAQ](http://www.nic.uoregon.edu/tau-wiki/FAQInstrumentingFortran)
- [TAU Performance Workshop 2018 Presentation](https://www.alcf.anl.gov/files/shende-TAU-ALCF-Workshop-May18.pdf)

## Compiling Your Application with TAU
While there are several methods of incorporating TAU instrumentation into a program, the two most common are automatic insertion (using the PDT source instrumentation method) and compiler instrumentation insertion. With either of these methods users must compile an application in a specific way so as to insert the TAU instrumentation that enables data collection. This involves invoking wrapper scripts that manage the compiling and linking process.

### Build Time Module
Start by loading the TAU module
```
module load tau
```
This will set add TAU scripts to your path and set the $TAU environment variable.

Two additional environment variables will also need to be set, but the settings will depend on the performance data to be collected and the method used for collection. Examples of settings for these values are:
```
TAU_MAKEFILE = $TAU/Makefile.tau-intel-mpi-pdt
TAU_OPTIONS = '-optVerbose -optNoRevert'
```
It is also possible to specify these parameters at the command line.
```
tau_cc.sh -tau_makefile=[makefile with complete path] tau_options=[options] sampleCprogram.c
```
The default method of instrumentation is to use PDT to insert calls into the source code. Alternatively, instrumentation may be inserted by the compiler, which requires adding -optCompInst to the TAU_OPTIONS setting. While PDT provides the widest array of instrumentation features, in some cases it may not be able to properly parse the source code and the instrumentation and will fail. In these cases, compiler instrumentation is the recommended alternative.

The TAU_MAKEFILE option largely determines what type of information TAU collects during program execution. This variable must specify the name of the TAU configuration file. Some of the options are:
```
Makefile.tau-gnu-papi-mpi-pdt
Makefile.tau-gnu-papi-mpi-pthread-pdt
Makefile.tau-gnu-papi-pthread-pdt
Makefile.tau-intel-19.0.3.199-papi-mpi-pthread-pdt
Makefile.tau-intel-19.0.3.199-papi-ompt-v5-mpi-pdt-openmp
Makefile.tau-intel-datascience_tensorflow_113-papi-mpi-pthread-python-pdt
Makefile.tau-intel-mpi-pdt
Makefile.tau-intel-papi-mpi-pdt
Makefile.tau-intel-papi-ompt-tr6-mpi-pdt-openmp
Makefile.tau-intel-papi-pdt
...
```
The Makefile extension is a descriptive name for the configuration. Selecting a makefile will involve making choices among the following:
- Compiler: For GNU, look for those containing ‘gnu.’ For Intel, look for those containing ‘intel’
- Parallelism: MPI is well supported. 
- PAPI: Interfaces with the PAPI hardware counters.
- Scalasca: Interfaces with Scalasca, [http://www.scalasca.org](http://www.scalasca.org). **Note:** Dynamic linking is not available (e.g., it will not work with Python).

### TAU_OPTIONS
The TAU_OPTIONS affect how TAU inserts the instrumentation and various options are [documented](http://www.cs.uoregon.edu/Research/tau/docs/newguide/re01.html). Here is a list of commonly used TAU_OPTIONS.

```
-optVerbose          self-explanatory
-optNoRevert         causes hard-failure when there is a TAU error, default behavior is to revert to an uninstrumented compile
-optKeepFiles        output your source file after it has been processed by the PDT parser
-optPreProcess       use if preprocess directives are present in Fortran code
-optPdtCOpts         pass special options to the PDT parse
-optCompInst         enables compiler-based instrumentation
-optTauSelectFile    enables selective instrumentation; cannot be used with compiler-based instrumentation
-optShared           linked against shared libraries, not recommended unless you know what you are doing
```
### Compiling
Once the TAU environment has been fully specified, an application may be compiled with TAU instrumentation by replacing the standard compiler names in the applications make file with the TAU compiler wrapper scripts:
```
ftn ->   tau_f90.sh (add -qfixed for Fortran77)
cc  ->   tau_cc.sh
CC  ->   tau_cxx.sh
```
These scripts will perform the instrumentation and compilation of the code by invoking TAU pre-processing tools and then invoke the compilers using the appropriate method. Successful compilation will produce a binary executable for the program. It is recommended to give your executable a unique name, perhaps by adding a .tau suffix, to distinguish it from a standard executable. Once a program is compiled with TAU instrumentation, the type and nature of the instrumentation is largely set (though some runtime variables exist that alter the instrumentation’s behavior). If you wish to collect a different type of performance data, you must re-compile the application with a different TAU_MAKEFILE.

### C Tips
If you receive the following type of error:
```
"foo.c", line 368: error: 
expected an expression for (int i = 0; i < n; i++)
```
append ```-optPdtCOpts=-c99 to TAU_OPTIONS```

### Fortran Tips
- Append -optPreProcess to TAU_OPTIONS if pre-process directives are present.
- Source-based instrumentation will not work for ENTRY points; a workaround is needed.
- Identify all relevant ENTRY points and exclude the parent function with a selective instrumentation file.
- Use compiler-based instrumentation instead.
- For Fortran77 codes, tau_f90.sh called the Fortran90 compiler. Thus, it will be necessary to add "-qfixed" to the Fortran compiler flags in your Makefile. If compilation fails with errors referencing syntax errors on lines that are comments, this indicates the use of "-qfixed." Comments using "C" in the first column are one instance in which "-qfixed" is required.

### Running with TAU
Once an application has been built with TAU instrumentation, it is not necessary to do anything special in order to run it. Simply execute the application as usual and TAU will collect data and write it to one or more files. However, it should be noted that in many cases TAU collects a large amount of performance data that have a significant impact on your application's wall-clock time. It is always a good idea to compare the wall-clock from an instrumented binary to that of the pristine (un-instrumented) binary. If you see a large number of function calls, chances are there will be significant overhead. 

Several runtime environment variables are available that can influence TAU's runtime behavior and limit the imposed overhead:
```
These environment variables are passed to TAU when you job is submitted with cobalt using the --env flag. 
() denotes default.

TAU_VERBOSE=(0) or 1        Stderr contains TAU debugging information. 
TAU_THROTTLE=0 or (1)       Attempts to reduce TAU overhead by turning off instrumentation for frequently called routines.
TAU_COMPENSATE=(0) or 1     Attempts to approximate and subtract out the instrumentation overhead from the reported metrics.
TAU_COMM_MATRIX=(0) or 1    Collects details information on point-to-point communication for MPI ranks.
TAU_TRACE=(0) or 1          Collects tracing information instead of profile information.
TAU_CALLPATH=(0) or 1       Generates a call path information for profiles. 
TAU_CALLPATH_DEPTH=N(2)     Where N is an positive integer.
TAU_PROFILE_FORMAT=merged   Will merge all data into a single file in snapshot format: tauprofile.xml. 
                               Recommended using more than 10,000 cores.
TAU_TRACK_HEAP=(0) or 1     Measures heap on function entry and exit.
TAU_TRACK_MESSAGE=(0) or 1  Collects detailed information about message sizes.
```
The simplest method for managing instrumentation overhead is by using the | ```TAU_THROTTLE``` [environment variable](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch02s02.html). ```TAU_THROTTLE = 0``` is needed for a full profile of your application. However, it is possible that you will incur a significant amount of overhead. If your application spends a significant percent of its runtime calling small routines repeatedly, e.g., 10 microsecond per call and more 1e5 calls, use either ```TAU_THROTTLE=0``` or selective instrumentation to have a flat profile with manageable overhead (<25%).

The ```AU_COMPENSATE``` setting will approximate the instrumentation overhead and subtract this from the metric reported. Check against timings from a pristine binary. For example, if the total exclusive time reported by TAU and the wall-clock time from the pristine binary are very different from the compensate option; it is not working effectively for your application.

```TAU_COMM_MATRIX``` collects and writes the communication matrix (columns, actually) for each rank. Different values of ```TAU_CALLPATH_DEPTH``` produce different types of information:

- 0 - communication matrix for application as a whole
- 1 - communication matrix broken down by function
- 2 - same as ```TAU_CALLPATH_DEPTH=1``` but also includes parent function

### Analyzing Your Data
The data collected by TAU will be written to one more file, by default to the application execution directory. This data may be viewed with the TAU command line tool pprof, or the GUI tools ParaProf and PerfExplorer. These tools may be run from the login nodes if you have an X-Windows environment on your local machine and X11 forwarding was set by logging in with "ssh –X." Alternatively, the GUI tools may be installed onto your local machine.

ParaProf is primarily for viewing a handful of profiles, while PerfExplorer is analyzing a larger collection of performance data (e.g., for weak scaling, strong scaling, etc.). PerfExplorer is highly recommended when large volumes of performance data are to be collected. In order to use PerfExplorer, first create a PerfDMF database.
```
perfdmf_configure --create-default
```
Go to the directory that contains your profiles. There will be a number of files named profile.x.0.0 where x is the MPI rank. The profile files are text files and it is convenient to pack into the native TAU format.
```
paraprof --pack filename.ppk
```
SCP filename.ppk to your desktop and open it up in the ParaProf viewer.

To analyze a trace file (from TAU_TRACE=1), see the Jumpshot instructions at [http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch04s03.html](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk01ch04s03.html)

## Advanced Users
### Selective Instrumentation
Selective instrumentation is enabled by appending -optTauSelectFile=<filename> to TAU_OPTIONS. Note that this is incompatible with the compiler-based instrumentation, e.g., -optCompInst. The syntax of the select instrumentation file can be found here:

[https://www.cs.uoregon.edu/research/tau/docs/newguide/bk01pt01ch01s03.html](https://www.cs.uoregon.edu/research/tau/docs/newguide/bk01pt01ch01s03.html)

It is also possible to generate a selective instrumentation file using Paraprof and a full flat profile obtained through automatic instrumentation.

[http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk03pt02ch16s03.html](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk03pt02ch16s03.html)

### Manual Instrumentation
If automatic instrumentation has too much overhead or too fine-grained details, an option to consider is performing light-weight/coarse-grained instrumentation by adding functional calls to the TAU timers directly in the source code. See the TAU API: 

[http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk03rn01.html](http://www.cs.uoregon.edu/Research/tau/docs/newguide/bk03rn01.html)

The most useful TAU API routines are those for starting and stopping timers. A text token needs to be provided for the timer:
```
Fortran:

CALL TAU_START('dgemm');
CALL TAU_STOP('dgemm');
C:
#include 
TAU_START("dgemm");
TAU_STOP("dgemm");
```
  
**Note:** External libraries such as BLAS or LAPACK are not automatically instrumented. This must be performed manually or a TAU wrapper library must be created:

[http://www.cs.uoregon.edu/Research/tau/docs/newguide/re32.html](http://www.cs.uoregon.edu/Research/tau/docs/newguide/re32.html)
  
## Resources
- [TAU Performance Analysis](https://www.alcf.anl.gov/sites/default/files/2019-07/Chaimov_TAU_Performance_Analysis_4.pdf)
  
<iframe width="560" height="315" src="https://www.youtube.com/embed/R2cHWMK91xI" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
