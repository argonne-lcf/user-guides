# Containers on Sophia

Sophia, powered by NVIDIA A100 GPUs, benefits from container-based workloads for seamless compatibility across NVIDIA systems. This guide details the use of containers on Sophia, including custom container creation, large-scale execution, and common pitfalls.

## Apptainer Setup

Sophia employs Apptainer (formerly known as Singularity) for container management. To set up Apptainer, run:

```bash
module use /soft/spack/base/0.7.1/install/modulefiles/Core/
module load apptainer
apptainer version #1.3.3
```

The Apptainer version on Sophia is 1.3.3. Detailed user documentation is available [here](https://apptainer.org/docs/user/1.3/).

## Building from Docker or Argonne GitHub Container Registry

Containers on Sophia can be built by writing Dockerfiles on a local machine and then publishing the container to DockerHub, or by directly building them on an ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing containers, you can pull them from various registries like DockerHub and run them on Sophia.

Since Docker requires root privileges, which users do not have on Sophia, existing Docker containers must be converted to Apptainer. To build a Docker-based container on Sophia, use the following as an example:

```bash
qsub -I -A <Project> -l select=1:ngpus=8:ncpus=256 -l walltime=01:00:00 -l filesystems=home:eagle -l singularity_fakeroot=True -q by-node -k doe
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
module use /soft/spack/base/0.7.1/install/modulefiles/Core/
module load apptainer
apptainer build --fakeroot pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

You can find the latest prebuilt NVIDIA PyTorch containers [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch). The TensorFlow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't typically prebuild the TF-1 containers). You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers). For custom containers tailored for Sophia, visit [ALCF's GitHub container registry](https://github.com/argonne-lcf/container-registry/tree/main).

> **Note:** Currently, container build and executions are only supported on the Sophia compute nodes.