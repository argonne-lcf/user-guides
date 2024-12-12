# Python on Aurora

> [!IMPORTANT]  
> We have system installed frameworks module which contains common AI/ML packages such as PyTorch and TensorFlow installed. If the users have custom package or virtual environment installed in your own home or project directory, it is **HIGHLY** recommended to use Copper package to help reducing I/O overhead in importing python package at large scale. We have seen that at a scale beyong 1000 nodes, loading python modules from home or lustre project directory might be significantly slow, or even crash the Lustre file system. Please refer to (../data-management/copper/copper.md) on how to use Copper for loading custom installed python packages for details. 

## AI/ML Framework Module

For most Python users on Aurora, a good starting point is the AI/ML framework module. 
The Anaconda environment loaded with this module makes available TensorFlow, Horovod, and PyTorch with Intel extensions and optimizations, among other popular Python and ML packages. 

The following command can be used both from an interactive session on a terminal and within a batch job script to load the latest module
```bash
module load frameworks
```

Please note that:

- The `frameworks` module automatically activates a pre-built `conda` environment (`aurora_nre_models_frameworks-2024.2.1_u1`) which comes with GPU-supported builds of PyTorch and TensorFlow. Both of these frameworks have `Horovod` support for multi-node calculations, as well as PyTorch DDP with [oneCCL](./frameworks/oneCCL.md).
- The `frameworks` module may load a different oneAPI compiler SDK than the default module
- The `frameworks` module is updated approximately every quarter

For more information on PyTorch and TensorFlow on Aurora, please see the respective pages: 

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

The `--system-site-packages` flag will make sure that all the packages included in the `frameworks` module are available after sourcing the `venv`.
If, however, you would like to create an empty `venv`, simply remove this flag.
You can always retroactively change the `--system-site-packages` flag state for 
this virtual environment by editing `venv/pyvenv.cfg` and changing the value 
of `include-system-site-packages` to `true`.

To install a different version of a package that is already installed in the 
base environment, you can use:
```bash
pip install --ignore-installed ... # or -I
```
The base environment is not writable, so it is not possible to remove or 
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

- [dpnp](https://github.com/IntelPython/dpnp) - Data Parallel Extensions for Numpy is a library that implements a subset of Numpy that can be executed on any data parallel device. The subset is a drop-in replacement of core Numpy functions and numerical data types, similar to CuPy for CUDA devices.
- [dpctl](https://github.com/IntelPython/dpctl) - Data Parallel Control library provides utilities for device selection, allocation of data on devices, tensor data structure along with Python Array API Standard implementation, and support for creation of user-defined data-parallel extensions.
- [numba_dpex](https://github.com/IntelPython/numba-dpex) - Data Parallel Extensions for Numba is an extension to Numba compiler for programming data-parallel devices similar to developing programs with Numba for CPU or CUDA devices.

The DPEP packages follow the compute-follows-data programming model, 
meaning that the offload target for a Python library call, or a hand-written kernel using numba-dpex, 
does not need to be specified directly when making the call.
Instead, the offload target is inferred from the input arguments to the library call.
With this programming model, the user only needs to specify the offload target when creating the tensor/ndarray objects.
Note that operating on arrays created on different devices will raise an exception.

For example,
```python
import dpctl.tensor as dpt

x_gpu = dpt.arange(100, device=”gpu”)
sqx_gpu = dpt.square(x_gpu) # square offloads to the “gpu” device
print(sqx_gpu.device) # sqx_gpu is created on the “gpu” device
```

### Installing the DPEP packages
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
conda install -y -c https://software.repos.intel.com/python/conda/ -c conda-forge dpctl dpnp numba-dpex
```

### dpnp
The dpnp library implements the Numpy API using DPC++ and is meant as a drop-in replacement for `numpy`.
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
arguments (device, queue, and usm_type) which users can leverage to explicitly specify on which device or queue
they want the data to be created along with the USM memory type to be used.

Note that for dpnp version <= 0.15.0, all dpnp kernels are hard-coded to sync with the CPU after completion (i.e., `event.wait()` is inserted before returning).
From dpnp version > 0.15.0, all kernels are run asynchronously, with linear ordering of groups of tasks (similar to CuPy). 
This results in faster runtime and better GPU utilization. 
To time kernels with dpnp > 0.15.0, insert `.sycl_queue.wait()` before measuring the end time, for example
```python
import dpnp as np
from time import perf_counter

x = np.random.randn(1000,1000)
tic = perf_counter()
y = np.matmul(x,x)
y.sycl_queue.wait()
print(f"Execution time: {perf_counter() - tic} sec")
```

### dpctl 

The dpctl package lets users access devices supported by the DPC++ SYCL runtime. 
The package exposes features such as device instrospection, execution queue creation, memory allocation, and kernel submission. 
Below are some of the basic device management functions, but more functionality is available on the [dpctl documentation](https://intelpython.github.io/dpctl/latest/index.html).

```python
import dpctl

dpctl.lsplatform()
print("GPU devices:", dpctl.get_devices(device_type="gpu")) # Get the list of all GPU devices
print("Number of GPU devices", dpctl.get_num_devices(device_type="gpu")) # Get the number of GPU devices
print("Has CPU devices?", dpctl.has_cpu_devices()) # Check if there are CPU devices
```

Note that on Aurora, the number of devices visible on each node depends on the `ZE_FLAT_DEVICE_HIERARCHY` environment variable.
With `ZE_FLAT_DEVICE_HIERARCHY=FLAT` 12 devices are visible (tile as device mode), 
whereas with `ZE_FLAT_DEVICE_HIERARCHY=COMPOSITE` 6 devices are visible (GPU as device).
In addition, `ONEAPI_DEVICE_SELECTOR=level_zero:gpu` by default, meaning that `dpctl.has_cpu_devices()` returns `False`. 
This setting allows dpnp and dpctl to use the GPU as the default SYCL device without needing to explicitly specify the device desired.
To access the CPU as a SYCL device, set `ONEAPI_DEVICE_SELECTOR=opencl:cpu`.

The dpctl library contains `dpctl.tensor`, which is a tensor library implemented using DPC++ that follows the Python Array API standard.
We refer the user to the [dpctl.tensor documentation](https://intelpython.github.io/dpctl/latest/api_reference/dpctl/tensor.html) for details on all the array creation, manipulation, and linear algebra functions.  

Similarly to dpnp, dpctl version > 0.17.0 runs all kernels asynchronously, therefore `.sycl_queue.wait()` must be used to measure execution time on GPU. 

### numba-dpex

Numba-dpex is Intel's Data Parallel Extension for Numba which allows users to apply Numba's JIT compiler and generate performant, parallel code on Intel's GPU.
Its LLVM-based code generator implements a new kernel programming API (kapi) in pure Python that is modeled after the SYCL API.
The example below implements and launches simple vector addition as a range kernel.
Range kernels implement a basic parallel-for calculation that is ideally suited for embarrassingly parallel operations, such as element-wise computations over n-dimensional arrays.

```python
import dpnp
import numba_dpex as dpex
from numba_dpex import kernel_api as kapi

# Data parallel kernel implementation of vector sum
@dpex.kernel
def vecadd(item: kapi.Item, a, b, c):
    i = item.get_id(0)
    c[i] = a[i] + b[i]

N = 1024
a = dpnp.ones(N)
b = dpnp.ones_like(a)
c = dpnp.zeros_like(a)
dpex.call_kernel(vecadd, dpex.Range(N), a, b, c)
```

In the example above, the `vecadd` function, when decorated as a dpex kernel, is compiled with numba-dpex into a data-parallel function to be executed individually by a set of work items (`item.get_id(0)`). 
Numba-dpex, therefore, follows the SPMD programming model, wherein each work item runs the function for a subset of the elements of the input arrays.  
The set of work items is defined by the `dpex.Range()` object and the `dpex.call_kernel()` call instructs every work item in the range to execute the `vecadd` kernel for a specific subset of the data.
Numba-dpex also follows the compute-follows-data programming model, meaning that the kernel is run on the same device as the dpnp and dpctl arrays passed as inputs.

Note that the numba-dpex kapi allows for more complex data parallel kernels (e.g., nd-range kernels) and the ability to create device callable functions. 
For these and more features, we refer the users to the [numba-dpex documentation](https://intelpython.github.io/numba-dpex/latest/user_guide/kernel_programming/index.html#).


### DLPack

Thanks to dpctl supporting the Python Array API standard, both dpnp and dpctl provide interoperability with other Python libraries that follow the same standards, such as Numpy and PyTorch, through DLPack.
This allows for zero-copy data access across the Python ecosystem.

An example of using DLPack to pass arrays between dpnp and PyTorch is shown below 
(note that `ZE_FLAT_DEVICE_HIERARCHY=FLAT` must be set if not already)
```python
import dpnp as dp
import torch
import intel_extension_for_pytorch as ipex

t_ary = torch.arange(4).to('xpu') # array [0, 1, 2, 3] on GPU
dp_ary = dp.from_dlpack(t_ary)
t_ary[0] = -2.0 # modify the PyTorch array
print(f'Original PyTorch array: {t_ary}')
print(f'dpnp view of PyTorch array: {dp_ary} on device {dp_ary.device}\n')
del t_ary, dp_ary

dp_ary = dp.arange(4) # array [0, 1, 2, 3] on GPU
t_ary = torch.from_dlpack(dp_ary)
dp_ary[0] = -3.0 # modify the dpnp array
print(f'Original dpnp array: {dp_ary} on device {dp_ary.device}')
print(f'PyTorch view of dpnp array: {t_ary}')
```

which produces the output
``` { .bash .no-copy }
Original PyTorch array: tensor([-2,  1,  2,  3], device='xpu:0')
dpnp view of PyTorch array: [-2  1  2  3] on device Device(level_zero:gpu:0)

Original dpnp array: [-3  1  2  3] on device Device(level_zero:gpu:0)
PyTorch view of dpnp array: tensor([-3,  1,  2,  3], device='xpu:0')
```


On Aurora, please note that:

- Zero-copy interoperability is supported between dpnp, dpctl, and PyTorch on CPU and GPU, and between Numpy as well on CPU only
- Interoperability between TensorFlow and the other packages is limited on the GPU due to TensorFlow not being compatible with the latest DLPack rules and still requiring the use of `dlcapsules`
- Numba-dpex does not directly support DLPack, however numba-dpex kernels take as inputs dpnp and dpctl arrays, thus
inperoperability between PyTorch and numba-dpex is available through those packages 





