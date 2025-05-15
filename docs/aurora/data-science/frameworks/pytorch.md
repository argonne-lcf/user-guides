# PyTorch on Aurora

PyTorch is a popular, open-source deep learning framework developed and 
released by Facebook. The [PyTorch home page](https://pytorch.org/), has more
information about PyTorch, which you can refer to. For troubleshooting on 
Aurora, please contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).


## Provided Installation

PyTorch is already installed on Aurora with GPU support and available through the [frameworks module](../python.md). 
To use it from a compute node, please load the following modules:

```bash
module load frameworks
```
Then, you can `import` PyTorch in Python as usual (below showing results from the `frameworks/2024.2.1_u1`  module):
``` { .python .no-copy }
>>> import torch
>>> torch.__version__
'2.3.1+cxx11.abi'
```
A simple but useful check could be to use PyTorch to get device information on a compute node. You can do this the following way:

```python linenums="1" title="get-device-info.py"
import torch
import intel_extension_for_pytorch as ipex

print(f"GPU availability: {torch.xpu.is_available()}")
print(f'Number of tiles = {torch.xpu.device_count()}')
current_tile = torch.xpu.current_device()
print(f'Current tile = {current_tile}')
print(f'Current device ID = {torch.xpu.device(current_tile)}')
print(f'Device properties = {torch.xpu.get_device_properties()}')
```

???+ example "Example output:"

    ``` { .python-console .no-copy }
	GPU availability: True
    Number of tiles = 12
    Current tile = 0
    Current device ID = <intel_extension_for_pytorch.xpu.device object at 0x1540a9f25790>
    Device properties = _XpuDeviceProperties(name='Intel(R) Data Center GPU Max 1550', platform_name='Intel(R) Level-Zero', \
	type='gpu', driver_version='1.3.30872', total_memory=65536MB, max_compute_units=448, gpu_eu_count=448, \
	gpu_subslice_count=56, max_work_group_size=1024, max_num_sub_groups=64, sub_group_sizes=[16 32], has_fp16=1, has_fp64=1, \
	has_atomic64=1)
	```

Each Aurora node has 6 GPUs (also called "Devices" or "cards") and each GPU is composed of two tiles (also called "Sub-device"). By default, each tile is mapped to one PyTorch device, giving a total of 12 devices per node in the above output. 

!!! note "`import intel_extension_for_pytorch as ipex`"
    
    Along with importing the `torch` module, you need to import the `intel_extension_for_pytorch` module in order to detect Intel GPUs as `xpu` devices. 

    !!! warning
    
        It is highly recommended to import `intel_extension_for_pytorch` right after `#!python import torch`, prior to importing other packages, (from [Intel's "Getting Started" doc](https://github.com/intel/intel-extension-for-pytorch/blob/main/docs/tutorials/getting_started.md)).


??? info "Using GPU Devices as PyTorch devices"

    By default, each tile is mapped to one PyTorch device, giving a total of 12 devices per node, as seen above. 
    To map a PyTorch device to one particular GPU Device out of the 6 available on a compute node, these 
    environmental variables should be set
    
    ```bash
    export ZE_FLAT_DEVICE_HIERARCHY=COMPOSITE
    export ZE_AFFINITY_MASK=0
    
    # or, equivalently, following the syntax `Device.Sub-device`
    export ZE_AFFINITY_MASK=0.0,0.1
    ```
    In the example given above, an application is targeting the `Device:0` 
    and `Sub-devices: 0, 1`, i.e. *the two tiles of the GPU:0*. This is 
    particularly important in setting a performance benchmarking baseline.
    Setting the above environmental variables after loading the frameworks modules,
    you can check that each PyTorch device is now mapped to one GPU:
    ```python linenums="1"
    import torch
    import intel_extension_for_pytorch as ipex
    torch.xpu.device_count()
    torch.xpu.get_device_properties()
    ```
    
    ???+ example "Example output"
    
        ``` { .bash .no-copy }
    	1
    	_XpuDeviceProperties(name='Intel(R) Data Center GPU Max 1550', platform_name='Intel(R) Level-Zero', type='gpu', driver_version='1.3.30872', total_memory=131072MB, max_compute_units=896, gpu_eu_count=896, gpu_subslice_count=112, max_work_group_size=1024, max_num_sub_groups=64, sub_group_sizes=[16 32], has_fp16=1, has_fp64=1, has_atomic64=1)
    	```
    	
    More information and details are available through the [Level Zero Specification Documentation - Affinity Mask](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/PROG.html?highlight=affinity#affinity-mask)



## Code changes to run PyTorch on Aurora GPUs

[Intel Extension for PyTorch (IPEX)](https://pytorch.org/tutorials/recipes/recipes/intel_extension_for_pytorch.html) is an [open-source project](https://github.com/intel/intel-extension-for-pytorch) that extends PyTorch with optimizations for extra performance boost on Intel CPUs and enables the use of Intel GPUs.

Here we list some common changes that you may need to do to your PyTorch code in order to use Intel GPUs.  
Please consult [Intel's IPEX Documentation](https://intel.github.io/intel-extension-for-pytorch/xpu/latest/tutorials/examples.html) for additional details and useful tutorials.

1. Import the `intel_extension_for_pytorch` **right after** importing `torch`:
   ```python hl_lines="2"
   import torch
   import intel_extension_for_pytorch as ipex
   ```
1. All the `API` calls involving `torch.cuda`, should be replaced with `torch.xpu`. For example:
   ```diff
   - torch.cuda.device_count()
   + torch.xpu.device_count()
   ```
1. When moving tensors and model to GPU, replace `"cuda"` with `"xpu"`. For example:
   ```diff
   - model = model.to("cuda")
   + model = model.to("xpu")
   ```
1. Convert model and loss criterion to `xpu`, and then call `ipex.optimize` for additional performance boost:
   ```python
   device = torch.device('xpu')
   model = model.to(device)
   criterion = criterion.to(device)
   model, optimizer = ipex.optimize(model, optimizer=optimizer)
   ```

!!! tip
    
    A more portable solution to select the appropriate device is the following:
    ```python
    if torch.cuda.is_available():
        device = torch.device('cuda')
    elif torch.xpu.is_available():
        device = torch.device('xpu')
    else: 
        device = torch.device('cpu')
    model = model.to(device)
    ```


## Example: training a PyTorch model on a single GPU tile

Here is a simple code to train a dummy PyTorch model on CPU:

```python linenums="1" title="pytorch_cpu.py"
import torch

torch.manual_seed(0)

src = torch.rand((2048, 1, 512))
tgt = torch.rand((2048, 20, 512))
dataset = torch.utils.data.TensorDataset(src, tgt)
loader = torch.utils.data.DataLoader(dataset, batch_size=32, shuffle=True)

model = torch.nn.Transformer(batch_first=True)
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
criterion = torch.nn.CrossEntropyLoss()
model.train()

for epoch in range(10):
    for source, targets in loader:
        optimizer.zero_grad()

        output = model(source, targets)
        loss = criterion(output, targets)

        loss.backward()
        optimizer.step()
```

And here is the code to train the same model on a single GPU tile on Aurora, with new or modified lines highlighted:

```python linenums="1" title="pytorch_xpu.py" hl_lines="2 3 16-18 22 23"
import torch
import intel_extension_for_pytorch as ipex
device = torch.device('xpu')

torch.manual_seed(0)

src = torch.rand((2048, 1, 512))
tgt = torch.rand((2048, 20, 512))
dataset = torch.utils.data.TensorDataset(src, tgt)
loader = torch.utils.data.DataLoader(dataset, batch_size=32, shuffle=True)

model = torch.nn.Transformer(batch_first=True)
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
criterion = torch.nn.CrossEntropyLoss()
model.train()
model = model.to(device)
criterion = criterion.to(device)
model, optimizer = ipex.optimize(model, optimizer=optimizer)

for epoch in range(10):
    for source, targets in loader:
        source = source.to(device)
        targets = targets.to(device)
        optimizer.zero_grad()

        output = model(source, targets)
        loss = criterion(output, targets)

        loss.backward()
        optimizer.step()
```

Here are the steps to run the above code on Aurora:

1. [Login to Aurora](../../getting-started-on-aurora.md): 
   ```bash
   ssh <username>@aurora.alcf.anl.gov
   ```
1. [Request a one-node interactive job](../../running-jobs-aurora.md#submitting-a-job) for 30 minutes:
   ```bash
   qsub -q debug -A <your_project_name> -l select=1,walltime=30:00 -l filesystems=home:flare -k doe -j oe -I
   ```
1. Copy the above Python script into a file called `pytorch_xpu.py` and make it executable with `#!bash chmod a+x pytorch_xpu.py`.
1. [Load the frameworks module](../python.md#aiml-framework-module):
   ```bash
   module load frameworks
   ```
1. Run the script:
   ```bash
   python pytorch_xpu.py
   ```


## PyTorch Best Practices on Aurora

When running PyTorch applications, we have found the following practices to be 
generally, if not universally, useful and encourage you to try some of these 
techniques to boost performance of your own applications.

1. Use Reduced Precision. Reduced Precision is available on Intel Max 1550 and 
is supported with PyTorch operations. In general, the way to do this is via the 
PyTorch Automatic Mixed Precision package (AMP), as described in the 
[mixed precision documentation](https://pytorch.org/docs/stable/amp.html). In 
PyTorch, users generally need to manage casting and loss scaling manually, 
though context managers and function decorators can provide easy tools to do 
this.

2. PyTorch has a `JIT` module as well as backends to support op fusion, similar to TensorFlow's `tf.function` tools. See [TorchScript](https://pytorch.org/docs/stable/jit.html) for more information.

3. `torch.compile` will be available through the next framework release.

4. In order to run an application with `TF32` precision type, one must set the following environmental parameter: `#!bash export IPEX_FP32_MATH_MODE=TF32`. This allows calculations using `TF32` as opposed to the default `FP32`, and done through `intel_extension_for_pytorch` module.

5. For convolutional neural networks, using `channels_last` (NHWC) memory format gives better performance. More info [here](https://intel.github.io/intel-extension-for-pytorch/xpu/latest/tutorials/features.html#channels-last) and [here](https://intel.github.io/intel-extension-for-pytorch/xpu/latest/tutorials/features/nhwc.html)


## Distributed Training on multiple GPUs

Distributed training with PyTorch on Aurora is facilitated through both [Distributed Data Parallel (DDP)](https://pytorch.org/tutorials/intermediate/ddp_tutorial.html) and [Horovod](https://horovod.readthedocs.io/en/stable/pytorch.html), with comparable performance. 
We recommend using native PyTorch DDP to perform Data Parallel training on Aurora. 


### Distributed Data Parallel (DDP)

DDP training is accelerated using oneAPI Collective Communications Library Bindings for Pytorch (`oneccl_bindings_for_pytorch`). The extension supports FP32 and BF16 data types. 
More detailed information and examples are available at the [Intel oneCCL repo](https://github.com/intel/torch-ccl), formerly known as `torch-ccl`.

#### Code changes to train on multiple GPUs using DDP

The key steps in performing distributed training are:

1. Load the `oneccl_bindings_for_pytorch` module, which enables efficient distributed deep learning training in PyTorch using Intel's oneCCL library, implementing collectives like `allreduce`, `allgather`, `alltoall`.
1. Initialize PyTorch's `DistributedDataParallel`
1. Use `DistributedSampler` to partition the training data among the ranks
1. Pin each rank to a GPU
1. Wrap the model in DDP to keep it in sync across the ranks 
1. Rescale the learning rate
1. Use `set_epoch` for shuffling data across epochs

Here is the code to train the [same dummy PyTorch model](#example-training-a-pytorch-model-on-a-single-gpu-tile) on multiple GPUs, where new or modified lines have been highlighted:

```python linenums="1" title="pytorch_ddp.py" hl_lines="1 2 4 6 8-24 31-33 36 37 43 44 47 48 61 62"
from mpi4py import MPI
import os, socket
import torch
from torch.nn.parallel import DistributedDataParallel as DDP
import intel_extension_for_pytorch as ipex
import oneccl_bindings_for_pytorch as torch_ccl

# DDP: Set environmental variables used by PyTorch
SIZE = MPI.COMM_WORLD.Get_size()
RANK = MPI.COMM_WORLD.Get_rank()
LOCAL_RANK = os.environ.get('PALS_LOCAL_RANKID')
os.environ['RANK'] = str(RANK)
os.environ['WORLD_SIZE'] = str(SIZE)
MASTER_ADDR = socket.gethostname() if RANK == 0 else None
MASTER_ADDR = MPI.COMM_WORLD.bcast(MASTER_ADDR, root=0)
os.environ['MASTER_ADDR'] = f"{MASTER_ADDR}.hsn.cm.aurora.alcf.anl.gov"
os.environ['MASTER_PORT'] = str(2345)
print(f"DDP: Hi from rank {RANK} of {SIZE} with local rank {LOCAL_RANK}. {MASTER_ADDR}")

# DDP: initialize distributed communication with nccl backend
torch.distributed.init_process_group(backend='ccl', init_method='env://', rank=int(RANK), world_size=int(SIZE))

# DDP: pin GPU to local rank.
torch.xpu.set_device(int(LOCAL_RANK))
device = torch.device('xpu')
torch.manual_seed(0)

src = torch.rand((2048, 1, 512))
tgt = torch.rand((2048, 20, 512))
dataset = torch.utils.data.TensorDataset(src, tgt)
# DDP: use DistributedSampler to partition the training data
sampler = torch.utils.data.distributed.DistributedSampler(dataset, shuffle=True, num_replicas=SIZE, rank=RANK, seed=0)
loader = torch.utils.data.DataLoader(dataset, sampler=sampler, batch_size=32)

model = torch.nn.Transformer(batch_first=True)
# DDP: scale learning rate by the number of GPUs.
optimizer = torch.optim.Adam(model.parameters(), lr=(0.001*SIZE))
criterion = torch.nn.CrossEntropyLoss()
model.train()
model = model.to(device)
criterion = criterion.to(device)
model, optimizer = ipex.optimize(model, optimizer=optimizer)
# DDP: wrap the model in DDP
model = DDP(model)

for epoch in range(10):
    # DDP: set epoch to sampler for shuffling
    sampler.set_epoch(epoch)

    for source, targets in loader:
        source = source.to(device)
        targets = targets.to(device)
        optimizer.zero_grad()

        output = model(source, targets)
        loss = criterion(output, targets)

        loss.backward()
        optimizer.step()

# DDP: cleanup
torch.distributed.destroy_process_group()
```

Here are the steps to run the above code on Aurora:

1. [Login to Aurora](../../getting-started-on-aurora.md): 
   ```bash
   ssh <username>@aurora.alcf.anl.gov
   ```
1. [Request an interactive job on two nodes](../../running-jobs-aurora.md#submitting-a-job) for 30 minutes:
   ```bash
   qsub -q debug -A <your_project_name> -l select=2,walltime=30:00 -l filesystems=home:flare -k doe -j oe -I
   ```
1. Copy the above Python script into a file called `pytorch_ddp.py` and make it executable with `#!bash chmod a+x pytorch_ddp.py`.
1. [Load the frameworks module](../python.md#aiml-framework-module):
   ```bash
   module load frameworks
   ```
1. Run the script on 24 tiles, 12 per node:
   ```bash
   mpiexec -n 24 -ppn 12 python pytorch_ddp.py
   ```

!!! warning "Settings for training beyond 16 nodes"

    When training at medium and large scales, we recommend using the module `frameworks_optimized`, which provides an [optimized setup](./oneCCL.md) based on observed performance.
    To use this optimized setup, the last two steps of the above instructions should be replaced with the following ones:
    
    1. Load the `frameworks_optimized` module:
       ```bash
       module use /soft/datascience/frameworks_optimized/
       module load frameworks_optimized
       ```
    1. Run the script on 24 tiles, 12 per node:
       ```bash
       mpiexec -n 24 -ppn 12 --cpu-bind=${CPU_BIND} python pytorch_ddp.py
       ```

    ??? tip "Setting the CPU Affinity"
    
        The [CPU affinity](../../running-jobs-aurora.md#mpi-rank-and-thread-binding-to-cores-and-gpus) can be set manually through mpiexec. 
        You can do this the following way (after having loaded all needed modules):
        
        ```bash
        ## Option 1
        export CPU_BIND="list:4:9:14:19:20:25:56:61:66:71:74:79" # 12 ppn to 12 cores

        ## Option 2
        export CPU_BIND="verbose,list:4-7:8-11:12-15:16-19:20-23:24-27:56-59:60-63:64-67:68-71:72-75:76-79" # 12 ppn with each rank having 4 cores
        mpiexec ... --cpu-bind=${CPU_BIND}
        ```

        These bindings should be used along with the following oneCCL and Horovod 
        environment variable settings:

        ```bash
        HOROVOD_THREAD_AFFINITY="4,8,12,16,20,24,56,60,64,68,72,76"

        ## Option 1
        CCL_WORKER_AFFINITY="42,43,44,45,46,47,94,95,96,97,98,99"

        ## Option 2
        unset CCL_WORKER_AFFINITY  # Default will pick up from the last 24 cores even if you didn't specify these in the binding.
        ```

        When running 12 ranks per node with these settings the `framework`s use 4 cores, 
        with Horovod tightly coupled with the `framework`s using one of the 4 cores, and 
        oneCCL using a separate core for better performance, e.g. with rank 0 the 
        `framework`s would use cores 4-7, Horovod would use core 4, and oneCCL would 
        use core 42.

        In the provided CPU binding list we have provided two options. First one is
        based on one CPU core per rank. In the second option, we assign 4 CPU cores per
        rank. In the first oneCCL worker affinity option we pick 12 CPU cores, one per
        rank. Notice that, these cores are picked out from the last 12 cores of each
        socket (CPU), aligned with oneCCL default core picking strategy. 42-47 belongs
        to the first socket, and 94-99 belongs to the second socket. We leave a few
        cores free, in case, the user may want to use other services like copper and
        DAOS along with their application. The second oneCCL option is to delegate
        task of picking cores to the system. In this case, the user should not declare
        or export the `CCL_WORKER_AFFINITY` variable.

        Each workload may perform better with different settings. 
        The criteria for choosing the cpu bindings are:
        
        - Binding for GPU and NIC affinity – To bind the ranks to cores on the proper 
            socket or NUMA nodes.
        - Binding for cache access – This is the part that will change per application 
            and some experimentation is needed.
        
        __Important__: This setup is a work in progress, and based on observed 
        performance. The recommended settings are likely to changed with new `framework`
        releases.


### Distributed Training with Multiple CCSs

The Intel PVC GPUs contain 4 Compute Command Streamers (CCSs) on each tile, which can be used to group Execution Units (EUs) into common pools. 
These pools can then be accessed by separate processes thereby enabling distributed training with multiple MPI processes per tile. 
This feature on PVC is similar to MPS on NVIDIA GPUs 
and can be beneficial for increasing computational throughput when training or performing inference with smaller models which do not require the entire memory of a PVC tile.
For more information, see the section on using multiple CCSs under the [Running Jobs on Aurora](../../running-jobs-aurora.md) page.

For both DDP and Horovod, distributed training with multiple CCSs can be enabled programmatically within the user code by explicitly setting the `xpu` device in PyTorch, for example

```python linenums="1"
import os
from argparse import ArgumentParser
import torch
import intel_extension_for_pytorch
import oneccl_bindings_for_pytorch

parser = ArgumentParser(description='CCS Test')
parser.add_argument('--ppd', default=1, type=int, choices=[1,2,4], 
                    help='Number of MPI processes per GPU device') # (1)!
args = parser.parse_args()

local_rank = int(os.environ.get('PALS_LOCAL_RANKID'))
if torch.xpu.is_available():
    xpu_id = local_rank//args.ppd if torch.xpu.device_count()>1 else 0
    assert xpu_id>=0 and xpu_id<torch.xpu.device_count(), \
           f"Assert failed: xpu_id={xpu_id} and {torch.xpu.device_count()} available devices"
    torch.xpu.set_device(xpu_id)
```

1. PVC GPU allow the use of 1, 2 or 4 CCSs on each tile

and then adding the proper environment variables and `mpiexec` settings in the run script. 
For example, to run distributed training with 48 MPI processes per node exposing 4 CCSs per tile, set

```bash linenums="1"
export ZEX_NUMBER_OF_CCS=0:4,1:4,2:4,3:4,4:4,5:4,6:4,7:4,8:4,9:4,10:4,11:4
BIND_LIST="1:2:4:6:8:10:12:14:16:18:20:22:24:26:28:30:32:34:36:38:40:42:44:46:53:54:56:58:60:62:64:66:68:70:72:74:76:78:80:82:84:86:88:90:92:94:96:98"
mpiexec --pmi=pmix --envall -n 48 --ppn 48 \
    --cpu-bind=verbose,list:${BIND_LIST} \
    python training_script.py --ppd=4
```

Alternatively, users can use the following modified GPU affinity script in their `mpiexec` command in order to bind multiple MPI processes to each tile by setting `ZE_AFFINITY_MASK`

```bash linenums="1" title="gpu_affinity_ccs.sh"
#!/bin/bash

num_ccs=$1 # (1)!
shift
gpu_id=$(( PALS_LOCAL_RANKID / num_ccs ))
export ZE_AFFINITY_MASK=$gpu_id
exec "$@"
```

1. Note that the script takes the number of CCSs exposed as a command line argument

!!! info "Checking PVC usage with `xpu-smi`"
	Users are invited to check correct placement of the MPI ranks on the different tiles by connecting to the compute node being used and executing 
	```bash
	module load xpu-smi
	watch -n 0.1 xpu-smi stats -d <GPU_ID> # (1)!
    ```

	1. In this case, GPU_ID refers to the 6 GPU on each node, not an individual tile

	and checking the GPU and memory utilization of both tiles.

!!! warning "Multiple CCSs and oneCCL"
	- When performing distributed training exposing multiple CCSs, the collective communications with the oneCCL backend are delegated to the CPU. This is done in the background by oneCCL, so no change to the users' code is required to move data between host and device, however it may impact the performance of the collectives at scale.
	- When using PyTorch DDP, the model must be offloaded to the XPU device after calling the `DDP()` wrapper on the model to avoid hangs.
