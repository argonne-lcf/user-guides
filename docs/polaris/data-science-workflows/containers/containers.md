# Containers on Polaris
Since Polaris is using NVIDIA A100 GPUs, there can be portability advantages with other NVIDIA-based systems if your workloads use containers.  In this document, we'll outline some information about containers on Polaris including how to build custom containers, how to run containers at scale, and common gotchas. 

Container creation can be achieved one of two ways either by using Docker on your local machine and publishing it to DockerHub, or by using a Apptainer recipe file and building on a Polaris worker node. If you are not interested in building a container and only want to use the available containers, you can you can `pull` existing containers from any of the container registries like DockerHub and run them on Polaris.

## Apptainer

The container system on Polaris is now upgraded to `apptainer` previously called as `singularity`.  You can set up apptainer as follows:

```bash
module use /soft/spack/gcc/0.6.1/install/modulefiles/Core
module load apptainer
apptainer version #1.2.2
```

The version of apptainer installed on Polaris is `1.2.2`. You can find user documentation [here](https://apptainer.org/docs/user/1.2/)

## Build from Docker Images or Argonne Github container registry

Docker containers require root privileges, which users do not have on Polaris.  That doesn't mean all your docker containers aren't useful, though.  If you have an existing docker container, you can convert it to apptainer pretty easily. For example, to build the latest NVIDIA container for PyTorch you can run the following on a Polaris compute node:

```bash
qsub -I -A <Project> -q debug -l select=1 -l walltime=01:00:00 -l filesystems=home:eagle -l singularity_fakeroot=true
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
module use /soft/spack/gcc/0.6.1/install/modulefiles/Core
module load apptainer
apptainer build --fakeroot pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

 You can find the latest PyTorch containers [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The Tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers).

You can also use our custom built containers hosted on a Github OCI container registry. Here's a list of [containers](https://github.com/orgs/argonne-lcf/packages) distributed by ALCF staff tailored for Polaris. You can also find instructions to build and run containers [here](https://github.com/argonne-lcf/container-registry)


> **Note:** Currently container build and executions are only supported on the Polaris compute nodes

## Build with a Recipe

You can also build an apptainer container using a recipe file. Detailed instructions for recipe construction are available on the [Apptainer Recipe Page](https://apptainer.org/docs/user/1.2/build_a_container.html#building-containers-from-apptainer-definition-files). You can also check our [apptainer recipe example](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/mpich.def) for building a MPICH container on Polaris.Alternatively, you can just pull `apptainer pull oras://ghcr.io/argonne-lcf/mpich-4:latest` the MPICH image distributed by us and build a container by using the MPICH image as a base image. 






## Running Apptainer on Polaris

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

We move to current working directory and enable network access at run time by setting the proxy. We also load apptainer.

```bash
# SET proxy for internet access
module use /soft/spack/gcc/0.6.1/install/modulefiles/Core
module load apptainer
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
```

This is important for system (Polaris - Cray) mpich to bind to containers mpich. Set the following environment variables

```bash
ADDITIONAL_PATH=/opt/cray/pe/pals/1.2.12/lib
module load cray-mpich-abi
export APPTAINERENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"
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
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN apptainer exec -B /opt -B /var/run/palsd/ $CONTAINER /usr/source/mpi_hello_world

echo Python MPI
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN apptainer exec -B /opt -B /var/run/palsd/ $CONTAINER python3 /usr/source/mpi_hello_world.py
```

The job can be submitted using:

```bash
qsub -v CONTAINER=mpich-4_latest.sif job_submission.sh
```

## Available containers

If you just want to know what containers are available, here you go. 

* For running mpich/MPI containers on Polaris, it can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/mpi/Polaris)
* For running databases on Polaris. It can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/databases)
* For using shpc - that allows for running containers as modules. It can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/shpc)
* Some containers are found in /soft/containers

The latest containers are updated periodically. If you have trouble using containers, or request a newer or a different container please contact ALCF support at `support@alcf.anl.gov`.
 

## Troubleshooting

1. **Permission Denied Error**: One may get a `permission denied` error during the build process, due to a nasty permission setting, quota limitations, or simply due to an unresolved symbolic link. You can try one of the solutions below:
	- Check your quota and delete any unnecessary files. 
	- Clean-up apptainer cache, `~/.apptainer/cache`, and set the apptainer tmp and cache directories as below:
		```bash
 		export APPTAINER_TMPDIR=/tmp/apptainer-tmpdir
 		mkdir $APPTAINER_TMPDIR
 		export APPTAINER_CACHEDIR=/tmp/apptainer-cachedir/
 		mkdir $APPTAINER_CACHEDIR
		``` 
	- Make sure you are not on a directory accessed with a symlink, i.e. check if `pwd` and `pwd -P` returns the same path.
	- If any of the above doesn't work, try running the build in your home directory.

2. **Mapping to rank 0 on all nodes**: This is mainly due to container mpich not binding to system mpich. It is imperative for the container to have mpich which can bind dynamically to system mpich at runtime. Ensure your submission script has the following variables and modules loaded (see below). If this does not resolve, ensure the containers mpich is built with the '--disable-wrapper-rpath' flag. Please refer to this [link](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris) to find examples of building a mpich based container from scratch and running on Polaris.

 ```bash
 ADDITIONAL_PATH=/opt/cray/pe/pals/1.1.7/lib/
 module load cray-mpich-abi
 export APPTAINERENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"
 apptainer exec -B /opt -B /var/run/palsd/
 ```

3. **libmpi.so.40 not found**: This may be due to containers MPI is not binding correctly to the system MPI. You can also try removing .conda & .cache & .local folders from your home directory. Rebuild your container and try again.

4. Containers built with openmpi may not work correctly. Please ensure your container is built with mpich and the base image is of Debian architecture (For e.g. Ubuntu) image.
