# Containers on Theta(GPU)

<!-- Needs updating for sophia: -->
<!-- On Theta(GPU), container creation can be achieved by using Docker on your local machine as mentioned in [Example `Dockerfile`](../../../theta/data-science-workflows/containers/containers.md#example-dockerfile), or using a Singularity recipe file and building on a Theta(GPU) worker node. -->

## Building using Docker
If you followed the `Dockerfile` instructions, using the Theta(GPU) specific [`Dockerfile_thetagpu`](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Dockerfile_thetagpu), you can build your container for ThetaGPU using:
```bash
singularity build <image_name> docker://<username>/<repo_name>:<tag>
# using tutorial example
singularity build my_image.simg docker://jtchilders/alcf_cwp_example:thetagpu
```

![singularity_build_thetagpu](../files/singularity_build_thetagpu.gif)

Then you can submit a job to Theta(GPU) using the [job submission script](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/job_submission_thetagpu.sh).

```bash
module load cobalt/cobalt-gpu
qsub -A <project-name> job_submission_thetagpu.sh ./my_image.simg
```

The output should look like this:
```
C++ MPI
Hello world from processor thetagpu12, rank 4 out of 16 processors
Hello world from processor thetagpu12, rank 7 out of 16 processors
Hello world from processor thetagpu12, rank 1 out of 16 processors
Hello world from processor thetagpu12, rank 5 out of 16 processors
Hello world from processor thetagpu12, rank 6 out of 16 processors
Hello world from processor thetagpu12, rank 0 out of 16 processors
Hello world from processor thetagpu12, rank 2 out of 16 processors
Hello world from processor thetagpu12, rank 3 out of 16 processors
Hello world from processor thetagpu18, rank 14 out of 16 processors
Hello world from processor thetagpu18, rank 15 out of 16 processors
Hello world from processor thetagpu18, rank 13 out of 16 processors
Hello world from processor thetagpu18, rank 8 out of 16 processors
Hello world from processor thetagpu18, rank 9 out of 16 processors
Hello world from processor thetagpu18, rank 11 out of 16 processors
Hello world from processor thetagpu18, rank 12 out of 16 processors
Hello world from processor thetagpu18, rank 10 out of 16 processors
Python MPI
Hello world from processor thetagpu18, rank 13 out of 16 processors
Hello world from processor thetagpu18, rank 8 out of 16 processors
Hello world from processor thetagpu18, rank 9 out of 16 processors
Hello world from processor thetagpu18, rank 14 out of 16 processors
Hello world from processor thetagpu18, rank 15 out of 16 processors
Hello world from processor thetagpu18, rank 11 out of 16 processors
Hello world from processor thetagpu18, rank 10 out of 16 processors
Hello world from processor thetagpu18, rank 12 out of 16 processors
Hello world from processor thetagpu12, rank 2 out of 16 processors
Hello world from processor thetagpu12, rank 5 out of 16 processors
Hello world from processor thetagpu12, rank 0 out of 16 processors
Hello world from processor thetagpu12, rank 6 out of 16 processors
Hello world from processor thetagpu12, rank 4 out of 16 processors
Hello world from processor thetagpu12, rank 1 out of 16 processors
Hello world from processor thetagpu12, rank 7 out of 16 processors
Hello world from processor thetagpu12, rank 3 out of 16 processors
```

## Building using Singularity Recipes

While building using Docker on your local machine tends to be the easier method, there are sometimes reasons to build in the environment of the supercomputer. In this case, one can build a Singularity container on ThetaGPU in an interactive session on a compute (or worker) node. First, a recipe file is needed. Below is an example Singularity definition file, which can also be found [here](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/mpi.def).

Detailed directions for recipe construction are available on the [Singularity Recipe Page](https://sylabs.io/guides/2.6/user-guide/container_recipes.html).

## Example Singularity definition file

Here we have defined the base image from which to `bootstrap` our container. We are using an image from Docker Hub, `ubuntu:20.04`.

```singularity
Bootstrap: docker
From: ubuntu:20.04
```

The `%files` section lists files to copy from the host system (left path) to the container filesystem (right path) prior to build time.

```singularity
%files
	../Local/source/* /usr/source/
	../Local/submit.sh /usr/
```

The `%environment` section defines environment variables that will be available to the container at runtime.

```singularity
%environment
	export PATH=$PATH:/mpich/install/bin
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/mpich/install/lib
```

The `%post` section executes within the container at build time on top of our `ubuntu:20.04` operating system. The `%post` section is therefore the place to perform installations of custom apps with syntax similar to BASH.

```singularity
%post
	#### INSTALL BASE PACKAGES NEEDED FOR MPI APPLICATIONS AND PYTHON3 ####
	DEBIAN_FRONTEND=noninteractive
	apt-get update -y \
	&& DEBIAN_FRONTEND=noninteractive \
	&& apt-get install -y build-essential libfabric-dev libibverbs-dev gfortran wget \
	&& apt-get install -y python3 python3-distutils python3-pip gcc

	#### DOWNLOAD AND INSTALL MPICH AND MPI4PY ####
	# Source is available at http://www.mpich.org/static/downloads/
	# See installation guide of target MPICH version
	# Ex: https://www.mpich.org/static/downloads/4.0.2/mpich-4.0.2-installguide.pdf
	# These options are passed to the steps below
	OPENMPI_VERSION_A="4.0"
	OPENMPI_VERSION_B="4.0.5"
	OPENMPI_CONFIGURE_OPTIONS="--prefix=/openmpi/install --disable-wrapper-rpath --disable-wrapper-runpath"
	OPENMPI_MAKE_OPTIONS="-j"
	mkdir -p openmpi
	cd /openmpi
	wget https://download.open-mpi.org/release/open-mpi/v${OPENMPI_VERSION_A}/openmpi-${OPENMPI_VERSION_B}.tar.gz
	tar xfz openmpi-${OPENMPI_VERSION_B}.tar.gz  --strip-components=1
   ./configure ${OPENMPI_CONFIGURE_OPTIONS}
   make install ${OPENMPI_MAKE_OPTIONS}
	
	export PATH=$PATH:/openmpi/install/bin
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/openmpi/install/lib

	pip install mpi4py

	#### BUILD FILES ####
	chmod +x /usr/submit.sh
	mpicc -o /usr/source/mpi_hello_world /usr/source/mpi_hello_world.c
```

The `%runscript` section defines actions for the container to take when it is executed using `singularity run <container_name>`.

```singularity
%runscript
	exec /usr/submit.sh "$@"
```

The `%labels` section allows for custom metadata to be added to the container.

```singularity
%labels
        MAINTAINER Aditya atanikanti@anl.gov
```

The `%help` section can be used to define how to build and run the container.

```singularity
%help
    	This container is used to illustrate an MPI-based def file to build a container running Python and C programs. To build the container, use singularity build --fakeroot mpi.sif mpi.def
```

## Build Singularity container on ThetaGPU compute

After logging on to Theta login nodes, launch an interactive job using the attrs `fakeroot=true`, `pubnet=true` and specifying the filesystems `filesystems=home,theta-fs0`.

```bash
# on Theta login node, must load cobalt-gpu module to submit jobs to ThetaGPU
module load cobalt/cobalt-gpu
qsub -I -n 1 -t 01:00:00 -q single-gpu -A <project_name> --attrs fakeroot=true:pubnet=true:filesystems=home,theta-fs0
```

Before building the container, make sure the ThetaGPU compute nodes have access to external resources. This is achieved by setting the `http_proxy` and `https_proxy` variables.
```bash
# setup network proxy to reach outside world
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
```

Now build the container using `--fakeroot` where `<def_filename>.def` is the definition file we have defined in the example above and `<image_name>.sif` is the user-defined image file name. Using [mpi.def](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/mpi.def) example:
```bash
# important you run this in the proper path because the file copies in
# the `%files` section of the recipe uses relative paths on the host.
cd 
singularity build --fakeroot <image_name>.sif <def_filename>.def 
```

## Run Singularity container on ThetaGPU compute

An example job submission script is here: [job_submission_thetagpu.sh](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/job_submission_thetagpu.sh).

First, we define our job, and our script takes the container name as an input parameter.

```bash
#!/bin/bash -l
#COBALT -n 1
#COBALT -t 00:10:00
#COBALT -q single-gpu
#COBALT --attrs filesystems=home,theta-fs0:pubnet=true
CONTAINER=$1
```

Enable network access at runtime by setting the proxy.

```bash
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
```

Set up our MPI settings, figure out the number of nodes `NODES`, fix the number of processes per node `PPN`, and multiply to get total MPI ranks `PROCS`.

```bash
NODES=`cat $COBALT_NODEFILE | wc -l`
PPN=8 # GPUs per NODE
PROCS=$((NODES * PPN))
echo NODES=$NODES  PPN=$PPN  PROCS=$PROCS
```

The OpenMPI installed on ThetaGPU must be used for MPI to properly run across nodes. Here the library path is added to `SINGULARITYENV_LD_LIBRARY_PATH`, which will be used by Singularity to set the container's `LD_LIBRARY_PATH` and therefore tell our executables where to find the MPI libraries.

```bash
MPI_BASE=/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/
export LD_LIBRARY_PATH=$MPI_BASE/lib:$LD_LIBRARY_PATH
export SINGULARITYENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo mpirun=$(which mpirun)
```

Finally, the executable is launched. Notice on NVidia systems that the `singularity exec` or `singularity run` commands must use the `--nv` flag to pass important libraries/drivers from the host to the container environment.

```bash
mpirun -hostfile $COBALT_NODEFILE -n $PROCS -npernode $PPN singularity exec --nv -B $MPI_BASE $CONTAINER /usr/source/mpi_hello_world
```
The job can be submitted using:
```bash
qsub -A <project-name> job_submission_thetagpu.sh /path/to/my_image.sif
```

The output should look like this:
```bash
C++ MPI
Hello world from processor thetagpu02, rank 12 out of 16 processors
Hello world from processor thetagpu02, rank 8 out of 16 processors
Hello world from processor thetagpu02, rank 10 out of 16 processors
Hello world from processor thetagpu02, rank 11 out of 16 processors
Hello world from processor thetagpu02, rank 13 out of 16 processors
Hello world from processor thetagpu02, rank 9 out of 16 processors
Hello world from processor thetagpu02, rank 14 out of 16 processors
Hello world from processor thetagpu02, rank 15 out of 16 processors
Hello world from processor thetagpu01, rank 0 out of 16 processors
Hello world from processor thetagpu01, rank 1 out of 16 processors
Hello world from processor thetagpu01, rank 2 out of 16 processors
Hello world from processor thetagpu01, rank 3 out of 16 processors
Hello world from processor thetagpu01, rank 4 out of 16 processors
Hello world from processor thetagpu01, rank 5 out of 16 processors
Hello world from processor thetagpu01, rank 6 out of 16 processors
Hello world from processor thetagpu01, rank 7 out of 16 processors
Python MPI
Hello world from processor thetagpu02, rank 9 out of 16 processors
Hello world from processor thetagpu02, rank 10 out of 16 processors
Hello world from processor thetagpu02, rank 11 out of 16 processors
Hello world from processor thetagpu02, rank 15 out of 16 processors
Hello world from processor thetagpu02, rank 13 out of 16 processors
Hello world from processor thetagpu02, rank 8 out of 16 processors
Hello world from processor thetagpu02, rank 12 out of 16 processors
Hello world from processor thetagpu02, rank 14 out of 16 processors
Hello world from processor thetagpu01, rank 7 out of 16 processors
Hello world from processor thetagpu01, rank 3 out of 16 processors
Hello world from processor thetagpu01, rank 1 out of 16 processors
Hello world from processor thetagpu01, rank 4 out of 16 processors
Hello world from processor thetagpu01, rank 5 out of 16 processors
Hello world from processor thetagpu01, rank 6 out of 16 processors
Hello world from processor thetagpu01, rank 0 out of 16 processors
Hello world from processor thetagpu01, rank 2 out of 16 processors
```

## Pre-existing Images for Deep Learning

There are several containers on ThetaGPU that will help you get started with deep learning experiments that can efficiently use the A100 GPUs. We have different optimized containers for DL here `ls /lus/theta-fs0/software/thetagpu/nvidia-containers/`.

The [bootstrap.def](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/bootstrap.def) gives an example of how these containers were created.

The image is bootstrapped from an NVidia image, in this case from a [PyTorch](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch) build. One can also use the [TensorFlow](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) build. At the time of this writing, the latest tag for the PyTorch image was `22.04-py3`, but users should select the version that best suits their needs.

```singularity
Bootstrap: docker
From: nvcr.io/nvidia/pytorch:22.04-py3
```
Next, we need to install MPI support for cross-node parallel training.

```singularity
%post

    # Install mpi4py
    CC=$(which mpicc) CXX=$(which mpicxx) pip install --no-cache-dir mpi4py

    # Install horovod
    CC=$(which mpicc) CXX=$(which mpicxx) HOROVOD_WITH_TORCH=1 pip install --no-cache-dir horovod
```
Next, build the container on a ThetaGPU compute node, following the instructions in the previous section. Then an example job submission script is here: [job_submission_thetagpudl.sh](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/ThetaGPU/job_submission_thetagpudl.sh).