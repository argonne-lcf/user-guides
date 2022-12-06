# Containers on Polaris

Since Polaris is using NVIDIA A100 GPUs, there can be portability advantages with other NVIDIA-based systems if your workloads use containers.  In this document, we'll outline some information about containers on Polaris including how to build custom containers, how to run containers at scale, and common gotchas. 
Container creation can be achieved one of two ways either by using Docker on your local machine as mentioned in [Docker](../../../theta/data-science-workflows/containers/containers.md#docker) section of Theta(KNL) and publishing it to DockerHub, or by using a Singularity recipe file and building on a Polaris worker node. If you are not interested in building a container and only want to use the available containers, you can read the section on [available containers](#available-containers).

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


## Build from Docker Images

Docker containers require root privileges, which users do not have on Polaris.  That doesn't mean all your docker containers aren't useful, though.  If you have an existing docker container, you can convert it to singularity pretty easily on the login node. To build the latest NVIDIA container for PyTorch you can run the following:

```bash
module load singularity
singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

Note that `latest` here mean when these docs were written, summer 2022.  It may be useful to get a newer container if you need the latest features.  You can find the PyTorch container site [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers).


## Build with a Recipe

You can also build a singularity container using a recipe file. Detailed instructions for recipe construction are available on the [Singularity Recipe Page](https://sylabs.io/guides/2.6/user-guide/container_recipes.html). You can also check our [singularity recipe example](../../../theta-gpu/data-science-workflows/containers/containers.md#example-singularity-definition-file) for ThetaGPU.

Once you have a recipe file, you can build it on Polaris, but only on compute nodes. You can launch an interactive job using the attribute `singularity_fakeroot=true` to build on a compute node. 

```bash
qsub -I -A <project_name> -q <queue> -l select=1 -l walltime=60:00 -l singularity_fakeroot=true -l filesystems=home:eagle:grand
```

You need to replace the `<project_name>` with the appropriate project to charge and `<queue>` with `debug`, or `preemptable` queues since we only request a single node. 

After your interactive job has started, you need to load the `singularity` module on the compute node and export the proxy variables for internet access. Then you can build the container as shown below.

```bash
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
singularity build --fakeroot <image_name>.sif <def_filename>.def 
```

For example, let's use the definition file from [the tutorial example](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Polaris/bootstrap.def):

```bash
wget https://raw.githubusercontent.com/argonne-lcf/GettingStarted/master/DataScience/Containers/Polaris/bootstrap.def
singularity build --fakeroot bootstrap.sif bootstrap.def
```
You can find more details about the `bootstrap.def` file, [here](../../../theta-gpu/data-science-workflows/containers/containers.md#pre-existing-images-for-deep-learning).

## Running Singularity container on Polaris

`bootstrap.def` extends existing NVIDIA containers by installing `mpi4py` and `horovod` in it. Now to run the `bootstrap.sif` file on Polaris compute, you can use the [submission script](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Polaris/job_submission_dl.sh).

### Example submission script on Polaris

First we define our job and our script takes the container name as an input parameter.
```bash
#!/bin/sh
#PBS -l select=1
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
NRANKS_PER_NODE=$(nvidia-smi -L | wc -l)
NTHREADS=1
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"
```

Finally the executable is launched. Notice on NVIDIA systems that the `singularity exec` or `singularity run` commands must use the `--nv` flag to pass important libraries/drivers from the host to the container environment.

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
Containers are stored at `/soft/containers/`, within `pytorch` and `tensorflow` subfolders. The latest containers are updated periodically. If you have trouble using containers, or request a newer or a different container please contact ALCF support at `support@alcf.anl.gov`.

!!! warning
    These containers work out-of-the-box on a single node, but currently we are investigating a problem with multi-node runs. Once the problem is resolved, we will also include containers with `mpi4py` and `horovod` for Polaris.
    

## Troubleshooting

One may get a `permission denied` error during the build process, due to a nasty permission setting, quota limitations, or simply due to an unresolved symbolic link. You can try one of the solutions below:

1. Check your quota and delete any unnecessary files. 

2. Clean-up singularity cache, `~/.singularity/cache`, and set the singularity tmp and cache directories as below:

    ```bash
    export SINGULARITY_TMPDIR=/tmp/singularity-tmpdir
    mkdir $SINGULARITY_TMPDIR
    
    export SINGULARITY_CACHEDIR=/tmp/singularity-cachedir/
    mkdir $SINGULARITY_CACHEDIR
    ``` 

3. Make sure you are not on a directory accessed with a symlink, i.e. check if `pwd` and `pwd -P` returns the same path.
4. If any of the above doesn't work, try running the build in your home directory.
