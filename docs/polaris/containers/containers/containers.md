# Containers on Polaris

Since Polaris is using nvidia A100 GPUs, there can be portability advantages with other nvidia-based systems if your workloads use containers.  In this document, we'll outline some information about containers on Polaris including how to build custom containers, how to run containers at scale, and common gotchas.

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

### Which singularity?

There used to be a single `singularity` tool, which in 2021 split after some turmoil.  There are now two `singularity`s: one developed by Sylabs, and the other as part of the Linux Foundation.  Both are open source, and the split happened around version 3.10.  The version on Polaris is from [Sylabs](https://sylabs.io/docs/) but for completeness, here is the [Linux Foundation's version](https://github.com/apptainer/apptainer).  Note that the Linux Foundation version is renamed to `apptainer` - different name, roughly the same thing though divergence may happen after 2021's split.

### Why not docker?

Docker containers require root privileges to run, which users do not have on Polaris.  That doesn't mean all your docker containers aren't useful, though.  If you have an existing docker container, you can convert it to singularity pretty easily:

```bash
# Singularity can automatically download the dockerhub hosted container and build it as a singularity container:
$ singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

## Building containers

Building containers is fairly straightforward, though the build times can occasionally make the debugging process tedious if that is needed.  Many containers can be built from existing libraries, or you can build via a recipe, or you can do a hybrid: start with an existing container and build on top of that.

Full documentation of the build process is better referenced from the Sylabs website: [Build a Container](https://docs.sylabs.io/guides/3.8/user-guide/build_a_container.html).

In the docs below, we'll see how to build a container from nvidia (the pytorch container mentioned above), then we'll run it on the compute nodes.  We will see that the default container is missing a package we want, so we'll rebuild a new container based on the old one to add that package.

#TODO this part isn't done yet - have to validate container builds of mpi4py.

By the way - building a container can sometimes use disk resources in your home directory you weren't expecting.  Check `~/.singularity` if you need to clear a cache, and these environment variables sometimes help:
```
# Expecting to do this on a compute node that has /tmp!
export SINGULARITY_TMPDIR=/tmp/singularity-tmpdir
mkdir $SINGULARITY_TMPDIR

SINGULARITY_CACHEDIR=/tmp/singularity-cachedir/
mkdir $SINGULARITY_CACHEDIR
```

If you aren't interested in any of that, just skip to the bottom to see the available containers.

### Default nvidia container:

Build the latest nvidia container if you like:
```bash
# Singularity can automatically download the dockerhub hosted container and build it as a singularity container:
$ singularity build pytorch:22.06-py3.sing docker://nvcr.io/nvidia/pytorch:22.06-py3
```

Note that `latest` here mean when these docs were written, summer 2022.  It may be useful to get a newer container if you need the latest features.  You can find the pytorch container site [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch).  The tensorflow containers are [here](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) (though note that LCF doesn't prebuild the TF-1 containers typically).  You can search the full container registry [here](https://catalog.ngc.nvidia.com/containers)

#### Running the new container

Let's take an interactive node to test the container (`qsub -I -l walltime=1:00:00 -l select=1:system=polaris` for example).

When on the interactive node, re-load the modules you need:
```bash
module load singularity
```

And launch the container with a command like this:

```bash
$ singularity exec --nv -B /lus /soft/containers/pytorch/pytorch-22.06-py3.sing bash
```

A couple things are important to note here:  the `--nv` flag tells singularity that you want to use the nvidia GPUs from inside your container (more info [here](https://docs.sylabs.io/guides/3.8/user-guide/gpu.html?highlight=gpus)).  The `-B` flag is for "bind mount" (more info [here](https://docs.sylabs.io/guides/3.8/user-guide/bind_paths_and_mounts.html)) which tells singularity that you want to be able to access the `/lus` directory from inside your container applications.

Once you start the container, you should be able to do the typical things (`nvidia-smi`, `python`, `nvcc`, etc.).  But this is an interactive shell, and typically running jobs is not an interactive process.  You can launch commands that aren't bash, as well:

```bash
# Do this outside the container...
$ echo "print('Hello from singularity')" >> test.py
$ singularity exec --nv -B /lus /soft/containers/pytorch/pytorch-22.06-py3.sing python test.py
Hello from singularity
```

(You can also use `run` or `shell` instead of `exec` for some uses: check out the (docs)[https://docs.sylabs.io/guides/3.8/user-guide/quick_start.html])

#### What's in the container?

If you are using this pytorch container to run pytorch, you may see this:
```bash
>>> import torch
>>> torch.cuda.is_available()
True
>>> import mpi4py
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ModuleNotFoundError: No module named 'mpi4py'
>>>
```
In otherwords, we've got torch but no mpi4py or horovod.  There are two ways you could address this: install these packages in a way that's visible when you run the container (either in `.local` or a virtualenv) or rebuild the container to have them built in.

Sometimes, you need to pass through environment variables into the container for one reason or another.  A good option for this is to use the `--env` flag to singularity (docs [here](https://docs.sylabs.io/guides/3.8/user-guide/environment_and_metadata.html?highlight=env)).

### Extending a container

The easiest way to extend a container is to build off of one that exists, and write a recipe you can build with `--fakeroot`.  This capability is expected on Polaris very shortly, though isn't here on day 1.

#TODO update the fakeroot section.

## Containers at Scale

And important aspect to using containers is to be able to launch them at scale.  Typically that involves using a single container per GPU, or 4 launches per node.  Containers are meant to communicate seamlessly with MPI, and pass through typically works.  Launch your container with mpirun/mpiexec, and then launch your application in the container as usual:

```bash
mpirun -n ${N_RANKS} -ppn 4 singularity exec [singularity args] ${CONTAINER} python cool_stuff.py
```

The reality is messier.  Containers at scale on Polaris don't cooperate out of the box with MPI, though we're working on it (as of July 2022). 

Typically a build script to do that looks something like this:
```
$ cat recipe.sr

Bootstrap: docker
From: nvcr.io/nvidia/pytorch:22.06-py3


%help

To start your container simply try
singularity exec THIS_CONTAINER.simg bash

To use GPUs, try
singularity exec --nv THIS_CONTAINER.simg bash

%labels
Maintainer coreyjadams

%environment

%post

    # Install mpi4py
    CC=$(which mpicc) CXX=$(which mpicxx) pip install --no-cache-dir mpi4py

    # Install horovod
    CC=$(which mpicc) CXX=$(which mpicxx) HOROVOD_WITH_TORCH=1 pip install --no-cache-dir horovod
```

And you build it like this:
```bash
singularity build --fakeroot custom-torch-container.simg recipe.sr
```

You need network access to download the container from dockerhub (set the usual proxies) and to use fakeroot you need to use an interactive job - the scheduler enables and disables the fakeroot attributes in your job, and you have to explicitly request it:  `--attrs fakeroot=true`.

#TODO Need to validate the fakeroot docs.


## Available containers

If you just want to know what containers are available, here you go.
 
Containers are stored at `/soft/containers/`, with a subfolder for pytorch and tensorflow.  The default nvidia containers "as-is" are available, and soon we will also distribute containers with mpi4p and horovod capabilities that work on Polaris.

The latest containers are updated periodically.  If you have trouble with them, or a new container is available that you need (or want something outside to torch/tensorflow) please contact ALCF support at `support@alcf.anl.gov`.
