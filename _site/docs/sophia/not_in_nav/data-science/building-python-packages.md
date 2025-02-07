# Building Python Packages

To build Python packages for ThetaGPU, there are two options: build on top of a bare-metal build or build on top of (and within) a Singularity container. Additionally, you can build a new container from NVIDIA's Docker images.

## Build on ThetaGPU compute using Conda

To build on ThetaGPU compute and install your own packages, log in to Theta and then submit an interactive job to log on to a ThetaGPU compute node.

Please see [Running PyTorch with Conda](./dl-frameworks/running-pytorch-conda.md) or [Running TensorFlow with Conda](./dl-frameworks/running-tensorflow-conda.md) for more information.

## Building on top of a container

At the moment, you will need two shells to do this: have one open on a login node (for example, `thetaloginN`), and one open on a compute node (`thetagpuN`). First, start the container in interactive mode:

```bash
singularity exec -B /lus:/lus --nv /lus/theta-fs0/projects/datascience/thetaGPU/containers/pytorch_20.08-py3.sif bash
```

From here, you can create a virtual environment for installation:

```bash
export VENV_LOCATION=/path/to/virtualenv # replace this with your path! 
python -m venv --system-site-packages $VENV_LOCATION
```

**Note:** Sometimes, the `venv` package is available, and if not, you can try `python -m virtualenv`. If neither are available, you can install it in your user directory:

```bash
pip install --user virtualenv
```

and it should work.

Next time you log in, you'll have to start the container, and then run `source $VENV_LOCATION/bin/activate` to re-enable your installed packages.

## Reaching the outside world for pip packages

You'll notice right away when you try to pip install you cannot, because the connection fails. You can, however, go through a proxy server for pip by enabling these variables:

```bash
export HTTP_PROXY=http://theta-proxy.tmi.alcf.anl.gov:3128
export HTTPS_PROXY=https://theta-proxy.tmi.alcf.anl.gov:3128
```

Now, you can pip install your favorite packages: `pip install mpi4py`

## Building custom packages

Most packages (HDF5, for example, or Python packages) can be built and installed into your virtual environment. Here are two common examples that aren't currently part of the PyTorch container that may be useful.

### HDF5

You can find the source code for HDF5 on their website [https://www.hdfgroup.org/downloads/hdf5/source-code](https://www.hdfgroup.org/downloads/hdf5/source-code). When downloaded and un-tarred, `cd` to the directory and run:

```bash
./configure --prefix=$VENV_LOCATION # Add any other configuration arguments 
make -j 64 
make install
```

This should get you HDF5! For example, after this:

```bash
(pytorch_20.08) Singularity> which h5cc 
/home/cadams/ThetaGPU/venvs/pytorch_20.08/bin/h5cc # This is my virtualenv, success!
```

### Horovod

Horovod is useful for distributed training. To use it, you need it enabled within the container.

```bash
git clone https://github.com/horovod/horovod.git 
cd horovod 
git submodule update --init 
python setup.py build 
python setup.py install
```

This should install Horovod within your container.