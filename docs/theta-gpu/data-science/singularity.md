# Singularity on ThetaGPU
## Containers on Theta(GPU)
On Theta(GPU), container creation can be achieved by following the [README](https://github.com/argonne-lcf/CompPerfWorkshop/blob/main/03_containers/README.md) using Docker on your local machine, or using a Singularity recipe file and building on a Theta(GPU) worker node.

## Docker on ThetaGPU
If you already have a Docker image you can build a singularity image as follows:
```
singularity build <image_name> docker://<username>/<repo_name>:<tag>
# using tutorial example
singularity build my_image.simg docker://jtchilders/alcf_cwp_example:thetagpu
```

Then you can submit a job to Theta(GPU) using:
```
module load cobalt/cobalt-gpu
qsub -A <project-name> job_submission_thetagpu.sh ./my_image.simg
```

## Building using Singularity Recipes
While building using Docker on your local machine tends to be the easier method. There are sometimes reasons to build in the environment of the supercomputer. In this case, one can build a singularity container on ThetaGPU in an interactive session on a compute (or worker) node. First a recipe file is needed, here is an example [singularity definition](https://github.com/argonne-lcf/CompPerfWorkshop/blob/main/03_containers/ThetaGPU/mpi.def) file.

Detailed directions for recipe construction are available on the [Singularity Recipe Page](https://sylabs.io/guides/2.6/user-guide/container_recipes.html).

## Build Singularity container on ThetaGPU compute
After logging on to Theta login nodes, launch an interactive job using the attrs ```fakeroot=true, pubnet=true``` and specifying the filesystems ```filesystems=home,theta-fs0```.

```
# on Theta login node, must load cobalt-gpu module to submit jobs to ThetaGPU 
module load cobalt/cobalt-gpu 
qsub -I -n 1 -t 01:00:00 -q single-gpu -A <project_name> --attrs fakeroot=true:pubnet=true:filesystems=home,theta-fs0
```

Before building the container make sure the ThetaGPU compute nodes have access to external resources, this is achieved by setting the ```http_proxy and https_proxy``` variables:
```
# setup network proxy to reach outside world 
export http_proxy=http://proxy.tmi.alcf.anl.gov:3128 
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128
```

Now build the container using ```--fakeroot``` where ```<def_filename>.def``` is the definition file we have defined in the example above and ```<image_name>.sif``` is the user defined image file name:
```
# important you run this in the proper path because the file copies in 
# the `%files` section of the recipe uses relative paths on the host. 
cd /path/to/CompPerWorkshop/03_containers/ThetaGPU 
singularity build --fakeroot <image_name>.sif <def_filename>.def
```

## Run Singularity container on ThetaGPU compute
An example job submission script is here: [job_submission_thetagpu.sh](https://github.com/argonne-lcf/CompPerfWorkshop/blob/main/03_containers/ThetaGPU/job_submission_thetagpu.sh).

```
#!/bin/bash -l
#COBALT -n 1
#COBALT -t 00:10:00
#COBALT -q single-gpu
#COBALT --attrs filesystems=home,theta-fs0:pubnet=true

CONTAINER=$1

#Enable network access at run time by setting the proxy.

export http_proxy=http://proxy.tmi.alcf.anl.gov:3128
export https_proxy=http://proxy.tmi.alcf.anl.gov:3128

#Setup our MPI settings, figure out number of nodes NODES and fix number of process per node PPN and multiply to get total MPI ranks PROCS.

NODES=`cat $COBALT_NODEFILE | wc -l`
PPN=8 # GPUs per NODE
PROCS=$((NODES * PPN))
echo NODES=$NODES PPN=$PPN PROCS=$PROCS

#The OpenMPI installed on ThetaGPU must be used for MPI to properly run across nodes. Here the library path is added to SINGULARITYENV_LD_LIBRARY_PATH, which will be used by Singularity to set the container's LD_LIBRARY_PATH and therefore tell our executables where to find the MPI libraries.

MPI_BASE=/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/
export LD_LIBRARY_PATH=$MPI_BASE/lib:$LD_LIBRARY_PATH
export SINGULARITYENV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo mpirun=$(which mpirun)
#Finally the exectuable is launched. Notice on NVidia systems that the singularity exec or singularity run commands must use the --nv flag to pass important libraries/drivers from the host to the container environment.

mpirun -hostfile $COBALT_NODEFILE -n $PROCS -npernode $PPN singularity exec --nv -B $MPI_BASE $CONTAINER /usr/source/mpi_hello_world
```

The job can be submitted using:
```
qsub -A <project-name> job_submission_thetagpu.sh /path/to/my_image.sif 
```

## Pre-existing Images for Deep Learning Using NVIDIA containers
There are several containers on ThetaGPU that will help you get started with deep learning experiments that can efficiently use the A100 GPUs. We have different optimized containers for DL here ```ls /lus/theta-fs0/software/thetagpu/nvidia-containers/```

The [bootstap.def](https://github.com/argonne-lcf/CompPerfWorkshop/blob/main/03_containers/ThetaGPU/bootstrap.def) gives an example of how these containers were created.

The image is bootstrapped from an NVidia image, in this case from a [PyTorch](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/pytorch) build. One can also use the [Tensorflow](https://catalog.ngc.nvidia.com/orgs/nvidia/containers/tensorflow) build. At the time of this writing, the latest tag for the  pyTorch image was ```22.04-py3```, but users should select the version that best suits their needs.

Next we need to install MPI support for cross-node parallel training.
```
Bootstrap: docker 
From: nvcr.io/nvidia/pytorch:22.04-py3
```

Next build the container on a ThetaGPU compute node, following the instructions in the previous section. Then an example job submission script is here: [job_submission_thetagpudl.sh](https://github.com/argonne-lcf/CompPerfWorkshop/blob/main/03_containers/ThetaGPU/job_submission_thetagpudl.sh).

```
%post

    # Install mpi4py
    CC=$(which mpicc) CXX=$(which mpicxx) pip install --no-cache-dir mpi4py

    # Install horovod
    CC=$(which mpicc) CXX=$(which mpicxx) HOROVOD_WITH_TORCH=1 pip install --no-cache-dir horovod
    ```
    
    

