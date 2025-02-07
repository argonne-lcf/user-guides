# Containers on Polaris
Polaris, powered by NVIDIA A100 GPUs, benefits from container-based workloads for seamless compatibility across NVIDIA systems. This guide details the use of containers on Polaris, including custom container creation, large-scale execution, and common pitfalls.

## Apptainer Setup

Polaris employs Apptainer (formerly known as Singularity) for container management. To set up Apptainer, run:

```bash
ml use /soft/modulefiles
ml load spack-pe-base/0.8.1
ml load apptainer
ml load e2fsprogs
apptainer version #1.2.2
```

The Apptainer version on Polaris is 1.2.2. Detailed user documentation is available [here](https://apptainer.org/docs/user/1.2/).

## Building from Docker or Argonne GitHub Container Registry

Containers on Polaris can be built by writing Dockerfiles on a local machine and then publishing the container to DockerHub, or by directly building them on an ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing containers, you can pull them from various registries like DockerHub and run them on Polaris.

Since Docker requires root privileges, which users do not have on Polaris, existing Docker containers must be converted to Apptainer. To build a Docker-based container on Polaris, use the following as an example:

```bash
qsub -I -A <Project> -q debug -l select=1 -l walltime=01:00:00 -l filesystems=home:eagle -l singularity_fakeroot=true
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
ml use /soft/modulefiles
ml load spack-pe-base/0.8.1
ml load apptainer
ml load e2fsprogs
apptainer build --fakeroot pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

You can find the latest prebuilt Nvidia PyTorch containers [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch). The TensorFlow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't typically prebuild the TF-1 containers). You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers). For custom containers tailored for Polaris, visit [ALCF's GitHub container registry](https://github.com/argonne-lcf/container-registry/tree/main).

> **Note:** Currently, container build and executions are only supported on the Polaris compute nodes.

## Running Containers on Polaris

To run a container on Polaris, you can use the submission script described [here](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/job_submission.sh). Below is an explanation of the job submission script.

```bash
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:eagle
#PBS -A <project_name>
cd ${PBS_O_WORKDIR}
echo $CONTAINER
```

We move to the current working directory and enable network access at runtime by setting the proxy. We also load Apptainer.

```bash
# SET proxy for internet access
ml use /soft/modulefiles
ml load spack-pe-base/0.8.1
ml load apptainer
ml load e2fsprogs
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
```

This is important for the system (Polaris - Cray) mpich to bind to the container's mpich. Set the following environment variables:

```bash
ADDITIONAL_PATH=/opt/cray/pe/pals/1.2.12/lib
module load cray-mpich-abi
export APPTAINERENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"
```

Set the number of ranks per node spread as per your scaling requirements:

```bash
# MPI example w/ 16 MPI ranks per node spread evenly across cores
NODES=`wc -l < $PBS_NODEFILE`
PPN=16
PROCS=$((NODES * PPN))
echo "NUM_OF_NODES= ${NODES} TOTAL_NUM_RANKS= ${PROCS} RANKS_PER_NODE= ${PPN}"
```

Finally, launch your script:

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

<!-- --8<-- [start:commoncontainerdoc] -->

## Recipe-Based Container Building

As mentioned earlier, you can build Apptainer containers from recipe files. Instructions are available [here](https://apptainer.org/docs/user/1.2/build_a_container.html#building-containers-from-apptainer-definition-files). See [available containers](#available-containers) for more recipes.

> Note: You can also build custom recipes by bootstrapping from prebuilt images. For example, the first two lines in a recipe to use our custom TensorFlow implementation would be `Bootstrap: oras` followed by `From: ghcr.io/argonne-lcf/tf2-mpich-nvidia-gpu:latest`.

## Available containers

If you just want to know what containers are available, here you go:

* Examples for running MPICH containers can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/mpi/Polaris).

* Examples for running databases can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/databases).

* For using shpc - that allows for running containers as modules. It can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/shpc).

The latest containers are updated periodically. If you have trouble using containers or request a newer or a different container, please contact ALCF support at `support@alcf.anl.gov`.

## Troubleshooting Common Issues

**Permission Denied Error**: If you encounter permission errors during the build:

* Check your quota and delete any unnecessary files.

* Clean up the Apptainer cache, `~/.apptainer/cache`, and set the Apptainer tmp and cache directories as below. If your home directory is full and if you are building your container on a compute node, then set the tmpdir and cachedir to local scratch:

```bash
export BASE_SCRATCH_DIR=/local/scratch/ # FOR POLARIS
#export BASE_SCRATCH_DIR=/raid/scratch/ # FOR SOPHIA
export APPTAINER_TMPDIR=$BASE_SCRATCH_DIR/apptainer-tmpdir
mkdir $APPTAINER_TMPDIR
export APPTAINER_CACHEDIR=$BASE_SCRATCH_DIR/apptainer-cachedir/
mkdir $APPTAINER_CACHEDIR
```

* Make sure you are not in a directory accessed with a symbolic link, i.e., check if `pwd` and `pwd -P` return the same path.

* If any of the above doesn't work, try running the build in your home directory.

**Mapping to rank 0 on all nodes**: Ensure that the container's MPI aligns with the system MPI. For example, follow the additional steps outlined in the [container registry documentation for MPI on Polaris](https://github.com/argonne-lcf/container-registry/tree/main/containers/mpi/Polaris).

**libmpi.so.40 not found**: This can happen if the container's application has an OpenMPI dependency, which is not currently supported on Polaris. It can also spring up if the container's base environment is not a Debian-based architecture such as Ubuntu. Ensure the application has an MPICH implementation as well. Also, try removing `.conda/`, `.cache/`, and `.local/` folders from your home directory and rebuilding the container.

**Disabled Port mapping, user namespace, and [network virtualization]** [Network virtualization](https://apptainer.org/docs/user/main/networking.html) is disabled for the container due to security constraints. See issue [#2533](https://github.com/apptainer/apptainer/issues/2553).

!!! bug "Apptainer instance errors with version 1.3.2"

    Use `nohup` and `&` as an alternative if you want to run Apptainer as a background process. See below for an example of running Postgres as a background process:
    ```bash linenums="1"
     nohup apptainer run 
     -B pgrun:/var/run/postgresql \
     -B pgdata:/var/lib/postgresql/data \
     --env-file pg.env \
     postgres.sing postgres &

     # 3) Capture its PID so we can kill it later
     echo $! > postgres_pid.txt
     echo "Started Postgres in the background with PID $(cat postgres_pid.txt)"

    # 4) Perform whatever work you need while Postgres is running
    #    In this demo, we just sleep for 30 minutes (1800 seconds).
    sleep 1800

    # 5) Kill the background process at the end of the job
    kill "$(cat postgres_pid.txt)"
    rm postgres_pid.txt
    ```
<!-- --8<-- [end:commoncontainerdoc] -->