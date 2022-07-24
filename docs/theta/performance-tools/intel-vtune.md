# Intel Vtune
VTune is an advanced profiling tool which helps you to optimize your code for various architectures. It allows you to track how well your code is threaded and vectorized to take advantage of multiple CPUs/FPUs and how well the code is utilizing the non-uniform memory architecture and caches.

VTune is a core and node-level profiler. In general, it is not a good to try to profile many ranks. As such this tutorial show you how to setup and run profiling on a single or a small number of ranks. 

## Step-by-step guide
  1. Build your target application with all optimizations enabled e.g. -O3 -xMIC-AVX512 and enable debug symbols and dynamic linking: -g -dynamic 
  **Note:** -dynamic enables dynamic linking. Required for SW sampling, optionalfor HW sampling, but always recommended.  
  2. Copy and modify sample batch script ```/soft/perftools/intel/vtune/amplxe.qsub```

You need to at least change the project name to your allocation and add any environment variables specific to your application. After that you can submit your job as follows:

```qsub ./amplxe.qsub ./myBinary ./inputfile```

```
#!/bin/bash
#COBALT -t 60
#COBALT -n 1
#COBALT -A Intel
#COBALT -q debug-flat-quad
# COBALT -q debug-cache-quad

# Job Size
export n_nodes=$COBALT_JOBSIZE
export n_mpi_ranks_per_node=1
export n_mpi_ranks=$(($n_nodes * $n_mpi_ranks_per_node))
export n_openmp_threads_per_rank=1
export n_hyperthreads_per_core=1
export n_hyperthreads_skipped_between_ranks=1


# Make a temp external wrapper
echo "#!/bin/bash" >> profile1.sh
echo "export PE_RANK=\$ALPS_APP_PE" >> profile1.sh
echo "export PMI_NO_FORK=1" >> profile1.sh
# if you want to profile a rank other than 0 change here
echo "if [ "\$PE_RANK" == 0 ];then" >> profile1.sh
echo "\$1 -- \$2" >> profile1.sh
echo "else" >> profile1.sh
echo "\$2" >> profile1.sh
echo "fi" >> profile1.sh
echo "" >> profile1.sh
chmod 744 ./profile1.sh

# OpenMP Settings
export OMP_NUM_THREADS=$n_openmp_threads_per_rank
export OMP_AFFINITY=compact,granularity=core
export OMP_STACKSIZE=16G
# Big stacks to prevent segfaults and disable DARSHAN IO profiling
ulimit -s unlimited
export DARSHAN_DISABLE=1
export PMI_NO_FORK=1

# Setup Intel Vtune
module load vtune

# Binary name and directory to be used in naming result dir
BIN=$1
BINDIR=$(dirname $(realpath $1))
BINNAME=$(basename $BIN)
echo "using $BINNAME  at $BIN"
# If 2nd argument is provided, use it to better idenfity result
if [ $# -eq 2 ]; then
    INPUT=$2
    INPUTNAME=$(basename $2)
    echo "with $INPUTNAME at $BIN"
fi

# Time stamp down to the minute - useful when binary names don't change
T=$(date +%F-%H-%M)

# Modify/Add Additional search paths to locate sources
SEARCH=" --search-dir src:=${BINDIR} "
SEARCH+=" --search-dir bin:=${BINDIR} "
RESDIR=" amplxe_${BINNAME}_${INPUTNAME}_${T} "

# Set job size and run. Line # 68 to profile rank 0, Line # 69 to profile all ranks
aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node \
      -cc depth \
      -d $n_hyperthreads_skipped_between_ranks \
      -j $n_hyperthreads_per_core \
      ./profile1.sh "amplxe-cl -c hotspots -r ${RESDIR} ${SEARCH}" $@
      #amplxe-cl -c hotspots -r ${RESDIR} ${SEARCH} -- $@

# Save the cobalt files along with result 
mv $COBALT_JOBID.* $RESDIR

rm ./profile1.sh
```

**Note:** Make sure to add source and binary search direction. Inclusion of these will allow you to view the sources files in the amplxe-gui, in addition to providing execution time costs line-by-line. IMPORTANT: Use the “realpath” command to specify these, as sometimes Vtune sees the realbath and not the user alias. 

Example:
```
$> pwd
/home/pvelesko/projects/distress
$> realpath `pwd`
/lus/theta-fs0/projects/intel/pvelesko/distress
```
There are many command line options such as the following (amplxe-cl -h collect).
  - uarch-exploration - Microarchitecture Exploration
  - hpc-performance - HPC Performance Characterization
  - io - Input and Output

Results will be collected in a directory called amplxe_EXE_TIMESTAMP. It is recommended to add the –no-auto-finalize flag to collections that will be creating large results. The finalization step is compute intensive and runs serially which may take a long time on the KNL. Finalization can be done on another machine after copying the results off the KNL. The data collected may be very large for longer runs with many threads active. If you find that you are reaching the data limit, use the flag -data-limit=<integer>. The default limit is 500MB. The integer specifies the size in MB. Use –data-limit=0 for no limit.

  - The script above is setup for profile only rank 0
  - You can modify the login on line #19 to include additional ranks.  
  - If you wish to profile all ranks, use line # 69 instead of line # 68

 After step 2 has been completed, i.e. a results file has been created you can conveniently finalize the results by doing the following:  
```amplxe-cl -finalize -r <vtune-result-dir> -search-dir ./```
  
  - The finalized results can be examined in either the GUI or the command line interface.
  - To examine the results using the GUI interface do the following:
  - Copy the results directory to a machine of your choice (on which you have already installed the VTune GUI)
  - Launch the GUI
  - Click on the “Open Result” link.While the GUI is very convenient, the command line interface provides a quick way to generate reports directly on Theta.
  ```amplxe-cl -report <report-type> -r <vtune-result-dir> [report-options]```
  
### Command line options and Help system

After setting up the VTune environment variables you can view the set of options using 'amplxe-cl -h' The available actions are:
  - collect
  - collect-with
  - command
  - finalize
  - help
  - import
  - report
  - version
  
Type ```'amplxe-cl -help <action>'``` for help on a specific action.

Full CLI reference: https://software.intel.com/en-us/vtune-amplifier-help-amplxe-cl-command-syntax
  
## Analysis types in VTune
Intel® VTune Analyzer supports two types of profiling: Time Based profiling (SW) and Event Based profiling (HW). Time based profiling utilizes a system clock and reports how time is spent in various parts of a program. This is the “traditional” method of profiling. Event Based profiling utilizes hardware counters to count the number of events generated by various parts of a program. Events one may want to track are, for example, cache hits and cache misses at various levels of cache.
  
VTune organizes its various analysis types into templates. The templates “Concurrency” and “Locks and Waits” are time-based analyses. Hotspots is SW based by default but can use HW sampling using -knob sampling-mode=hw switch. The rest of the templates are Event Based analyses.  The most common event-based analysis one can run is called “uarch-exploration”. This analysis tracks most of the available HW counters. Because only a finite number of hardware counters can be collected, Vtune uses time multiplexing and as such, the job should run for over 5 minutes or so for the results to be accurate. Users new to Event Based analysis can start with “hotspots -knob sampling-mode=hw” or jump right into “uarch-exploration. A short description is provided below. For more details the user should consult the documentation system built into the tool. All VTune documentation is also available online: https://software.intel.com/en-us/vtune-amplifier-help.

### Hotspots HW analysis
The Hotspots HW Analysis will show where your application is spending its time, including information related to OpenMP parallelism. Use the Bottom-up view in the GUI to see time spent at various granularities; for example, Function or Module granularities. This can be changed in the Grouping drop-down menu. Focus tuning efforts on the hot portions of your application.

### uArch Exploration
Most useful for identifying sections of code with high CPI (cycles per instruction) and gathering L1 and L2 cache hit/miss ratios. KNL supports 512-bit vector instructions. To optimize for KNL, an application should take advantage of these large vector units with heavily vectorized code. Look at the metric VPU Utilization to determine the areas of high and low vectorization. The VPU Utilization metric is also available in the Bottom-up view of the General Exploration viewpoint. Locate hotspots with low VPU Utilization and try to improve their usage of the AVX512 capabilities.
  
  
<iframe width="560" height="315" src="https://www.youtube.com/embed/Dt8-wKLFCRw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
  
