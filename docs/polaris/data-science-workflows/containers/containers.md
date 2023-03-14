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


## Build from Docker Images or Argonne Github container registry

Docker containers require root privileges, which users do not have on Polaris.  That doesn't mean all your docker containers aren't useful, though.  If you have an existing docker container, you can convert it to singularity pretty easily on the login node. To build the latest NVIDIA container for PyTorch you can run the following:

```bash
module load singularity
singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

Note that `latest` here mean when these docs were written, summer 2022.  It may be useful to get a newer container if you need the latest features.  You can find the PyTorch container site [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers).

You can also use our custom built containers using Github OCI container [registry](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry). Here's a list of [containers](https://github.com/orgs/argonne-lcf/packages) distributed by ALCF staff tailored for Polaris.

```bash
module load singularity
singularity pull IMAGE_NAME oras://ghcr.io/argonne-lcf/IMAGE_NAME:latest
```

## Build with a Recipe

You can also build a singularity container using a recipe file. Detailed instructions for recipe construction are available on the [Singularity Recipe Page](https://sylabs.io/guides/2.6/user-guide/container_recipes.html). You can also check our [singularity recipe example](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/mpich.def) for building a mpich version 4 container on Polaris.

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

Alternatively, you can just pull the mpich 4 image distributed by us and build on top of it

```bash
singularity pull oras://ghcr.io/argonne-lcf/mpich-4:latest
```


## Running Singularity container on Polaris

### Example submission script on Polaris

To run a container on Polaris you can use the submission script described [here](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/job_submission.sh). Below we have described the submission script for your understanding.

First we define our job and our script takes the container name as an input parameter.

```bash
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:grand
#PBS -A <project_name>
cd ${PBS_O_WORKDIR}
echo $CONTAINER
```

We move to current working directory and enable network access at run time by setting the proxy. We also load singularity.

```bash
# SET proxy for internet access
module load singularity
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
```

This is important for system (Polaris - Cray) mpich to bind to containers mpich. Set the following environment variables

```bash
ADDITIONAL_PATH=/opt/cray/pe/pals/1.1.7/lib/
module load cray-mpich-abi
export SINGULARITYENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"
```

Set the number of ranks per node spread as per your scaling requirements

```bash
# MPI example w/ 16 MPI ranks per node spread evenly across cores
NODES=`wc -l < $PBS_NODEFILE`
PPN=16
PROCS=$((NODES * PPN))
echo "NUM_OF_NODES= ${NODES} TOTAL_NUM_RANKS= ${PROCS} RANKS_PER_NODE= ${PPN}"
```

Finally launch your script

```bash
echo C++ MPI
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN singularity exec -B /opt -B /var/run/palsd/ $CONTAINER /usr/source/mpi_hello_world

echo Python MPI
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN singularity exec -B /opt -B /var/run/palsd/ $CONTAINER python3 /usr/source/mpi_hello_world.py
```

The job can be submitted using:

```bash
qsub -v CONTAINER=mpich-4_latest.sif job_submission.sh
```

## Available containers

If you just want to know what containers are available, here you go. 

* Some of the available containers are stored at `/soft/containers/`
* For running databases on Polaris. It can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/databases)
* For using shpc - that allows for running containers as modules. It can be found [here](https://github.com/argonne-lcf/container-registry/blob/main/containers/shpc/shpc.md)

The latest containers are updated periodically. If you have trouble using containers, or request a newer or a different container please contact ALCF support at `support@alcf.anl.gov`.
 

## Troubleshooting

1. *Permission Denied Error*: One may get a `permission denied` error during the build process, due to a nasty permission setting, quota limitations, or simply due to an unresolved symbolic link. You can try one of the solutions below:
 * Check your quota and delete any unnecessary files. 
 * Clean-up singularity cache, `~/.singularity/cache`, and set the singularity tmp and cache directories as below:

```bash
 export SINGULARITY_TMPDIR=/tmp/singularity-tmpdir
 mkdir $SINGULARITY_TMPDIR
 export SINGULARITY_CACHEDIR=/tmp/singularity-cachedir/
 mkdir $SINGULARITY_CACHEDIR
``` 
 * Make sure you are not on a directory accessed with a symlink, i.e. check if `pwd` and `pwd -P` returns the same path.
 * If any of the above doesn't work, try running the build in your home directory.

2. *Mapping to rank 0 on all nodes*: This is mainly due to container mpich not binding to system mpich. It is imperative for the container to have mpich which can bind dynamically to system mpich at runtime. Ensuring your submission script has the following variables (see below) and the container to have mpich which is built with the the ' --disable-wrapper-rpath' flag will rectify this error. Please refer to this [link](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/job_submission.sh).
 ```bash
 ADDITIONAL_PATH=/opt/cray/pe/pals/1.1.7/lib/
 module load cray-mpich-abi
 export SINGULARITYENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"
 singularity exec -B /opt -B /var/run/palsd/
 ```

3. libmpi.so.40 not found: This may be due to mpich binding to the wrong system mpich. Try removing .conda & .cache & .local folders from your home directory. Also rebuild your container and try again.

