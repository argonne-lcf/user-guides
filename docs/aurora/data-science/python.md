# Python on Aurora

!!! warning "Importing Python modules at scale"

	We have system-installed frameworks modules described in this page, which contain common AI/ML packages such as PyTorch and TensorFlow. If a custom package or virtual environment is installed in your own home or project directory, it is **highly** recommended to use the [Copper](../data-management/copper/copper.md) package to help reduce I/O overhead when importing Python modules at large node counts. We have seen that beyond 1000 nodes, importing Python modules from a home or Lustre project directory might be significantly slower, or it may even crash the Lustre file system. Please refer to [Copper](../data-management/copper/copper.md) for detailed instructions on loading custom-installed Python packages using Copper.

	!!! info
	
		If you only use packages from the system installed framework module, [Copper](../data-management/copper/copper.md) is not needed. 

## AI/ML Framework Module

For most Python users on Aurora, a good starting point is the AI/ML framework module. 
The Anaconda environment loaded with this module makes available TensorFlow, Horovod, and PyTorch with Intel extensions and optimizations, among other popular Python and ML packages. 

The following command can be used both from an interactive session on a terminal and within a batch job script to load the latest module
```bash
module load frameworks
```

Please note that:

- The `frameworks` module automatically activates a pre-built `conda` environment which comes with GPU-supported builds of PyTorch and TensorFlow. Both of these frameworks have `Horovod` support for multi-node calculations, as well as PyTorch DDP with [oneCCL](./frameworks/oneCCL.md).
- The `frameworks` module may load a different oneAPI compiler SDK than the default module
- The `frameworks` module is updated approximately every quarter

For more information on PyTorch and TensorFlow on Aurora, please see the respective pages: 

- [PyTorch](./frameworks/pytorch.md)
- [TensorFlow](./frameworks/tensorflow.md)

## Virtual environments via `venv`

While the Anaconda environment automatically loaded with the `frameworks` module contains many 
of the most commonly used Python packages for our users, you may still 
encounter a scenario in which you need to extend the functionality of the 
environment (i.e. install additional packages). In this case, we suggest the use of Python virtual environments. 

!!! warning
	
	There are several alternative approaches for extending or modifying the base Anaconda environments that are generally not recommended on ALCF machines. On Aurora, there are additional performance and functionality pitfalls with those approaches. More detailed information on the alternatives can be seen on the [Polaris Python documentation](../../polaris/data-science/python.md).

Creating and activating a new virtual environment (`venv`) is straightforward. After loading the above module, execute:

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

Any time you wish to use this virtual environment in future shell sessions, be sure to first execute `module load frameworks` before `source /path/to/new/venv/bin/activate`.

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
For example,

```python linenums="1"
import dpctl.tensor as dpt

x_gpu = dpt.arange(100, device=”gpu”)
sqx_gpu = dpt.square(x_gpu) # (1)!
print(sqx_gpu.device) # (2)!
```

1. `dpct.square()` offloads to the "gpu" device
2. `sqx_gpu` is created on the "gpu" device

???+ example "Output"

	``` { .bash .no-copy }
	Device(level_zero:gpu:0)
	```

However, note that operating on arrays created on different devices will raise an exception.

### Accessing the DPEP Packages

Users can access the `dpnp` (v0.16.3) and `dpctl` (v0.18.3) packages by simply loading the latest AI/ML frameworks module with `module load frameworks`.

!!! warning "Accessing numba-dpex on Aurora"
	The current `frameworks` module does not come with the numba-dpex package installed, thus users need to install it separately. 
	This issue will be addressed in the next `frameworks` module, but in the mean time users can either create a new environment and install the dpnp and dpctl packages with
	```bash
	module load frameworks
	module load cmake
    conda create -y --prefix /path/to/dpep_env python=3.10 pip
	conda install -y -c https://software.repos.intel.com/python/conda/linux-64 -c conda-forge --strict-channel-priority dpctl==0.18.3 dpnp==0.16.3
	```
	or clone the base environment with
	```bash
	module load frameworks
	conda create --prefix /path/to/dpep_env --clone /opt/aurora/24.347.0/frameworks/aurora_nre_models_frameworks-2025.0.0
	```
	and then install numba-dpex from source with
	```bash
	conda install -y scikit-build numba==0.59* -c conda-forge
    pip install versioneer
    git clone https://github.com/IntelPython/numba-dpex.git
    cd numba-dpex
    CXX=$(which dpcpp) python setup.py develop
	```

### dpnp
The dpnp library implements the NumPy API using DPC++ and is meant to serve as a drop-in replacement for NumPy, similar to CuPy for CUDA devices.
Therefore, dpnp should be used to port NumPy and CuPy code to Intel GPU, however, please refer to this [comparison table](https://intelpython.github.io/dpnp/reference/comparison.html) to check the current coverage of dpnp API relative to NumPy and CuPy.

Below is a minimal example using dpnp to create and operate on an array allocated on the PVC.

```python linenums="1"
import dpnp as np

x = np.asarray([1, 2, 3]) # (1)!
print("Array x allocated on the device:", x.device)
y = np.sum(x) # (2)!
print("Result y is located on the device:", y.device)
```

1. `np.asarray()` creates an array on the default SYCL device, which is the Intal Max 1550 GPU on Aurora. The queue associated with this array is now carried with `x`, and the pre-compiled kernel for `np.sum(x)` is submitted to that queue. 
2. The result `y` is allocated on the device and is associated with the queue of `x`.

???+ example "Output"

	``` { .bash .no-copy }
	Array x allocated on the device: Device(level_zero:gpu:0)
	Result y is located on the device: Device(level_zero:gpu:0)
	```

All dpnp array creation routines and random number generators have additional optional keyword
arguments (device, queue, and usm_type) which users can leverage to explicitly specify on which device or queue
they want the data to be created along with the USM memory type to be used.

!!! info "Changes after version 0.15.0"
	For dpnp version <= 0.15.0, all dpnp kernels are hard-coded to sync with the CPU after completion (i.e., `event.wait()` is inserted before returning). 
	From dpnp version > 0.15.0, all kernels are run asynchronously, with linear ordering of groups of tasks (similar to CuPy). 
	This results in faster runtime and better GPU utilization. 
	To time kernels with dpnp > 0.15.0, insert `.sycl_queue.wait()` before measuring the end time, for example
	```python linenums="1"
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

```python linenums="1"
import dpctl

dpctl.lsplatform()
num_devices = dpctl.get_num_devices(device_type="gpu") # (1)!
device_list =  dpctl.get_devices(device_type="gpu") # (2)!
print(f"Found {num_devices} GPU devices")
for device in device_list:
    print(f"\t{device}")
print("\nFound CPU devices: ", dpctl.has_cpu_devices()) # (3)!
```

1. Get the number of GPU devices on the node
2. Get the list of GPU devices on the node
3. Check if CPU devices are available on the node

???+ example "Output"

	``` { .bash .no-copy }
    Intel(R) Level-Zero 1.5
	Found 12 GPU devices
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da03430>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da034f0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da035b0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da03530>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da03f70>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8d005770>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8b5a3ab0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8d2467b0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da24ef0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da03fb0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da240b0>
		<dpctl.SyclDevice [backend_type.level_zero, device_type.gpu,  Intel(R) Data Center GPU Max 1550] at 0x154e8da24130>
	
	Found CPU devices:  False
	```

!!! info "Managing GPU and CPU devices on Aurora"
	On Aurora, `ONEAPI_DEVICE_SELECTOR=level_zero:gpu` is set by default, meaning that the GPU are the only devices visible to dpctl. 
	For this reason, `dpctl.has_cpu_devices()` returns `False`. 
	This setting allows dpnp and dpctl to use the GPU as the default SYCL device without needing to explicitly specify it.
	To access the CPU as a SYCL device, set `ONEAPI_DEVICE_SELECTOR=opencl:cpu`.

	In addition, the number of GPU devices visible on each node depends on the `ZE_FLAT_DEVICE_HIERARCHY` environment variable.
	With `ZE_FLAT_DEVICE_HIERARCHY=FLAT` 12 devices are visible (tile as device mode), 
	whereas with `ZE_FLAT_DEVICE_HIERARCHY=COMPOSITE` 6 devices are visible (GPU as device).

The dpctl library contains `dpctl.tensor`, which is a tensor library implemented using DPC++ that follows the Python Array API standard.
We refer the user to the [dpctl.tensor documentation](https://intelpython.github.io/dpctl/latest/api_reference/dpctl/tensor.html) for details on all the array creation, manipulation, and linear algebra functions.  

!!! info "Changes after version 0.17.0"
	Similarly to dpnp, dpctl version > 0.17.0 runs all kernels asynchronously, therefore `.sycl_queue.wait()` must be used to measure execution time on GPU. 

### numba-dpex

Numba-dpex is Intel's Data Parallel Extension for Numba which allows users to apply Numba's JIT compiler and generate performant, parallel code on Intel's GPU.
Its LLVM-based code generator implements a new kernel programming API (kapi) in pure Python that is modeled after the SYCL API.
The example below implements and launches simple vector addition as a range kernel.
Range kernels implement a basic parallel-for calculation that is ideally suited for embarrassingly parallel operations, such as element-wise computations over n-dimensional arrays.

```python linenums="1"
import dpnp
import numba_dpex as dpex
from numba_dpex import kernel_api as kapi

# Data parallel kernel implementation of vector sum
@dpex.kernel # (1)!
def vecadd(item: kapi.Item, a, b, c):
    i = item.get_id(0) # (2)!
    c[i] = a[i] + b[i]

N = 1024 # (3)!
a = dpnp.ones(N)
b = dpnp.ones_like(a)
c = dpnp.zeros_like(a)
dpex.call_kernel(vecadd, dpex.Range(N), a, b, c)
assert dpnp.allclose(c,a+b)
print("Sum completed successfully")
```

1. Decorate the `vecadd` function as a dpex kernel
2. Get the work item
3. Define the number of work items

The `vecadd` function, when decorated as a dpex kernel, is compiled with numba-dpex into a data-parallel function to be executed individually by a set of work items (`#!python item.get_id(0)`). 
Numba-dpex follows the SPMD programming model, wherein each work item runs the function for a subset of the elements of the input arrays.  
The set of work items is defined by the `dpex.Range()` object and the `dpex.call_kernel()` call instructs every work item in the range to execute the `vecadd` kernel for a specific subset of the data.
Numba-dpex also follows the compute-follows-data programming model, meaning that the kernel is run on the same device as the dpnp and dpctl arrays passed as inputs.

Note that the numba-dpex kapi allows for more complex data parallel kernels (e.g., nd-range kernels) and the ability to create device callable functions. 
For these and more features, we refer the users to the [numba-dpex documentation](https://intelpython.github.io/numba-dpex/latest/user_guide/kernel_programming/index.html#).


### DLPack

Thanks to dpctl supporting the Python Array API standard, both dpnp and dpctl provide interoperability with other Python libraries that follow the same standards, such as Numpy and PyTorch, through DLPack.
This allows for zero-copy data access across the Python ecosystem.

An example of using DLPack to pass arrays between dpnp and PyTorch is shown below 
```python linenums="1"
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

???+ example "Output"

	``` { .python-console .no-copy }
	Original PyTorch array: tensor([-2,  1,  2,  3], device='xpu:0')
	dpnp view of PyTorch array: [-2  1  2  3] on device Device(level_zero:gpu:0)
	
	Original dpnp array: [-3  1  2  3] on device Device(level_zero:gpu:0)
	PyTorch view of dpnp array: tensor([-3,  1,  2,  3], device='xpu:0')
	```

!!! info "DLPack notes on Aurora"
	- `ZE_FLAT_DEVICE_HIERARCHY` must be set to `FLAT`
	- Zero-copy interoperability is supported between dpnp, dpctl, and PyTorch on CPU and GPU, and between Numpy as well on CPU only
	- Interoperability between TensorFlow and the other packages is limited on the GPU due to TensorFlow not being compatible with the latest DLPack rules and still requiring the use of `dlcapsules`
	- Numba-dpex does not directly support DLPack, however numba-dpex kernels take as inputs dpnp and dpctl arrays, thus inperoperability between PyTorch and numba-dpex is available through those packages 

