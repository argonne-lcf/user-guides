# Containers on Sophia
Sophia, powered by NVIDIA A100 GPUs, benefits from container-based workloads for seamless compatibility across NVIDIA systems. This guide details the use of containers on Sophia, including custom container creation, large-scale execution, and common pitfalls.

## Apptainer Setup

Sophia employs Apptainer (formerly known as Singularity) for container management. To set up Apptainer, run:

```bash
module use /soft/spack/base/0.7.1/install/modulefiles/Core/module load apptainer
apptainer version #1.3.3
```

The Apptainer version on Sophia is 1.3.3. Detailed user documentation is available [here](https://apptainer.org/docs/user/1.3/)

## Building from Docker or Argonne GitHub Container Registry

Containers on Sophia can be built by writing Dockerfiles on a local machine and then publish the container to DockerHub, or by directly building them on ALCF compute node by writing an Apptainer recipe file. If you prefer to use existing containers, you can pull them from various registries like DockerHub and run them on Sophia.

Since Docker requires root privileges, which users do not have on Sophia, existing Docker containers must be converted to Apptainer. To build a Docker-based container on Sophia, use the following as an example:


```bash
qsub -I -A <Project> -l select=1:ngpus=8:ncpus=256 -l walltime=01:00:00 -l filesystems=home:eagle -l singularity_fakeroot=True -q by-node -k doe
export HTTP_PROXY=http://proxy.alcf.anl.gov:3128
export HTTPS_PROXY=http://proxy.alcf.anl.gov:3128
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=http://proxy.alcf.anl.gov:3128
module use /soft/spack/base/0.7.1/install/modulefiles/Core/module load apptainer
apptainer build --fakeroot pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```
You can find the latest prebuilt Nvidia PyTorch containers [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The Tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers). For custom containers tailored for Sophia, visit [ALCF's GitHub container registry](https://github.com/argonne-lcf/container-registry/tree/main)

> **Note:** Currently container build and executions are only supported on the Sophia compute nodes

## Recipe-Based Container Building

As mentioned earlier, you can build Apptainer containers from recipe files. Instructions are available [here](https://apptainer.org/docs/user/1.3/build_a_container.html#building-containers-from-apptainer-definition-files). See [available containers](#available-containers) for more recipes. 

> Note: You can also build custom recipes by bootstrapping from prebuilt images. For e.g the first two lines in a recipe to use our custom Tensorflow implementation would be `Bootstrap: oras` followed by `From: ghcr.io/argonne-lcf/tf2-mpich-nvidia-gpu:latest`

## Troubleshooting Common Issues

- **Permission Denied Error**: If you encounter permission errors during the build
	- Check your quota and delete any unnecessary files. 
	- Clean-up apptainer cache, `~/.apptainer/cache`, and set the apptainer tmp and cache directories as below. If your home directory is full and if you are building your container on a compute node, then set the tmpdir and cachedir to local scratch 
		```bash
			export APPTAINER_TMPDIR=/local/scratch/apptainer-tmpdir
			mkdir $APPTAINER_TMPDIR
			export APPTAINER_CACHEDIR=/local/scratch apptainer-cachedir/
			mkdir $APPTAINER_CACHEDIR
		```
	- Make sure you are not on a directory accessed with a symlink, i.e. check if `pwd` and `pwd -P` returns the same path.
	- If any of the above doesn't work, try running the build in your home directory.

- **Disabled Port mapping, user namespace and [network virtualization]** [Network virtualization](https://apptainer.org/docs/user/main/networking.html) is disabled for the container due to security constraints. See issue [#2533](https://github.com/apptainer/apptainer/issues/2553)
