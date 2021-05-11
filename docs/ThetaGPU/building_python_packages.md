To build python packages for Theta GPU, there are two options: build on top of a bare-metal build (currently not available, but coming soon) or build on top of (and within) a singularity container.  Additionally, you can build a new container from nvidia's docker images.

# Building on top of a container

At the moment, you will need two shells to do this: have one open on a login node (for example, `thetaloginN`, and one open on a compute node (`thetagpuN`).  First, start the container in interactive mode:

```bash
singularity exec -B /lus:/lus --nv /lus/theta-fs0/projects/datascience/thetaGPU/containers/pytorch_20.08-py3.sif bash
```

From here, you can create a virtual env for installation:
```bash
export VENV_LOCATION=/path/to/virtualenv # replace this with your path!
python -m venv --system-site-packages $VENV_LOCATION
```

Note: sometimes, the `venv` package is available and if not, you can try `python -m virtualenv`.  If neither are available, you can install it in your user directory:
```bash
pip install --user virtualenv
```
and it should work.

Next time you log in, you'll have to start the container, and then run `source $VENV_LOCATION/bin/activate` to re-enable your installed packages.


## Reaching the outside world for `pip` packages

You'll notice right away when you try to `pip` install you can not, because the connection fails.  You can, however, go through a proxy server for pip by enabling these variables:
```bash
export http_proxy=http://theta-proxy.tmi.alcf.anl.gov:3128 
export https_proxy=https://theta-proxy.tmi.alcf.anl.gov:3128
```

Now, you can pip install your favorite packages:
`pip install mpi4py`

## Building custom packages

Most packages (hdf5, for example, or python packages) can be built and installed into your virtual env.  Here are two common examples that aren't currently part of the pytorch container that may be useful.

### HDF5

You can find the source code for hdf5 on their website https://www.hdfgroup.org/downloads/hdf5/source-code/.  When downloaded and un-tarred, `cd` to the directory and run
```bash
./configure --prefix=$VENV_LOCATION # Add any other configuration arguments
make -j 64
make install
```
This should get you hdf5!  For example, after this:
```
(pytorch_20.08) Singularity> which h5cc
/home/cadams/ThetaGPU/venvs/pytorch_20.08/bin/h5cc # This is my virtualenv, success!
```

### Horovod

Horovod is useful for distributed training.  To use it, you need it enabled within the container.

```bash
git clone https://github.com/horovod/horovod.git
cd horovod
git submodule update --init
python setup.py build
python setup.py install
```

This should install horovod within your container.
