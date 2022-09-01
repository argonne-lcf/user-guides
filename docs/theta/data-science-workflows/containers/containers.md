# Containers on Theta(KNL)
Containers are a method for shipping software that is pre-built inside a pre-defined static software environment. 
At ALCF, users must run [Singularity](https://sylabs.io/guides/3.8/user-guide/index.html) containers. Singularity is a container technology built for supercomputers with security in mind. 
Typically we recommend users build [Docker](https://docs.docker.com/) containers first, which can then be easily converted to Singularity containers.

We will not repeat the detailed instructions for building docker containers, but do provide system specific examples of what a `Dockerfile` should look like below.

* General Docker documentation can be found[here](https://docs.docker.com/)
* Specifics on building docker container recipes using `Dockerfile` can be found [here](https://docs.docker.com/engine/reference/builder/)

The trickiest parts of building containers for ALCF systems is ensuring proper MPI support and GPU driver compatibility.

## Docker

The easiest way to build a container is from your laptop. [First, install Docker.](https://docs.docker.com/get-docker/) Then follow these steps.

We have an example installation in the [Container](https://github.com/argonne-lcf/GettingStarted/tree/master/DataScience/Containers) directory, which contains an `Dockerfile`. This is a container recipe file that we will use to tell Docker how to install our software.

### Example `Dockerfile`

We include example build source code here: [Local Example Source](https://github.com/argonne-lcf/GettingStarted/tree/master/DataScience/Containers/source). This includes an example [Dockerfile](https://github.com/argonne-lcf/GettingStarted/blob/master/DataScience/Containers/Dockerfile) which we will describe line-by-line below.

```Dockerfile
FROM ubuntu:20.04
```
The first line specifies a starting point for our container. In this instance, we start from a container that only has Ubuntu version 20.04 installed. However, we could start with any other container available on Docker Hub. We'll build everything else on top of this operating system.

```Dockerfile
RUN apt-get update -y \
	&& DEBIAN_FRONTEND=noninteractive \
	&& apt-get install -y build-essential libfabric-dev libibverbs-dev gfortran wget \
        && apt-get install -y python3 python3-distutils python3-pip gcc
```

Here we install system packages we need to build and run our code examples using the standard [Ubuntu package manager](https://ubuntu.com/server/docs/package-management#:~:text=The%20apt%20command%20is%20a,upgrading%20the%20entire%20Ubuntu%20system.) `apt`.

```Dockerfile
WORKDIR /mpich
# Source is available at http://www.mpich.org/static/downloads/
# See installation guide of target MPICH version
# Ex: https://www.mpich.org/static/downloads/4.0.2/mpich-4.0.2-installguide.pdf
# These options are passed to the steps below
ARG MPICH_VERSION="3.3"
ARG MPICH_CONFIGURE_OPTIONS="--prefix=/mpich/install --disable-wrapper-rpath"
ARG MPICH_MAKE_OPTIONS="-j 4"
RUN wget http://www.mpich.org/static/downloads/${MPICH_VERSION}/mpich-${MPICH_VERSION}.tar.gz \
      && tar xfz mpich-${MPICH_VERSION}.tar.gz  --strip-components=1 \
      && ./configure ${MPICH_CONFIGURE_OPTIONS} \
      && make install ${MPICH_MAKE_OPTIONS}
ENV PATH $PATH:/mpich/install/bin
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/mpich/install/lib
```

Here we change our working directory to `/mpich` and then download and install MPI from scratch with some specific build options. You can find the installation documentation [HERE](https://www.mpich.org/static/downloads/4.0.2/mpich-4.0.2-installguide.pdf). The key compilation option is the `--disable-wrapper-rpath` which makes it possible to build applications inside the container using this MPI library, but then replace those libraries with the Theta-specific libraries during runtime simply using the `LD_LIBRARY_PATH` environment variable. This is important since Theta uses high-speed network interfaces that require custom drivers and interface libraries to use.

```Dockerfile
RUN pip install mpi4py
```

Here we simply install `mpi4py` into our python environment and it will utilize the MPICH we installed.

```Dockerfile
WORKDIR /usr
COPY source/* /usr/source/
COPY submit.sh /usr/
RUN chmod +x /usr/submit.sh
RUN mpicc -o /usr/source/mpi_hello_world /usr/source/mpi_hello_world.c
```

Next we copy the [source/](https://github.com/argonne-lcf/GettingStarted/tree/master/DataScience/Containers/source) code examples from our repo (paths are with respect to the location of the `Dockerfile`) into our containers filesystem and build the C-file into a binary we can later execute on Theta.

```Dockerfile
ENTRYPOINT ["/usr/submit.sh"]
```

In Docker (and Singularity) you can simply "run" a container if an entry point is defined, so calling `docker run <container-name>` in this recipe executes our `submit.sh` script. Otherwise we can be more explicit can call any binary in our container using `docker exec <container-name> <command>`.

### Building the Docker Image and Upload to Docker Hub

Side Note: 
Docker uses the terms "image" and "contianer" this way: 'images' are a static software environment from which 'containers' can be launched and created. For instance, I would have one image named `my_analysis_app` which contains my applications. However, this application can be run on multiple input data files in parallel. I can therefore use this image to launch multiple concurrent 'containers' and provide each one with different input data. Each 'container' is an isolated application running. They can be 'started' and 'stopped' or 'restarted'. I can 'enter' these containers interactively and do things to the running environment, e.g. `apt install python`, but when my container stops running all these changes are gone. Only changes to the base image last (though you can create a new image from a running container).

The first step is to create Docker Hub account here: [DockerHub Hub](https://hub.docker.com/). Then follow these steps.

Create a Docker Hub Repository:

![docker_hub_repo](../files/docker_hub_repo.gif)

Then build your Image:

```bash
# build image from Dockerfile, include the path to the folder that contains the Dockerfile
cd /path/to/CompPerfWorkshop/03_containers
docker build -t jtchilders/alcf_cwp_example:latest ./Local/
```
Here you see my username and repository name `jtchilders/alcf_cwp_example` and then attached with a `:` is the "tag" for this image. Just like GitHub, an image is tagged with a version. It's traditional in Docker to use `latest` for the most recent version, but otherwise, you can use any string you like.

Then `./Local/` simply points to the folder which contains my `Dockerfile`. The build recipe will be run in that folder and have access to all the source files inside. As an example, the instructions to build images for Theta(KNL) and Theta(GPU) are different. In order to build the image for Theta(GPU) you need to use the `Dockerfile_thetagpu` and must change the build command so it knows to use this file instead of the default:

```bash
docker build -t jtchilders/alcf_cwp_example:thetagpu -f ./Local/Dockerfile_thetagpu ./Local/
```

In this case, I'm still naming the image with my `username/repo` but I've changed my tag name to `thetagpu` so I can distinguish it from the previous image we built. I've also instructed docker on which `Dockerfile` to use as the recipe.

Last step is to make this image accessible on Theta, so we'll push the newly built image to Docker Hub using:
```bash
docker push jtchilders/alcf_cwp_example:latest
docker push jtchilders/alcf_cwp_example:thetagpu
```

Quick demo:

![docker_hub_repo_build](../files/docker_hub_repo_build.gif)

## Singularity

### Building Singularity images from Docker images on Theta

Anywhere singularity is installed, for instance on the Theta login nodes, you can run a build command based off a docker image using:

```bash
singularity build <image_name> docker://<username>/<repository_name>:<tag>
```

Example:
  
![Singularity build](../files/singularity_build.gif)

### Run Singularity container on Theta

```bash
qsub -A <project-name> /path/to/job_submission_theta.sh </path/to/image_name>
```

### Example `job_submission_theta.sh`

First we define the job submission parameters (number of nodes `-n`, queue name `-q`, wall time `-t`, etc.) that are needed to submit a job on Theta(KNL), the number of ranks per node, and the container is passed as an argument to the submission script.

```bash
#!/bin/bash
#COBALT -t 30
#COBALT -q debug-flat-quad
#COBALT -n 2
#COBALT -A <project_name>
#COBALT --attrs filesystems=theta-fs0,home
RANKS_PER_NODE=4
CONTAINER=$1
```
Next we load the proper Cray MPICH module for MPI support on Theta. ABI (Application Binary Independent) simply means, we can build our application inside the image using the MPICH we installed there. Then, at run-time, we will use the `LD_LIBRARY_PATH` to point our application at Cray's MPICH libraries instead. ABI enforces the use of a common interface to enable this swapping in and out. Otherwise, this would fail.

```bash
# Use Cray's Application Binary Independent MPI build
module swap cray-mpich cray-mpich-abi
#Only needed when interactive debugging
#module swap PrgEnv-intel PrgEnv-cray; module swap PrgEnv-cray PrgEnv-intel
```

These `ADDITIONAL_PATHS` are the paths to dependencies from the Cray modules.

```bash
export ADDITIONAL_PATHS="/opt/cray/diag/lib:/opt/cray/ugni/default/lib64/:/opt/cray/udreg/default/lib64/:/opt/cray/xpmem/default/lib64/:/opt/cray/alps/default/lib64/:/opt/cray/wlm_detect/default/lib64/"
```

Now we add all these library paths to the `SINGULARITYENV_LD_LIBRARY_PATH` which will be used by Singularity to set the `LD_LIBRARY_PATH` environment variable inside the container at runtime.

```bash
# The LD_LIBRARY_PATH and/or PATH environment variables in a 
# Singularity container can be altered only using the SINGULARITYENV_LD_LIBRARY_PATH 
# or SINGULARITYENV_PATH environment variables prior to execution.
export SINGULARITYENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATHS"
```

We need all the libraries to be accessible inside the container, therefore, we "bind-mount" the base path to all our dependencies using the `-B` option.

```bash
# need to mount these folders inside the container so that the Cray MPICH libraries will be found.
BINDINGS="-B /opt -B /etc/alternatives"
```

Next the actual run commands that combine the `aprun` launcher call that handles the MPI, and the `singularity` call to handle the containerized environment.

```bash
TOTAL_RANKS=$(( $COBALT_JOBSIZE * $RANKS_PER_NODE ))
# run my containner like an application
aprun -n $TOTAL_RANKS -N $RANKS_PER_NODE singularity exec $BINDINGS $CONTAINER /usr/source/mpi_hello_world
aprun -n $TOTAL_RANKS -N $RANKS_PER_NODE singularity exec $BINDINGS $CONTAINER python3 /usr/source/mpi_hello_world.py
```

The output should look like this:
```
Hello world from processor nid00020, rank 2 out of 8 processors
Hello world from processor nid00020, rank 3 out of 8 processors
Hello world from processor nid00020, rank 0 out of 8 processors
Hello world from processor nid00020, rank 1 out of 8 processors
Hello world from processor nid00021, rank 6 out of 8 processors
Hello world from processor nid00021, rank 7 out of 8 processors
Hello world from processor nid00021, rank 4 out of 8 processors
Hello world from processor nid00021, rank 5 out of 8 processors
Application 26449404 resources: utime ~14s, stime ~8s, Rss ~39912, inblocks ~64022, outblocks ~0
Hello world from processor nid00021, rank 7 out of 8 processors
Hello world from processor nid00021, rank 6 out of 8 processors
Hello world from processor nid00021, rank 5 out of 8 processors
Hello world from processor nid00021, rank 4 out of 8 processors
Hello world from processor nid00020, rank 2 out of 8 processors
Hello world from processor nid00020, rank 3 out of 8 processors
Hello world from processor nid00020, rank 1 out of 8 processors
Hello world from processor nid00020, rank 0 out of 8 processors
Application 26449405 resources: utime ~14s, stime ~8s, Rss ~39392, inblocks ~83290, outblocks ~0
```

## References
[Using Containers on Theta](https://www.alcf.anl.gov/asset/using-containers-theta)

<iframe width="560" height="315" src="https://www.youtube.com/embed/g6jkr-Ll9TA" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>



