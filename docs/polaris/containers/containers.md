# Containers on Polaris

Polaris, equipped with NVIDIA A100 GPUs, leverages container-based workloads for seamless compatibility across NVIDIA systems. This guide provides detailed instructions on using containers on Polaris, including setup, container creation, large-scale execution, and troubleshooting common issues.

## Apptainer Setup

Polaris uses Apptainer (formerly Singularity) for container management. Request a compute node as follows:

```bash
qsub -I -A <PROJECT_NAME> -q debug -l select=1 -l walltime=01:00:00 -l filesystems=home:grand:eagle -l singularity_fakeroot=true # Debug queue for 1 hour
```

After connecting to the compute node, load Apptainer and necessary modules:

```bash
ml use /soft/modulefiles
ml spack-pe-base/0.8.1
ml use /soft/spack/testing/0.8.1/modulefiles
ml apptainer/main
ml load e2fsprogs

export BASE_SCRATCH_DIR=/local/scratch/ # For Polaris
export APPTAINER_TMPDIR=$BASE_SCRATCH_DIR/apptainer-tmpdir
mkdir -p $APPTAINER_TMPDIR

export APPTAINER_CACHEDIR=$BASE_SCRATCH_DIR/apptainer-cachedir
mkdir -p $APPTAINER_CACHEDIR

# For internet access
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128

apptainer version # should return 1.4.0-rc.1+24-g6ae1a25f2
```

Detailed Apptainer documentation is available [here](https://apptainer.org/docs/user/latest/).

## Building Containers from Docker or Argonne GitHub Container Registry

Containers can be built by:
- Creating Dockerfiles locally and publishing to DockerHub, then converting to Apptainer.
- Building directly on ALCF nodes using Apptainer recipe files.

To convert a Docker container to Apptainer on Polaris, use:

```bash
apptainer build --fakeroot pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

Find prebuilt NVIDIA PyTorch containers [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch). TensorFlow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow). Search the full container registry [here](https://catalog.ngc.nvidia.com/containers). Custom containers tailored for Polaris are available in [ALCF's GitHub container registry](https://github.com/argonne-lcf/container-registry/tree/main).

> **Note:** Container build and execution are only supported on Polaris compute nodes.

## Running Containers on Polaris

Use the submission script detailed [here](https://github.com/argonne-lcf/container-registry/blob/main/containers/mpi/Polaris/job_submission.sh). Example job script:

```bash
#!/bin/sh
#PBS -l select=2:system=polaris
#PBS -q debug
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -l filesystems=home:eagle
#PBS -A <project_name>
cd ${PBS_O_WORKDIR}

ml use /soft/modulefiles
ml spack-pe-base/0.8.1
ml use /soft/spack/testing/0.8.1/modulefiles
ml apptainer/main
ml load e2fsprogs

export BASE_SCRATCH_DIR=/local/scratch/
export APPTAINER_TMPDIR=$BASE_SCRATCH_DIR/apptainer-tmpdir
mkdir -p $APPTAINER_TMPDIR
export APPTAINER_CACHEDIR=$BASE_SCRATCH_DIR/apptainer-cachedir
mkdir -p $APPTAINER_CACHEDIR

# Proxy setup for internet access
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128

# Environment variables for MPI
export ADDITIONAL_PATH=/opt/cray/pe/pals/1.2.12/lib
module load cray-mpich-abi
export APPTAINERENV_LD_LIBRARY_PATH="$CRAY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH:$ADDITIONAL_PATH"

# Set MPI ranks
NODES=$(wc -l < $PBS_NODEFILE)
PPN=16
PROCS=$((NODES * PPN))
echo "NUM_OF_NODES=${NODES}, TOTAL_NUM_RANKS=${PROCS}, RANKS_PER_NODE=${PPN}"

# Launch the container
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN apptainer exec --fakeroot -B /opt -B /var/run/palsd/ $CONTAINER /usr/source/mpi_hello_world

# Python example
mpiexec -hostfile $PBS_NODEFILE -n $PROCS -ppn $PPN apptainer exec --fakeroot -B /opt -B /var/run/palsd/ $CONTAINER python3 /usr/source/mpi_hello_world.py
```

Submit jobs using:

```bash
qsub -v CONTAINER=mpich-4_latest.sif job_submission.sh
```
<!-- --8<-- [start:commoncontainerdoc] -->

## Available Containers

- Examples for running MPICH containers can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/mpi/Polaris).
- Examples for running databases can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/databases).
- Examples for using SHPC (containers as modules) can be found [here](https://github.com/argonne-lcf/container-registry/tree/main/containers/shpc).


## Troubleshooting Common Issues

- **Permission Denied:** Check your quota, clean Apptainer cache (`~/.apptainer/cache`), or set directories to local scratch (`/local/scratch/`).
- **MPI Issues:** Ensure MPI compatibility by following [MPI container registry docs](https://github.com/argonne-lcf/container-registry/tree/main).
- **libmpi.so.40 not found:** Use MPICH-compatible base images.
- **Disabled Network Virtualization:** Network virtualization is disabled due to security constraints ([details](https://apptainer.org/docs/user/main/networking.html)).
- **Starter-suid Error:** Always use the `--fakeroot` flag on Polaris compute nodes.

For further assistance, contact ALCF support: `support@alcf.anl.gov`.

<!-- --8<-- [end:commoncontainerdoc] -->