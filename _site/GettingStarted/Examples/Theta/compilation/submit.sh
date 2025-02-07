#!/bin/bash -l
#COBALT -n 2 -t 30 -A Comp_Perf_Workshop -q comp_perf_workshop --attrs mcdram=cache:numa=quad --attrs filesystems=home,theta-fs0

echo "COBALT_JOBID = " $COBALT_JOBID
echo "COBALT_JOBSIZE (nodes) =" $COBALT_JOBSIZE
echo "COBALT_PARTNAME = " $COBALT_PARTNAME


rpn=8
depth=1

# option  long version            (explanation)
#
# -n                              "PEs" (ranks)
# -N      --pes-per-node          ranks per node
# -d      --cpus-per-pe           hyperthreads per rank
# -cc     --cpu-binding depth
# -j                              cpus (hyperthreads) per compute unit (core)


aprun -n $((COBALT_JOBSIZE*rpn)) -N $rpn -d $depth -j 1 -cc depth ./hellompi
status=$?


echo "Exit status of aprun is: $status"
exit $status

