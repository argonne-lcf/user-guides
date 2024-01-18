# PyTorch on Aurora

PyTorch is a popular, open source deep learning framework developed and 
released by Facebook. The [PyTorch home page](https://pytorch.org/), has more
information about PyTorch, which you can refer to. For troubleshooting on 
Aurora, please contact support@alcf.anl.gov.

## Installation on Aurora

PyTorch is already installed on Aurora with GPU support and available through
the frameworks module. To use it from a compute node, please do:

```
module use /soft/modulefiles/
module load frameworks/2023.12.15.001
```
Then you can load PyTorch as usual, the following is an output from the
`frameworks/2023.12.15.001` module

```
>>> import torch
>>> torch.__version__
'2.0.1a0+cxx11.abi'
```
A simple but useful check could be to use PyTorch to get device information on
a compute node. You can do this the following way:

```python
import torch
import intel_extension_for_pytorch as ipex

print(f"GPU availability: {torch.xpu.is_available()}")
print(f'Number of tiles = {torch.xpu.device_count()}')
current_tile = torch.xpu.current_device()
print(f'Current tile = {current_tile}')
print(f'Curent device ID = {torch.xpu.device(current_tile)}')
print(f'Device name = {torch.xpu.get_device_name(current_tile)}')
```

```
# output of the above code block

GPU availability: True
Number of tiles = 12
Current tile = 0
Curent device ID = <intel_extension_for_pytorch.xpu.device object at 0x1540a9f25790>
Device name = Intel(R) Data Center GPU Max 1550
```
Note that, along with importing the `torch` module, you need to import the
`intel_extension_for_pytorch` module. The default mode in `ipex` for counting
the available devices on a compute node treat each tile as a device, hence the
code block above is expected to output `12`. If you want to get the number of
"cards" as an output, you may declare the following environment variable:

```shell
export IPEX_TILE_AS_DEVICE=0
```
With this environmental variable, we expect the output to be `6` -- the number
of GPUs available on an Aurora compute node. All the `API` calls involving 
`torch.cuda`, should be replaced with `torch.xpu`, as shown in the above 
example.

__Important__: It is highly recommended to import `intel_extension_for_pytorch` 
right after `import torch`, prior to importing other packages, (from 
[Intel's getting started doc](https://github.com/intel/intel-extension-for-pytorch/blob/main/docs/tutorials/getting_started.md)).

Intel extension for PyTorch has been made publicly available as an open-source
project at [Github](https://github.com/intel/intel-extension-for-pytorch)

Please consult the following resources for additional details and useful 
tutorials.

- [PyTorch's webpage for Intel extension](https://pytorch.org/tutorials/recipes/recipes/intel_extension_for_pytorch.html)
- [Intel's Github repository](https://github.com/intel/intel-extension-for-pytorch)
- [Intel's Documentation](https://intel.github.io/intel-extension-for-pytorch/xpu/latest/)

# PyTorch Best Practices on Aurora

## Single Device Performance

To expose one particular device out of the 6 available on a compute node, this
environmental variable should be set

```shell
export ZE_AFFINITY_MASK=0.0,0.1

# The values taken by this variable follows the syntax `Device.Sub-device`
```
In the example given above, an application is targeting the `Device:0` 
and `Sub-devices: 0, 1`, i.e. *the two tiles of the GPU:0*. This is 
particularly important in setting a performance benchmarking baseline.

More information and details are available through the 
[Level Zero Specification Documentation - Affinity Mask](https://spec.oneapi.io/level-zero/latest/core/PROG.html?highlight=affinity#affinity-mask)

## Single Node Performance

When running PyTorch applications, we have found the following practices to be 
generally, if not universally, useful and encourage you to try some of these 
techniques to boost performance of your own applications.

1. Use Reduced Precision. Reduced Precision is available on Intel Max 1550 and 
is supported with PyTorch operations. In general, the way to do this is via the 
PyTorch Automatic Mixed Precision package (AMP), as descibed in the 
[mixed precision documentation](https://pytorch.org/docs/stable/amp.html). In 
PyTorch, users generally need to manage casting and loss scaling manually, 
though context managers and function decorators can provide easy tools to do 
this.

2. PyTorch has a `JIT` module as well as backends to support op fusion, similar 
to TensorFlow's `tf.function` tools. 
Please see [TorchScript](https://pytorch.org/docs/stable/jit.html) for more 
information.

3. `torch.compile` will be available through the next framework release.

## Multi-GPU / Multi-Node Scale Up

PyTorch is compatible with scaling up to multiple GPUs per node, and across 
multiple nodes. Good performance with PyTorch has been seen with both DDP and 
Horovod. For details, please see the 
[Horovod documentation](https://horovod.readthedocs.io/en/stable/pytorch.html) 
or the [Distributed Data Parallel documentation](https://pytorch.org/tutorials/intermediate/ddp_tutorial.html).
Some of the Aurora specific details might be helpful to you:

### Environmental Variables

The following environmental variables should be set on the batch submission 
script (PBSPro script) in the case of attempting to run beyond 16 nodes.

```shell
# This is a fix for running over 16 nodes:
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi

export MPIR_CVAR_ENABLE_GPU=0
# This is to disable certain GPU optimizations like the use of XeLinks between
# GPUs, collectives with GPU-placed data etc., in order to reduce `MPI_Init`
# overheads. Benefits are application dependent.
export CCL_KVS_GET_TIMEOUT=600
```

In order to run an application with `TF32` precision type, one must set the 
following environmental parameter:

```shell
export IPEX_FP32_MATH_MODE=TF32
```
This allows calculations using `TF32` as opposed to the default `FP32`, and 
done through `intel_extension_for_pytorch` module.

### CPU Affinity

The CPU affinity should be set manually through `mpiexec`. You can do this the 
following way:

```shell
export CPU_AFFINITY="verbose,list:0-7,104-111:8-15,112-119:16-23,120-127:24-31,128-135:32-39,136-143:40-47,144-151:52-59,156-163:60-67,164-171:68-75,172-179:76-83,180-187:84-91,188-195:92-99,196-203"
```

### Distributed Training

Distributed training with PyTorch on Aurora is facilitated through both DDP and
Horovod. DDP training is accelerated using oneAPI Collective Communications 
Library Bindings for Pytorch (oneCCL Bindings for Pytorch). 
The extension supports FP32 and BF16 data types. 
More detailed information and examples are available at the 
[Intel oneCCL repo](https://github.com/intel/torch-ccl), formerly known as 
`torch-ccl`.

The key steps in performing distributed training using 
`oneccl_bindings_for_pytorch` are the following:

```python
import os
import torch
import intel_extension_for_pytorch as ipex
import torch.distributed as dist
import torch.nn as nn
from torch.nn.parallel import DistributedDataParallel as DDP
import oneccl_bindings_for_pytorch as torch_ccl

...

# perform the necessary transforms
# set up the training data set
# set up the data loader
# set the master address, ports, world size and ranks through os.environ module

...
# Initialize the process group for distributed training with oneCCL backend

dist.init_process_group(backend='ccl', ... # arguments)

model = YOUR_MODEL().to(device)         # device = 'cpu' or 'xpu:{os.environ['MPI_LOCALRANKID']}'
criterion = torch.nn. ... .to(device)   # Choose a loss function 
optimizer = torch.optim. ...            # Choose an optimizer
# model.train()                         # Optional, model dependent

# Off-load the model to ipex for additional optimization 
model, optimizer = ipex.optimize(model, optimizer=optimizer)

# Initialize DDP with your model for distributed processing
if dist.get_world_size() > 1:
     model = DDP(model, device_ids=[device] if (device != 'cpu') else None)

for ...
    # perform the training loop
```

A detailed example of the full procedure with a toy model is given here:

- [Intel's oneCCL demo](https://github.com/intel/torch-ccl/blob/master/demo/demo.py)



## A Simple Job Script

Below we give an example job script:

```shell
#!/bin/bash -l
#PBS -l select=512                              # selecting 512 Nodes
#PBS -l place=scatter
#PBS -l walltime=1:59:00
#PBS -q EarlyAppAccess                          # a specific queue
#PBS -A Aurora_deployment                       # project allocation
#PBS -l filesystems=home                        # specific filesystem, can be a list separated by :
#PBS -k doe
#PBS -e /home/$USER/path/to/errordir            
#PBS -o /home/$USER/path/to/outdir              # path to `stdout` or `.OU` files
#PBS -j oe                                      # output and error placed in the `stdout` file
#PBS -N a.name.for.the.job

#####################################################################
# This block configures the total number of ranks, discovering
# it from PBS variables.
# 12 Ranks per node, if doing rank/tile
#####################################################################

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12
let NRANKS=${NNODES}*${NRANKS_PER_NODE}

# This is a fix for running over 16 nodes:
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
# These are workaround for a known Cassini overflow issue

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi
# These allow for logging from a specific provider (libfabric)

export MPIR_CVAR_ENABLE_GPU=0 
export CCL_KVS_GET_TIMEOUT=600

#####################################################################
# APPLICATION Variables that make a performance difference
#####################################################################

# Channels last is faster for pytorch, requires code changes!
# More info here:
# https://intel.github.io/intel-extension-for-pytorch/xpu/latest/tutorials/features.html#channels-last
# https://pytorch.org/tutorials/recipes/recipes/intel_extension_for_pytorch.html
DATA_FORMAT="channels_last"

#####################################################################
# FRAMEWORK Variables that make a performance difference 
#####################################################################

# Toggle tf32 on (or don't):
export IPEX_FP32_MATH_MODE=TF32

#####################################################################
# End of perf-adjustment section
#####################################################################

#####################################################################
# Environment set up, using the latest frameworks drop
#####################################################################

module use /soft/modulefiles
module load frameworks/2023.10.15.001

export NUMEXPR_MAX_THREADS=1
# This is to resolve an issue due to a package called "numexpr". 
# It sets the variable 
# 'numexpr.nthreads' to available number of threads by default, in this case 
# to 208. However, the 'NUMEXPR_MAX_THREADS' is also set to 64 as a package 
# default. The solution is to either set the 'NUMEXPR_NUM_THREADS' to less than 
# or equal to '64' or to increase the 'NUMEXPR_MAX_THREADS' to the available 
# number of threads. Both of these variables can be set manually.

#####################################################################
# End of environment setup section
#####################################################################

#####################################################################
# JOB LAUNCH
######################################################################

export CCL_LOG_LEVEL="WARN"
export CPU_AFFINITY="verbose,list:0-7,104-111:8-15,112-119:16-23,120-127:24-31,128-135:32-39,136-143:40-47,144-151:52-59,156-163:60-67,164-171:68-75,172-179:76-83,180-187:84-91,188-195:92-99,196-203"

ulimit -c 0

# Launch the script
mpiexec -np ${NRANKS} -ppn ${NRANKS_PER_NODE} \
--cpu-bind ${CPU_AFFINITY} \
python path/to/application.py

```


