# Containers on Polaris

Since Polaris is using nvidia A100 GPUs, there can be portability advantages with other nvidia-based systems if your workloads use containers.  In this document, we'll outline some information about containers on Polaris including how to build custom containers, how to run containers at scale, and common gotchas. 
Container creation can be achieved one of two ways either by using Docker on your local machine as mentioned in [Docker](../../../theta/data-science-workflows/containers/containers.md#docker) section of Theta(KNL) and publishing it to DockerHub, or using a Singularity recipe file and building on a Polaris worker node.

## Singularity

The container system on Polaris is `singularity`.  You can set up singularity with a module (this is different than, for example, ThetaGPU!):

```bash
# To see what versions of singularity are available:
module avail singularity

# To load the Default version:
module load singularity

# To load a specific version:
module load singularity/3.8.7 # the default at the time of writing these docs.

```

### Which Singularity?

There used to be a single `singularity` tool, which in 2021 split after some turmoil.  There are now two `singularity`s: one developed by Sylabs, and the other as part of the Linux Foundation.  Both are open source, and the split happened around version 3.10.  The version on Polaris is from [Sylabs](https://sylabs.io/docs/) but for completeness, here is the [Linux Foundation's version](https://github.com/apptainer/apptainer).  Note that the Linux Foundation version is renamed to `apptainer` - different name, roughly the same thing though divergence may happen after 2021's split.

### Additional settings to store cache

If you aren't interested in any of that, just skip to the bottom to see the available containers.
```
export SINGULARITY_TMPDIR=/tmp/singularity-tmpdir
mkdir $SINGULARITY_TMPDIR

SINGULARITY_CACHEDIR=/tmp/singularity-cachedir/
mkdir $SINGULARITY_CACHEDIR
```

## Build using Docker

Docker containers require root privileges to run, which users do not have on Polaris.  That doesn't mean all your docker containers aren't useful, though.  If you have an existing docker container, you can convert it to singularity pretty easily on the login node:
To Build the latest nvidia container you can
```bash
# Singularity can automatically download the dockerhub hosted container and build it as a singularity container:
$ singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```
Note that `latest` here mean when these docs were written, summer 2022.  It may be useful to get a newer container if you need the latest features.  You can find the pytorch container site [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers)

Alternatively if you want to build singularity on a worker node, you can request a worker node in interactive mode, export proxy variable for internet access, and finally use `singularity build` as mentioned below.

```bash
qsub -I -A <project_name> -l select=10:system=polaris -l walltime=30:00 -l singularity_fakeroot=true
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
# using tutorial example
singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

## Build singularity container on Polaris compute

If you want to build in the environment of the supercomputer, one can build a singularity container on Polaris in an interactive session on a compute (or worker) node. First a recipe file is needed, detailed directions for recipe construction are available on the [Singularity Recipe Page](https://sylabs.io/guides/2.6/user-guide/container_recipes.html) or to understand our singularity file example see ThetaGPU page section [Example Singularity definition file](../../../theta-gpu/data-science-workflows/containers/containers.md#example-singularity-definition-file)

On Polaris, once you have a recipe file, launch an interactive job using the attribute `singularity_fakeroot=true`, followed by `module load singularity module`. You will also have to export proxy variable for internet access from the worker node, and finally use `singularity build --fakeroot` as mentioned below.

```bash
qsub -I -A <project_name> -l select=10:system=polaris -l walltime=30:00 -l singularity_fakeroot=true
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
singularity build --fakeroot <image_name>.sif <def_filename>.def 
```

For example, let's use the definition file the tutorial example from [here](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Polaris/bootstrap.def)

```bash
cd /path/to/CompPerWorkshop/03_containers/ThetaGPU
singularity build --fakeroot bootstrap.sif bootstrap.def
```
To understand our bootstrap.def file, see [Pre-existing Images for Deep Learning](../../../theta-gpu/data-science-workflows/containers/containers.md#pre-existing-images-for-deep-learning)

## Running Singularity container on Polaris

bootstrap.def extends existing nvidia containers by installing mpi4py and horvod in it. Now to run the bootstrap.sif file on Polaris compute use the [submission script](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Polaris/job_submission_dl.sh)

### Example submission script on Polaris

First we define our job and our script takes the container name as an input parameter.
```bash
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:grand:eagle
#PBS -A Datascience
```

We move to current working directory and enable network access at run time by setting the proxy. We also load singularity.

```bash
cd ${PBS_O_WORKDIR}
# SET proxy for internet access
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
```

Setup our MPI settings and download pytorch benchmark tool

```bash
wget https://github.com/horovod/horovod/raw/master/examples/pytorch/pytorch_synthetic_benchmark.py
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=8
NDEPTH=8
NTHREADS=1
#PPN=16
#PROCS=$((NODES * PPN))
#echo "NUM_OF_NODES= ${NODES} TOTAL_NUM_RANKS= ${PROCS} RANKS_PER_NODE= ${PPN}"
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"
```

Finally the exectuable is launched. Notice on NVidia systems that the `singularity exec` or `singularity run` commands must use the `--nv` flag to pass important libraries/drivers from the host to the container environment.

```bash
echo test mpi
mpiexec -n ${NNODES} --ppn 1 \
   singularity exec --nv -B $PWD $CONTAINER \
      python $PWD/pytorch_synthetic_benchmark.py
```

The job can be submitted using:

```bash
qsub -v CONTAINER=bootstrap.sif job_submission_dl.sh
```

## Available containers

If you just want to know what containers are available, here you go. 
Containers are stored at `/soft/containers/`, with a subfolder for pytorch and tensorflow.  The default nvidia containers "as-is" are available, and soon we will also distribute containers with mpi4py and Horovod capabilities that work on Polaris.
The latest containers are updated periodically.  If you have trouble with them, or a new container is available that you need (or want something outside to Torch/TensorFlow) please contact ALCF support at `support@alcf.anl.gov`.
