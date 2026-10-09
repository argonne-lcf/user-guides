---
description: "Build the nekRS spectral element CFD code with its SYCL backend on Aurora and run it with one MPI rank per GPU tile."
tags:
  - CFD
---

# nekRS

## Overview

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:overview"

For details about the code and its usage, see the [nekRS](https://github.com/Nek5000/nekRS/blob/master/README.md) home page. This page provides information specific to running on Aurora at the ALCF. For Polaris, see [nekRS on Polaris](../../../polaris/applications-and-libraries/applications/nekrs.md).

## Using nekRS at ALCF

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:using"

## How to Obtain the Code

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:obtain"

## Building on Aurora

nekRS uses CMake to build and install the software package. The `BuildMe.Aurora` script at the top level of the repository loads the required modules, then configures, builds, and installs nekRS with the SYCL (DPC++) backend:

```bash linenums="1"
./BuildMe.Aurora
```

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:build"

The script uses the following modules, which must also be loaded when running nekRS (see the job script below):

```bash linenums="1"
module load oneapi/release/2025.3.1
module load cmake
```

Since the [September 2026 system update](../../system-updates.md#major-update-2026-09), the default oneAPI module on Aurora is `oneapi/release/2026.1.0`; loading `oneapi/release/2025.3.1` swaps to the previous programming environment, which was rebuilt for the new OS image. nekRS builds and runs with either release. To build with a different oneAPI release, set `ONEAPI_SDK` when running the script, e.g. `ONEAPI_SDK=oneapi/release/2026.1.0 ./BuildMe.Aurora`, and set the same value of `ONEAPI_SDK` when running the job script below.

If the configuration step was successful, the `Summary` section of the CMake output shows the MPICH compiler wrappers (`mpicc`, `mpic++`, `mpif77`) and `Default backend : DPCPP`. After installation, set up the environment:

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:env"

/// warning | Rebuild after system software upgrades

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:conf"
Installations built before the [September 2026 system update](../../system-updates.md#major-update-2026-09) (new GPU drivers and programming environment) must be rebuilt from a clean build directory, as must any installation whose oneAPI module is later updated or removed. Also delete the `.cache` directory in each case directory; see [Just-in-time (JIT) compilation](#just-in-time-jit-compilation).
///

/// bug | Multi-rank runs crash during multigrid setup

With the GPU drivers installed in the September 2026 update, the current `v26` branch of `nekRS_alcf` aborts during the pressure multigrid setup of any run with more than one MPI rank, right after `BUILDING pMG` is printed:

```output
Segmentation fault from GPU at 0x84000, ctx_id: 1 (CCS) type: 0 (NotPresent), level: 3 (PML4), access: 0 (Read), banned: 1, aborting.
```

The Schwarz smoother is timed before its weights are allocated, so a kernel reads from a null device pointer. Until this is fixed in the repository, apply the following change to `src/core/elliptic/MG/ellipticMultiGridSchwarz.cpp` before building (or rebuild afterward), moving `generateSchwarzWeights()` ahead of `autoOverlap()` near the end of `pMGLevel::setupSmootherSchwarz`:

```diff
-  autoOverlap();
-
-  free(maskedGlobalIdsExt);
-  meshFree(extendedMesh);
-
-  generateSchwarzWeights();
+  // weights (o_wts) are used by smoothSchwarz, which autoOverlap calls
+  generateSchwarzWeights();
+
+  autoOverlap();
+
+  free(maskedGlobalIdsExt);
+  meshFree(extendedMesh);
 }
```
///

## Running Jobs on Aurora

An example submission script for running nekRS on Aurora is shown below. It launches 12 MPI ranks per node, one per GPU tile, and binds each rank to 8 physical cores, skipping cores 0 and 52, which are [reserved for system services](../../running-jobs-aurora.md#mpi-rank-and-thread-binding-to-cores-and-gpus). Additional information on nekRS input files and application setup options is described [here](https://github.com/Nek5000/nekRS/blob/master/doc/parHelp.txt).
The correct options to execute the script are as follows:

```bash
NEKRS_HOME=</path/to/nekrs/install> PROJ_ID=<project_id> QUEUE=<queue> ./run.sh <casename> <number_of_nodes> <walltime_hh:mm:ss>
```

Users can copy the script below into a file `run.sh`, make it executable with `chmod +x run.sh`, and execute it using the command above.

```bash linenums="1" title="run.sh"
#!/bin/bash
: ${PROJ_ID:=""}

: ${QUEUE:="prod"} # debug, debug-scaling, prod
: ${NEKRS_HOME:="$HOME/.local/nekrs"}
: ${ONEAPI_SDK:="oneapi/release/2025.3.1"}
: ${NEKRS_SKIP_BUILD_ONLY:=0}

if [ $# -ne 3 ]; then
  echo "usage: [PROJ_ID] [QUEUE] $0 <casename> <number of compute nodes> <hh:mm:ss>"
  exit 0
fi

if [ -z "$PROJ_ID" ]; then
  echo "ERROR: PROJ_ID is empty"
  exit 1
fi

if [ -z "$QUEUE" ]; then
  echo "ERROR: QUEUE is empty"
  exit 1
fi

bin=${NEKRS_HOME}/bin/nekrs
case=$1
nodes=$2
time=$3
gpus_per_node=6
tiles_per_gpu=2
let ranks_per_node=$gpus_per_node*$tiles_per_gpu
let nn=$nodes*$ranks_per_node

# one list entry (8 physical cores) per rank; cores 0 and 52 are reserved
cpu_bind_list="1-8:9-16:17-24:25-32:33-40:41-48:53-60:61-68:69-76:77-84:85-92:93-100"

backend=DPCPP
NEKRS_GPU_MPI=0

if [ ! -f $bin ]; then
  echo "Cannot find" $bin
  exit 1
fi

if [ ! -f $case.par ]; then
  echo "Cannot find" $case.par
  exit 1
fi

if [ ! -f $case.udf ]; then
  echo "Cannot find" $case.udf
  exit 1
fi

if [ ! -f $case.re2 ]; then
  echo "Cannot find" $case.re2
  exit 1
fi

# qsub
SFILE=s.bin
echo "#!/bin/bash" > $SFILE
echo "#PBS -A $PROJ_ID" >>$SFILE
echo "#PBS -N nekRS_$case" >>$SFILE
echo "#PBS -q $QUEUE" >>$SFILE
echo "#PBS -l walltime=$time" >>$SFILE
echo "#PBS -l filesystems=home:flare" >>$SFILE
echo "#PBS -l select=$nodes" >>$SFILE
echo "#PBS -l place=scatter" >>$SFILE
echo "#PBS -k doe" >>$SFILE #write directly to the destination, doe=direct, output, error
echo "#PBS -j oe" >>$SFILE  #oe=merge stdout/stderr to stdout

# job to “run” from your submission directory
echo "cd \$PBS_O_WORKDIR" >> $SFILE

echo "module load $ONEAPI_SDK" >> $SFILE
echo "module load cmake" >> $SFILE
echo "module list" >> $SFILE

echo "ulimit -s unlimited " >>$SFILE

echo "export NEKRS_HOME=$NEKRS_HOME" >>$SFILE
echo "export NEKRS_GPU_MPI=$NEKRS_GPU_MPI" >>$SFILE

# required by parRSB
echo "export FI_CXI_RX_MATCH_MODE=hybrid" >> $SFILE

# bind each rank to one GPU tile
CMD=.lhelper
rm -f $CMD # avoid "Text file busy" if a previous job still has it open
echo "#!/bin/bash" >$CMD
echo "gpu_id=\$(((PALS_LOCAL_RANKID / ${tiles_per_gpu}) % ${gpus_per_node}))" >>$CMD
echo "tile_id=\$((PALS_LOCAL_RANKID % ${tiles_per_gpu}))" >>$CMD
echo "export ZE_AFFINITY_MASK=\$gpu_id.\$tile_id" >>$CMD
echo "\"\$@\"" >>$CMD
chmod 755 $CMD

# precompile kernels on a single node before the run
if [ $NEKRS_SKIP_BUILD_ONLY -eq 0 ]; then
echo "mpiexec -n $ranks_per_node -ppn $ranks_per_node --cpu-bind=list:$cpu_bind_list ./$CMD $bin --setup ${case} --backend ${backend} --device-id 0 --build-only $nn" >>$SFILE
fi

echo "mpiexec -n $nn -ppn $ranks_per_node --cpu-bind=list:$cpu_bind_list ./$CMD $bin --setup ${case} --backend ${backend} --device-id 0" >>$SFILE

qsub $SFILE

# clean-up
#rm -rf $SFILE .lhelper

```

## Just-in-time (JIT) compilation

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:jit"

## Discussion Group

--8<-- "./docs/polaris/applications-and-libraries/applications/nekrs.md:discussion"
