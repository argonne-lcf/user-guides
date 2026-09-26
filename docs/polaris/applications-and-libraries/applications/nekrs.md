# nekRS

## Overview

nekRS is a fast and scalable computational fluid dynamics (CFD) software package targeting massively parallel computers. It is based on the high-order spectral element method and is capable of solving incompressible and low Mach-number fluid flow problems. 
nekRS uses the [OCCA](https://github.com/libocca/occa) portability layer for offloading compute kernels to GPU devices.

For details about the code and its usage, see the [nekRS](https://github.com/Nek5000/nekRS/blob/master/README.md) home page. This page provides information specific to running on Polaris at the ALCF.

## Using nekRS at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). For questions, contact us at <support@alcf.anl.gov>.

## How to Obtain the Code

nekRS is an open-source code developed in the [Nek5000/nekRS](https://github.com/Nek5000/nekRS) GitHub repository. ALCF maintains a version of nekRS with build scripts for ALCF systems in the [argonne-cps/nekRS_alcf](https://github.com/argonne-cps/nekRS_alcf) repository, and the instructions on this page are based on the `v26` branch of that repository:

```bash linenums="1"
git clone https://github.com/argonne-cps/nekRS_alcf.git
cd nekRS_alcf
git checkout v26
```

Users who need a different version of nekRS can contact <support@alcf.anl.gov> for assistance.

## Building on Polaris

nekRS uses CMake to build and install the software package. The `BuildMe.Polaris` script at the top level of the repository loads the required modules, then configures, builds, and installs nekRS:

```bash linenums="1"
./BuildMe.Polaris
```

The script builds in a directory named `RBK_built.on.<date>` inside the repository (with a symbolic link `current` pointing to it) and installs nekRS to `.local/nekrs` in the directory **one level above** the repository. For example, if the repository was cloned into `$HOME/nekRS_alcf`, nekRS is installed in `$HOME/.local/nekrs`. Edit `NEKRS_HOME` in the script to install elsewhere.

!!! tip "Build on a compute node"

    The build (hypre, OCCA, Nek5000, and nekRS) is sizable. The per-user limits on the login nodes can cause the build to fail or run slowly, so we recommend building within an interactive job on a compute node, for example in the `debug` queue.

The script uses the following modules, which must also be loaded when running nekRS (see the job script below):

```bash linenums="1"
module restore
module use /soft/modulefiles
module swap PrgEnv-nvidia PrgEnv-gnu
module unload darshan
module load cuda/13.0
module load craype-x86-milan craype-accel-nvidia80
module load spack-pe-base cmake
```

The `cuda/13.0` module must be loaded before `craype-accel-nvidia80`; the latter does not load without it, and the `cudatoolkit-standalone` modules do not satisfy this requirement. `craype-accel-nvidia80` is needed for the compiler wrappers (`cc`, `CC`, `ftn`) to link the Cray MPICH GPU Transport Layer (GTL) library used for GPU-aware MPI.

If the configuration step was successful, the `Summary` section of the CMake output shows the Cray compiler wrappers and `Default backend : CUDA`. After installation, set up the environment:

```bash linenums="1"
export NEKRS_HOME=/path/to/installation/directory
export PATH=$NEKRS_HOME/bin:$PATH
```

Alternatively, you may add the above lines to your `$HOME/.bashrc` and type `source $HOME/.bashrc` in the current terminal window.

!!! warning "Rebuild after system software upgrades"

    nekRS records the compilers and flags used at build time in `$NEKRS_HOME/nekrs.conf` and reuses them to compile kernels and case files at run time. Installations built before the August 2026 Polaris upgrade (which removed `gcc-native/13.2` and older Cray PE releases) will not work and must be rebuilt from a clean build directory. Also delete the `.cache` directory in each case directory; see [Just-in-time (JIT) compilation](#just-in-time-jit-compilation).

## Running Jobs on Polaris

An example submission script for running a 2-node nekRS job is shown below as an example. Additional information on nekRS input files and application setup options is described [here](https://github.com/Nek5000/nekRS/blob/master/doc/parHelp.txt).
The correct options to execute the script are as follows:

> NEKRS_HOME= *</path/to/nekrs/install>* PROJ_ID=*<Your_Project_ID>* QUEUE=*<queue_to_submit>* ./run.sh *<casename>* *<number_of_nodes_requested>* *<walltime(hh:mm:ss)>*

Users can copy the script below into a file `run.sh` and execute it using the command above.
```bash linenums="1" title="run.sh"
#!/bin/bash
: ${PROJ_ID:=""}

: ${QUEUE:="prod"} # debug, debug-scaling, prod
: ${NEKRS_HOME:="$HOME/.local/nekrs"}
: ${NEKRS_CACHE_BCAST:=1}
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
gpu_per_node=4
cores_per_numa=8
let nn=$nodes*$gpu_per_node
let ntasks=nn
time=$3

backend=CUDA
NEKRS_GPU_MPI=1

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

striping_unit=16777216
max_striping_factor=128
let striping_factor=$nodes/2
if [ $striping_factor -gt $max_striping_factor ]; then
  striping_factor=$max_striping_factor
fi
if [ $striping_factor -lt 1 ]; then
  striping_factor=1
fi

MPICH_MPIIO_HINTS="*:striping_unit=${striping_unit}:striping_factor=${striping_factor}:romio_cb_write=enable:romio_ds_write=disable:romio_no_indep_rw=true"

# sbatch
SFILE=s.bin
echo "#!/bin/bash" > $SFILE
echo "#PBS -A $PROJ_ID" >>$SFILE
echo "#PBS -N nekRS_$case" >>$SFILE
echo "#PBS -q $QUEUE" >>$SFILE
echo "#PBS -l walltime=$time" >>$SFILE
echo "#PBS -l filesystems=home:eagle" >>$SFILE
echo "#PBS -l select=$nodes:system=polaris" >>$SFILE
echo "#PBS -l place=scatter" >>$SFILE
echo "#PBS -k doe" >>$SFILE #write directly to the destination, doe=direct, output, error
echo "#PBS -j eo" >>$SFILE  #oe=merge stdout/stderr to stdout

# job to “run” from your submission directory
echo "cd \$PBS_O_WORKDIR" >> $SFILE

echo "module restore" >> $SFILE
echo "module use /soft/modulefiles" >> $SFILE
echo "module swap PrgEnv-nvidia PrgEnv-gnu" >> $SFILE
echo "module unload darshan" >> $SFILE
echo "module load cuda/13.0" >> $SFILE
echo "module load craype-x86-milan craype-accel-nvidia80" >> $SFILE
echo "module load spack-pe-base cmake" >> $SFILE
echo "module list" >> $SFILE

echo "nvidia-smi" >> $SFILE
echo "ulimit -s unlimited " >>$SFILE

echo "export NEKRS_HOME=$NEKRS_HOME" >>$SFILE
echo "export NEKRS_GPU_MPI=$NEKRS_GPU_MPI" >>$SFILE

echo "export MPICH_MPIIO_HINTS=$MPICH_MPIIO_HINTS" >>$SFILE
echo "export MPICH_MPIIO_STATS=1" >>$SFILE

echo "export NEKRS_CACHE_BCAST=$NEKRS_CACHE_BCAST" >>$SFILE
echo "export NEKRS_LOCAL_TMP_DIR=/local/scratch" >>$SFILE

echo "export MPICH_GPU_SUPPORT_ENABLED=1" >> $SFILE
echo "export MPICH_OFI_NIC_POLICY=NUMA" >> $SFILE

# https://github.com/Nek5000/Nek5000/issues/759
echo "export FI_OFI_RXM_RX_SIZE=32768" >> $SFILE # >=lelt, large mpi-messsage for restart

if [ $NEKRS_SKIP_BUILD_ONLY -eq 0 ]; then
echo "mpiexec -n 1 $bin --setup ${case} --backend ${backend} --device-id 0 --build-only $nn" >>$SFILE
fi

CMD=.lhelper
echo "#!/bin/bash" >$CMD
echo "gpu_id=\$((${gpu_per_node} - 1 - \${PMI_LOCAL_RANK} % ${gpu_per_node}))" >>$CMD
echo "export CUDA_VISIBLE_DEVICES=\$gpu_id" >>$CMD
echo "$bin --setup ${case} --backend ${backend} --device-id 0" >>$CMD
chmod 755 $CMD

echo "mpiexec -n $nn -ppn $gpu_per_node -d $cores_per_numa --cpu-bind depth ./$CMD" >>$SFILE

qsub -q $QUEUE $SFILE

# clean-up
#rm -rf $SFILE $ROMIO_HINTS .lhelper

```

## Just-in-time (JIT) compilation
nekRS uses the OCCA library to translate, compile, and run GPU-targeted functions and kernels. Some useful notes on the cached object files can be found [here](https://nekrsdoc.readthedocs.io/en/latest/just_in_time_compilation.html).

The compiled kernels and case (`.udf`, `.usr`) objects are cached in the `.cache` directory of the case directory by default. After rebuilding or reinstalling nekRS, delete `.cache` so that stale objects built with the previous installation are not reused.

## Discussion Group
Users can visit the [GitHub Discussions](https://github.com/Nek5000/nekRS/discussions) page to seek help, find solutions, share ideas, and follow discussions on several application-specific topics.
