# Singularity on Cooley
## Singularity on ALCF Resources
Singularity is a software container solution to enable fine grained control over application environments on a diverse set of hardware systems.  For details on singularity, please see the Singularity [documentation](https://www.sylabs.io/guides/3.0/user-guide/index.html).

## Singularity on Cooley
This page is specific to setting up and running singularity on Cooley.  If you are interested in running singularity on theta, we have detailed Theta specific documentation for singularity.

## Why use Singularity on Cooley?
Singularity provides several advantages on Cooley.  First, singularity provides more control over the software environment you run your code in, meaning you can have fine grained control over packages like tensorflow, pytorch, or other code designed to run on GPUs.  While Cooley is primarily a visualization cluster, it can still be useful to be able to apply it's GPUs to datascience and machine learning challenges.

Second, singularity allows you to "bootstrap" images off of other singularity or docker images.  In particular, this means you can leverage an image built by nvidia with cuda, cudnn, without having to worry about the installation of that software yourself.  Singularity can save you time spent managing your runtime environment and let you focus on your application development.

## How to use Singularity on Cooley
In general, using a singularity container is a simple process.  You can execute a script inside of a container with a command such as:
```
singularity exec /path/to/singularity_container.img command_to_execute
```
If you want to run an interactive container, you can use bash as your executable:
```
singularity exec /path/to/singularity_container.img bash
```
## Using nvidia drivers inside of Singularity
Singularity allows you to use the nvidia device drivers on the host system within the container by passing the --nv option.  On the cooley compute nodes, this allows you to run tensorflow/pytorch on the GPUs inside of the container transparently, with no special set up or modification to your scrips.

## Other Important Runtime Flags
For a full list of available runtime flags, you can look at the output of "singularity run help".  An important flag to know is to bind mount points into the container.  By default, most mount points will not be available in the container, but if you want to use the /lus system from within your container you can have singularity make it available at runtime like this:
```
singularity exec -B /lus:/lus /path/to/singularity_container.img command_to_execute
```
## Using Singularity with MPI
Singularity has builtin support to use MPI and run MPI applications, as long as MPI is built into your container (more on how to do that below).  You can see details of the design on the singularity [docs for HPC](https://apptainer.org/docs-legacy).

In general, singularity must have MPI installed within the container.  When you execute an MPI program with singularity, each rank of your program runs singularity:
```
mpirun -n ${NPROC} singulari​ty exec /path/to/singularity_container.img command_to_execute [executable args]
```
If you want to run 2 ranks per node on Cooley (one for each of the two GPUs on each node) you can pass the list of hosts to mpirun with the -H command:
```
mpirun -n ${NPROC} -H $COBALT_NODEFILE singularity exec /path/to/singularity_container.img command_to_execute [executable args]
```
### Special Considerations for running MPI and Singularity with GPUs on Cooley
Executing MPI code on Cooley nodes can sometimes have extra details due to the communication that happens across GPUs if you use NCCL2.  If your code is hanging unexpectedly when using both GPUs per node, you can try disabling peer to peer communication in NCCL with:
```
export NCCL_P2P_DISABLE=1
```
within the container execution (or pass it into the container with a singularity environment variable).

Additionally, running with both GPUs on a node can cause a crash with CUDA code if there are X processes running.  You can use the nox11 queue, or pass the attribute nox11 to any cobalt job to turn off X processes for your job.

## Building a Singularity Container with TensorFlow or PyTorch for GPU
Building a container for singularity requires either root priveleges on a machine with singularity installed, or the use of [singularity hub](https://singularity-hub.org/) to build your images.  This guide won't go in to the details of how to build your singularity image, but rather contain recipes and snippets that are useful for builds targeted towards Cooley using common packages.

## Bootstrapping a Basic Image
The basic details of your singularity recipe could follow a recipe that looks like this:
```
Bootstrap: docker
From: nvidia/cuda:9.0-cudnn7-devel-centos7

%help
Centos7 with cuda9.0 cudnn7

To start your container simply try
singularity exec THIS_CONTAINER.simg bash

To use GPUs, try
singularity exec --nv THIS_CONTAINER.simg bash


%environment

    # for system
    export CUDA_DEVICE_ORDER=PCI_BUS_ID

    # Add cupti to the path for profiling:
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda/extras/CUPTI/lib64

    source scl_source enable devtoolset-4

%post

    # yum basics
    yum update -y
    yum groupinstall -y "Development Tools"
    yum install -y epel-release
    yum install -y centos-release-scl
    yum install -y devtoolset-4
    yum install -y wget emacs vim
    yum install -y emacs vim openssh-clients zip
    yum install -y python-devel python-pip python-setuptools
    yum install -y hdf5

    # pip basics
    pip --no-cache-dir --disable-pip-version-check install --upgrade setuptools
    pip --no-cache-dir --disable-pip-version-check install future
    pip --no-cache-dir --disable-pip-version-check install 'matplotlib<3.0' # for python2.7
    pip --no-cache-dir --disable-pip-version-check install 'ipython<6.0'    # for python2.7
    pip --no-cache-dir --disable-pip-version-check install 'ipykernel<5.0'  # for python2.7
    pip --no-cache-dir --disable-pip-version-check install numpy wheel zmq six pygments pyyaml cython gputil psutil humanize h5py tqdm scipy seaborn tables
    pip --no-cache-dir --disable-pip-version-check install  pandas scikit-image scikit-learn Pillow opencv-python
    pip --no-cache-dir --disable-pip-version-check install jupyter notebook
```

To add a package like tensorflow, or pytorch, you can install them with pip at the end of the post section as normal.  While building from source is not discouraged, it is much easier to install tensorflow/pytorch using pip:

```
# tensorflow
pip --no-cache-dir --disable-pip-version-check install --upgrade tensorflow-gpu==1.12.0
pip --no-cache-dir --disable-pip-version-check install tensorboard
    
# keras
pip --no-cache-dir --disable-pip-version-check install keras
```

The recipe above has a couple of details worth pointing out.  First, note the "Bootstrap" lines at the very beginning.  This indicates to singularity that this recipe is going to take the centos7 cuda enable docker image from nvidia, and build a singularity container on top of that.  Those two lines get cuda, cudnn, and save a lot of work.

The %help section is for informational purposes only, you can put whatever you like here to help keep organized.

The %environment section runs every time this container activates.  It functions somewhat like a login script in that commands there will be executed prior to whatever command has been passed to the container.  In this recipe, it adds the nvidia profiling tools to the path, and enables centos libraries installed to get gcc 5+.

The %post section runs only during the image build, after the basic set up completes.  It is where you can control and install software (with sudo permission for inside the container).  In this recipe, there are a lot of general purpose tools installed for development purposes, many of which may be unnecessary for your application and you can (and should) prune to what you want if you use this recipe.  Additionally, there are python tools installed with pip that may be useful and again you should prune as needed.

## Building MPI and common MPI tools for your container
MPI needs to be installed inside the container for MPI applications to run within singularity.   To do this, add the following snippet to the %post section (this is nearly identical to the Theta singularity instructions):
```
# install MPICH
wget -q http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
tar xf mpich-3.2.1.tar.gz
rm mpich-3.2.1.tar.gz
cd mpich-3.2.1
# disable the addition of the RPATH to compiled executables
# this allows us to override the MPI libraries to use those
# found via LD_LIBRARY_PATH
./configure --prefix=/usr/local/mpich/install --disable-wrapper-rpath
make -j 4 install
# add to local environment to build pi.c
export PATH=$PATH:/usr/local/mpich//install/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/mpich//install/lib
env | sort
cd ..
rm -rf mpich-3.2.1
```

Additionally, the following lines should be added to the %environment section:    # for MPICH:

```
export PATH=/usr/local/mpich/install/bin/:${PATH}
export LD_LIBRARY_PATH=/usr/local/mpich/install/lib/:${LD_LIBRARY_PATH}
```

If you want to use Nvidia's Collective Communications Library (NCCL2), you can install it:

```
# nccl2
git clone https://github.com/NVIDIA/nccl.git
cd nccl;
make -j src.build
make pkg.redhat.build
rpm -i build/pkg/rpm/x86_64/libnccl* 
cd -
```

mpi4py has worked in containers on Cooley from the pip installation: 

```
pip --no-cache-dir --disable-pip-version-check install mpi4py
```

Lastly, if you want to use horovod for distributed training, the following installation technique has been shown to work with tensorflow (but not pytorch):
```
ldconfig /usr/local/cuda/lib64/stubs
# install Horovod, add other HOROVOD_* environment variables as necessary
HOROVOD_GPU_ALLREDUCE=NCCL HOROVOD_WITH_TENSORFLOW=1 HOROVOD_NCCL_HOME=/nccl/build/ pip install --no-cache-dir horovod

# revert to standard libraries
ldconfig
```

For pytorch, this technique causes segmentation faults at runtime.  Instead, build your container without horovod.  Then, on an interactive node with --nv, and with torch installed into your container, build horovod with the same pip command from inside an interactive shell in your container.  Install it to a location in your area (--user if you like, or --prefix for more control) and add that area to your python path in your run scripts.  While this is a workaround, it works to enable horovod inside of your container for distributed learning.









