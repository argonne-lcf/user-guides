# NAMD on Polaris

## What is NAMD?

NAMD, recipient of a 2002 Gordon Bell Award, a 2012 Sidney Fernbach Award, and a 2020 Gordon Bell Prize, is a parallel molecular dynamics code designed for high-performance simulation of large biomolecular systems. Based on Charm++ parallel objects, NAMD scales to hundreds of cores for typical simulations and beyond 50,000 cores for the largest simulations. NAMD uses the popular molecular graphics program VMD for simulation setup and trajectory analysis, but is also file-compatible with AMBER, CHARMM, and X-PLOR.

## Using NAMD at ALCF

ALCF offers assistance with building binaries and compiling instructions for NAMD. For questions, contact us at [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Running NAMD on Polaris

Prebuilt releases of NAMD binaries can be found in the directory `/soft/applications/namd`. 
* `Linux-x86_64-netlrts-smp-CUDA` supports GPU-resident runs. 
* `Linux-x86_64-ofi-smp-CUDA` supports general GPU-offload runs.
* `Linux-x86_64-ofi-smp-CUDA-memopt` supports memory-optimized input files and parallel I/O for the largest simulations (~100 million atoms or more).

NAMD supports two types of parallelized simulations: single instance strong-scaling and multiple-copy weak-scaling (i.e., replica exchange). For more functionality details, please visit the NAMD [website](https://tcbg.illinois.edu/Research/namd).

### GPU-resident runs

A sample PBS script follows for GPU-resident runs on Polaris.

```bash
#!/bin/sh -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A PROJECT
#PBS -l filesystems=home:eagle

EXE=/PATH-TO/namd3

cd ${PBS_O_WORKDIR}

mpiexec -n 1 --ppn 1 --depth=16 --cpu-bind=depth $EXE +p 15 +devices 3,2,1,0 stmv.namd > stmv.output
```

Measured performance for a ~1,000,000 atom system generated with the above submission script run under NPT conditions and a timestep of 2 fs was `16 CPUs 0.00381933 s/step 45.2435 ns/day`.

Note, the GPU-resident version only runs on a single node currently, and some important functions remain to be implemented with it. A user is strongly encouraged to ensure the updated GPU-resident version fully supports the planned simulation in advance.

### Multiple-copy GPU-resident runs

A sample PBS script for multiple-copy GPU-resident runs follows.

```bash
#!/bin/sh -l
#PBS -l select=4:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling
#PBS -A PROJECT
#PBS -l filesystems=home:eagle

EXE=/PATH-TO/namd3
CHARMRUN=/PATH-TO/charmrum

cd ${PBS_O_WORKDIR}

$CHARMRUN ++mpiexec ++np 16 ++ppn 8 $EXE +replicas 16 init.conf --source rest2_remd.namd +pemap 0-31 +setcpuaffinity +devices 0,1,2,3 +stdout rest2_output/%d/job0.%d.log +devicesperreplica 1
```

This sample script launches a solute-tempering replica-exchange simulation with 16 replicas on 4 Polaris nodes. Each node accommodates 4 replicas, and each replica uses 8 CPU cores and binds to 1 GPU device.

### GPU-offload run on multiple nodes

A sample PBS script for a GPU-offload run follows.

```bash
#!/bin/sh -l
#PBS -l select=64:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q prod
#PBS -A PROJECT
#PBS -l filesystems=home:eagle

EXE=/PATH-TO/namd3

cd ${PBS_O_WORKDIR}

aprun -N 4 -n 256 --cc core --cpus-per-pe 8 $EXE +p 6 +setcpuaffinity +devices 3,2,1,0 stmv.namd > stmv_64nodes.output
```

Measured performance for a ~1,000,000 atom system generated with the above submission script run under NPT conditions and a timestep of 2 fs was `1536 CPUs 0.00151797 s/step 113.724 ns/day`.

### Multiple-copy GPU-offload run

A sample PBS script for multiple-copy GPU-offload runs follows.

```bash
#!/bin/sh -l
#PBS -l select=4:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug-scaling
#PBS -A PROJECT
#PBS -l filesystems=home:eagle

EXE=/PATH-TO/namd3

cd ${PBS_O_WORKDIR}

aprun -N 4 -n 16 --cc=core --cpus-per-pe 8 $EXE +replicas 16 init.conf --source rest2_remd.namd +setcpuaffinity +stdout rest2_output/%d/job0.%d.log +devicesperreplica 1
```

## Building NAMD

We recommend using the provided NAMD binaries.

The following instructions are for building the GPU-offload version on top of a Slingshot-11-optimized Charm++ library:

1. `module swap PrgEnv-nvhpc PrgEnv-gnu`
2. Download NAMD source [code](https://www.ks.uiuc.edu/Development/Download/download.cgi?PackageName=NAMD)
```bash title="build_namd.sh"
tar -xzf NAMD_3.0_Source.tar.gz
cd NAMD_3.0_Source
tar xvf charm-8.0.0.tar
cd charm-8.0.0
./buildold charm++ ofi-crayshasta cxi slurmpmi2cray smp --with-production -j8 -DCMK_OBJIC_COLLECTION_BITS=8 -DCMK_OBJID_HOME_BITS=20
cd ..
wget http://www.ks.uiuc.edu/Research/namd/libraries/fftw-linux-x86_64.tar.gz
wget http://www.ks.uiuc.edu/Research/namd/libraries/tcl8.6.13-linux-x86_64.tar.gz
wget http://www.ks.uiuc.edu/Research/namd/libraries/tcl8.6.13-linux-x86_64-threaded.tar.gz
tar xzf fftw-linux-x86_64.tar.gz
tar xzf tcl8.6.13-linux-x86_64.tar.gz
tar xzf tcl8.6.13-linux-x86_64-threaded.tar.gz
mv linux-x86_64 fftw
mv tcl8.6.13-linux-x86_64 tcl
mv tcl8.6.13-linux-x86_64-threaded tcl-threaded 
./config Linux-x86_64-g++  --charm-base ./charm-8.0.0 --charm-arch ofi-crayshasta-cxi-slurmpmi2cray-smp --with-cuda --cuda-prefix /soft/compilers/cudatoolkit/cuda-12.2.2
cd Linux-x86_64-g++
make -j8
```

The NAMD binary is `namd3`. To build a memory-optimized version, the flag `--with-memopt` needs to be inserted as a config argument.

The configure steps above can be replaced with the following to build the GPU-resident version of NAMD.

```bash
./build charm++ netlrts-linux-x86_64 gcc smp -j8  --with-production
./config Linux-x86_64-g++  --charm-base ./charm-8.0.0 --charm-arch netlrts-linux-x86_64-smp-gcc --with-cuda --with-single-node-cuda --cuda-prefix /soft/compilers/cudatoolkit/cuda-12.2.2
```
