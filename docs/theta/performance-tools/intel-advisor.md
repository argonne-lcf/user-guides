# Intel Advisor
## Introduction
Advisor is an advanced profiling tool which helps you to optimize your code on KNL architecture. It allows you to analyze the vectorization efficiency of your code and compares your code’s performance to the theoretical limits of the hardware. See [1] for more details on the roofline model.

## Step-by-step guide
  1. Build your target application with all optimizations enabled e.g. ```-O3 -xMIC-AVX512``` and ```set debugging -g flag```.
  2. Submit your job using sample batch script:

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

# Setup Intel Advisor
module swap intel/18.0.0.128 intel/19.0.3.199

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
RESDIR=" advixe_${BINNAME}_${INPUTNAME}_${T} "

# Set job size and run. Line # 68 to profile rank 0, Line # 69 to profile all ranks
aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node \
      -cc depth \
      -d $n_hyperthreads_skipped_between_ranks \
      -j $n_hyperthreads_per_core \
      ./profile1.sh "advixe-cl -c survey --project-dir ${RESDIR} ${SEARCH}" "${BIN} ${INPUT}"
      #advixe-cl -c survey --project-dir ${RESDIR} ${SEARCH} -- ${BIN} ${INPUT}
aprun -n $n_mpi_ranks -N $n_mpi_ranks_per_node \
      -cc depth \
      -d $n_hyperthreads_skipped_between_ranks \
      -j $n_hyperthreads_per_core \
      ./profile1.sh "advixe-cl -c tripcounts -flop --project-dir ${RESDIR} ${SEARCH}" "${BIN} ${INPUT}"
      #advixe-cl -c tripcounts -flops --project-dir ${RESDIR} ${SEARCH} -- ${BIN} ${INPUT}

# Save the cobalt files along with result 
mv $COBALT_JOBID.* $RESDIR

rm ./profile1.sh
```
  3. The survey results will be located in <result_dir> after the Advisor run has been completed. <br />
  4. Connect to Theta with X-11 forwarding enabled and launch the Advisor GUI with advixe-gui to view your results. <br />
  5. Click “Show My Result” and the survey data will load. <br />
  6. There is a plethora of information here on how to analyze survey data.

## Additional collections:
There are three other types of collections that can be performed with Advisor for more advanced analysis: tripcounts, memory access pattern, and dependencies. This data can be collected by changing survey to tripcounts, map, and dependencies. The roofline data can be obtained from collecting the survey and tripcounts data back-to-back or by changing survey to roofline.

## Additional Information:
There are many command line options. See [2] for more details on all of the options, and its more comprehensive user guide also available on Intel’s website.
- [1] Williams, Samuel, Andrew Waterman, and David Patterson. "Roofline: an insightful visual performance model for multicore architectures." Communications of the ACM 52.4 (2009): 65-76.
- [2] Intel. “Get Started with Intel Advisor.” Intel® Software, Intel, 18 Oct. 2018, [software.intel.com/en-us/get-started-with-advisor-for-more-information](software.intel.com/en-us/get-started-with-advisor-for-more-information)

<iframe width="560" height="315" src="https://www.youtube.com/embed/nCEBrkNLUHI" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
