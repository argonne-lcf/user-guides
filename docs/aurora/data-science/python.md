# Python on Aurora

## AI/ML Framework Module

For most Python users on Aurora, a good starting point is the AI/ML framework module. 
The Anaconda environment loaded with this module makes available TensorFlow, Horovod, and Pytorch with Intel extensions and optimizations, among other popular Python and ML packages. 

The following command can be used both from an interactive session on a terminal or within a batch job script to load the latest module
```bash
module load frameworks
```

Please note that:

- The module automatically activates a pre-built `conda` environment (`aurora_nre_models_frameworks-2024.2.1_u1`) which comes with GPU-supported builds of PyTorch and TensorFlow. Both of these frameworks have `Horovod` support for multi-node calculations, as well as PyTorch DDP with [oneCCL](./frameworks/oneCCL.md).
- The frameworks module may load a different oneAPI compiler SDK than the default module
- The frameworks module is updated approximately every quarter.

For more information on PyTorch and TensorFlow on Aurora, please see their respective pages: 

- [PyTorch](./frameworks/pytorch.md)
- [TensorFlow](./frameworks/tensorflow.md)


## Virtual environments via `venv`

While the Anaconda environment automatically loaded with the `frameworks` module contains many 
of the most commonly used Python packages for our users, you may still 
encounter a scenario in which you need to extend the functionality of the 
environment (i.e. install additional packages).
In this case, we suggest the use of Python virtual environments. 

Creating and activating a new virtual environment (`venv`) is straightforward:

```bash
python3 -m venv /path/to/new/venv --system-site-packages
source /path/to/new/venv/bin/activate
```

The `--system-site-packages` flag will make sure that all the packages included in the `frameworks` module are available after sourcing the venv.
If, however, you would like to create an empty venv, simply remove this flag.
You can always retroactively change the `--system-site-packages` flag state for 
this virtual environment by editing `venv/pyvenv.cfg` and changing the value 
of `include-system-site-packages` to `true`.

To install a different version of a package that is already installed in the 
base environment, you can use:
```bash
pip install --ignore-installed ... # or -I
```
The shared base environment is not writable, so it is impossible to remove or 
uninstall packages from it. The packages installed with the above `pip` command 
should shadow those installed in the base environment.

An alternative, although not recommended, approach to creating a `venv` is to install packages with
```bash
pip install --user ...
```
which will install packages in `$PYTHONUSERBASE/lib/pythonX.Y/site-packages`.
Note that this approach may require the `PATH` environment variable to be modified with `export PATH=$PYTHONUSERBASE/bin:$PATH`.
Cloning the Anaconda environment provided with the `frameworks` module, or using `venv` are both more flexible and transparent methods compared to `--user` installs.


## Intel's Data Parallel Extensions for Python (DPEP)

On Aurora, users can access Intel's Python stack comprising of compilers and libraries for programming heterogenous devices, namely the Data Parallel Extensions for Python (DPEP).
DPEP is composed of three main packages for programming on CPUs and GPUs:
- [dpnp](https://github.com/IntelPython/dpnp) - Data Parallel Extensions for Numpy is a library that implements a subset of Numpy that can be executed on any data parallel device. The subset is a drop-in replacement of core Numpy functions and numerical data types, similar to `cupy` for CUDA devices.
- [numba_dpex](https://github.com/IntelPython/numba-dpex) - Data Parallel Extensions for Numba is an extension to Numba compiler for programming data-parallel devices similar to developing programs with Numba for CPU or CUDA devices.
- [dpctl](https://github.com/IntelPython/dpctl) - Data Parallel Control library provides utilities for device selection, allocation of data on devices, tensor data structure along with Python Array API Standard implementation, and support for creation of user-defined data-parallel extensions.

The DPEP packages follow the "compute follows data" programming model, 
meaning that the offload target for a Python library call, or a hand-written kernel using numba-dpex, 
does not need to be specified directly when making the call.
Instead, the offload target is inferred from the input arguments to a library call.
With this programming model, the user only needs to specify the offload target when creating the tensor/ndarray objects.
Note that operating on arrays created different devices will raise an exception.

For example,
```python
import dpctl.tensor as dpt

x_gpu = dpt.arange(100, device=”gpu”)
sqx_gpu = dpt.square(x_gpu) # square offloads to the “gpu” device
print(sqx_gpu.device) # sqx_gpu is created on the “gpu” device
```

### Install 
For integration with the ML frameworks (PyTorch and TensorFlow with the respective Intel extensions for GPU), 
we recommend using an older `frameworks` module, namely
```bash
module load frameworks/2024.1
```

If integration with the ML frameworks is not required, you can obtain more recent DPEP packages with
```bash
module load frameworks
conda create -y --prefix /path/to/dpep_env python=3.11 pip
conda activate /path/to/dpep_env
conda install -y --channel https://software.repos.intel.com/python/conda/ --channel conda-forge dpctl dpnp numba-dpex
```

### dpnp
The dpnp library implements the NumPy API using DPC++ and is meant as a drop-in replacement for numpy.
Following the minimal example below

```python
import dpnp as np
x = np.asarray([1, 2, 3])
print("Array x allocated on the device:", x.device)
y = np.sum(x)
print("Result y is located on the device:", y.device)
```

`np.asarray()` creates an array on the default SYCL device, which is the Intal Max 1550 GPU on Aurora.
The queue associated with this array is now carried with `x`, and the pre-compiled kernel for `np.sum(x)` is submitted to that queue. 
The result `y` is allocated on the device and is associated with that queue too.

All dpnp array creation routines and random number generators have additional optional keyword
arguments: device, queue, and usm_type, which a user can explicitly specify on which device or queue
they want the tensor data to be created along with the USM memory type to be used.

Add note about synchronization...

### dpctl 

### numba-dpex

### dlpack







