# nekRS

## Overview

nekRS is a fast and scalable computational fluid dynamics (CFD) software package targeting massively parallel computers. It is based on the high-order spectral element method and is capable of solving incompressible and low Mach-number fluid flow problems. 
nekRS uses the [OCCA](https://github.com/libocca/occa) portability layer for offloading compute kernels to GPU devices.

For details about the code and its usage, see the [nekRS](https://github.com/Nek5000/nekRS/blob/master/README.md) home page. This page provides information specific to running on Polaris at the ALCF.

## Using nekRS at ALCF

ALCF provides assistance with build instructions, compiling executables, submitting jobs, and providing prebuilt binaries (upon request). For questions, contact us at <support@alcf.anl.gov>.

## How to Obtain the Code

nekRS is an open-source code and can be downloaded from the [website](https://github.com/Nek5000/nekRS/archive/refs/tags/v24.0.tar.gz). Alternatively, the user can clone from the [nekRS GitHub](https://github.com/Nek5000/nekRS/tree/master) repository. We recommend using the [next](https://github.com/Nek5000/nekRS/tree/next) branch since it is the most updated branch with some of the latest features, including the in-situ visualization capability. 

```bash linenums="1"
git clone https://github.com/Nek5000/nekRS.git
git checkout next
```
The rest of this documentation is based on building and running using the `next` branch. Users who are interested in running the default `master` branch can contact <support@alcf.anl.gov> for additional support. 

## Building on Polaris 

nekRS uses CMake to build and install the software package. After nekRS has been downloaded or cloned on an ALCF filesystem, users should see a directory with the name `nekRS`. Inside this directory, the user will find a file named `nrsconfig` that can be used to configure and customize the CMake build options.
The user should remove previous build and installation directories whenever there is an update.

### Building using GNU compilers
The following modules are to be loaded for this particular build.
```bash linenums="1"
module restore
module load craype-accel-nvidia80
module swap PrgEnv-nvhpc PrgEnv-gnu

module use /soft/modulefiles
module load cudatoolkit-standalone/12.4.0

module load spack-pe-base cmake
```

To build and install the code, run:
```bash linenums="1"
CC=cc CXX=CC FC=ftn ./build.sh -DCMAKE_INSTALL_PREFIX=/path/to/installation/directory
```
During the installation process, you will be prompted to verify the configuration options. If everything was done correctly, you should see the correct compilers and the `Default backend : CUDA` in the `Summary` section of the output. If you see this, press `Enter` to continue with the build and installation process.

After installation, execute the following commands to set up the environment. 
```bash linenums="1"
export NEKRS_HOME=/path/to/installation/directory
export PATH=$NEKRS_HOME/bin:$PATH
```

Alternatively, you may add the above lines to your `$HOME/.bashrc` and type `source $HOME/.bashrc` in the current terminal window.

### Building using NVHPC compilers

The following modules are to be loaded for this particular build. The initial `module restore` is just setting the default environment as the starting point.
```bash linenums="1"
module restore
module load craype-accel-nvidia80

module use /soft/modulefiles
module load spack-pe-base cmake
```

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

echo "module use /soft/modulefiles" >> $SFILE
echo "module use /opt/cray/pe/lmod/modulefiles/mix_compilers" >> $SFILE
echo "module load libfabric" >> $SFILE
echo "module load cpe-cuda" >> $SFILE
echo "module load PrgEnv-gnu" >> $SFILE
echo "module load nvhpc-mixed" >> $SFILE
echo "module load cmake" >> $SFILE
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

## Discussion Group
Users can visit the [GitHub Discussions](https://github.com/Nek5000/nekRS/discussions) page to seek help, find solutions, share ideas, and follow discussions on several application-specific topics.
