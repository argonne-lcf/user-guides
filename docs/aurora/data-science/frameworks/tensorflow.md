# TensorFlow on Aurora

TensorFlow is a popular, open-source deep learning framework developed and 
released by Google. The 
[TensorFlow home page](https://www.tensorflow.org/) has more information about 
TensorFlow, which you can refer to. For trouble shooting on Polaris, please 
contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Installation on Aurora

TensorFlow is already pre-installed on Aurora, available in the `frameworks` 
module. To use it from a compute node, please do:

```
module use /soft/modulefiles/
module load frameworks
```

Then you can `import` TensorFlow as usual, the following is an output from the 
`frameworks` module:

```
>>> import tensorflow as tf
>>> tf.__version__
'2.14.1'
```
This import will fail on login nodes because there is no XPU on login nodes. 

A simple but useful check could be to use TensorFlow to get device information 
on a compute node. You can do this the following way:

```
>>> tf.config.list_physical_devices()
[PhysicalDevice(name='/physical_device:CPU:0', device_type='CPU'), 
PhysicalDevice(name='/physical_device:XPU:0', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:1', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:2', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:3', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:4', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:5', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:6', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:7', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:8', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:9', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:10', device_type='XPU'), 
PhysicalDevice(name='/physical_device:XPU:11', device_type='XPU')]
```

Note that, here `tf.config` return 12 tiles of 6 cards (the number of GPU 
resources on an Aurora compute node), and treat each tile as a device. The user
can choose to set the environmental variable `ZE_FLAT_DEVICE_HIERARCHY` with 
appropriate values to achieve desired behavior, as described in the 
[Level Zero Specification documentation](https://spec.oneapi.io/level-zero/latest/core/PROG.html#device-hierarchy).
This environment variable is equivalent to the `ITEX_TILE_AS_DEVICE`, which is
to be deprecated soon.

Intel extension for TensorFLow is has been made publicly available as an 
open-source project at 
[Github](https://github.com/intel/intel-extension-for-tensorflow).

Please consult the following resources for additional details and useful tutorials.

- [Intel's Documentation](https://intel.github.io/intel-extension-for-tensorflow/latest/get_started.html#documentation)
- [Intel's Examples](https://github.com/intel/intel-extension-for-tensorflow/tree/main/examples)
- [Intel's ITEX Features Guide](https://www.intel.com/content/www/us/en/developer/articles/technical/innovation-of-ai-software-extension-tensorflow.html)
- [Intel's Practice Guide](https://intel.github.io/intel-extension-for-tensorflow/latest/docs/guide/practice_guide.html#gpu-practice-guide)

# TensorFlow Best Practices on Aurora

## Single Device Performance

To expose one particular device out of the 6 available on a compute node, 
this environmental variable should be set

```bash
export ZE_AFFINITY_MASK=0.0,0.1

# The values taken by this variable follows the syntax `Device.Sub-device`
```
In the example given above, an application is targeting the 
Device:0 and Sub-devices: 0, 1, i.e. *the two tiles of the GPU:0*. 
This is particularly important in setting a performance benchmarking baseline.

More information and details are available through
[Level Zero Specification Documentation - Affinity Mask](https://spec.oneapi.io/level-zero/latest/core/PROG.html?highlight=affinity#affinity-mask)

## Single Node Performance

When running TensorFlow applications, we have found the following practices to 
be generally, if not universally, useful and encourage you to try some of these 
techniques to boost performance of your own applications.

### Reduced Precision

Use Reduced Precision, whenever the application allows. Reduced Precision is 
available on Intel Max 1550 and is supported with TensorFlow operations. In 
general, the way to do this is via the `tf.keras.mixed_precision` Policy, as 
descibed in the 
[mixed precision documentation](https://www.tensorflow.org/guide/mixed_precision)
Intel's extension for TensorFlow is fully compatible with the Keras mixed 
precision API in TensorFlow. It also provides an advanced auto mixed precision 
feature. For example, you can just set two environment variables to get the 
performance benefit from low-precision data type `FP16`/`BF16` without changing the 
application code.

```bash
export ITEX_AUTO_MIXED_PRECISION=1
export ITEX_AUTO_MIXED_PRECISION_DATA_TYPE="BFLOAT16" # or "FLOAT16"
```
If you use a custom training loop (and not `keras.Model.fit`), you will also 
need to apply [loss scaling](https://www.tensorflow.org/guide/mixed_precision#training_the_model_with_a_custom_training_loop).

### TensorFlow's graph API

Use TensorFlow's graph API to improve efficiency of operations. TensorFlow is, 
in general, an imperative language but with function decorators like 
`@tf.function` you can trace functions in your code. Tracing replaces your 
python function with a lower-level, semi-compiled TensorFlow Graph. More 
information about the `tf.function` interface is available 
[here](https://www.tensorflow.org/api_docs/python/tf/function). 
When possible, use `jit_compile`, but be aware of sharp bits when using 
`tf.function`: python expressions that aren't tensors are often replaced as 
constants in the graph, which may or may not be your intention.

There is an experimental feature, which allows for aggressive fusion of kernels
through 
[oneDNN Graph API](https://spec.oneapi.io/onednn-graph/latest/introduction.html).
Intel's extension for TensorFlow can offload performance critical graph 
partitions to oneDNN library to get more aggressive graph optimizations. It can
be done by setting this environmental variable:

```bash
export ITEX_ONEDNN_GRAPH=1
```
This feature is experimental, and actively under development.

### `TF32` Math Mode

The Intel Xe Matrix Extensions (Intel XMX) engines in Intel Max 1550 Xe-HPC 
GPUs natively support `TF32` math mode. Through intel extension for tensorflow
you can enable it by setting the following environmental variable:

```bash
export ITEX_FP32_MATH_MODE="TF32"
```

### XLA Compilation (Planned/Upcoming)

XLA is the Accelerated Linear Algebra library that is available in TensorFlow 
and critical in software like JAX. XLA will compile a `tf.Graph` object, 
generated with `tf.function` or similar, and perform optimizations like 
operation-fusion. XLA can give impressive performance boosts with almost no 
user changes except to set an environment variable `TF_XLA_FLAGS=--tf_xla_auto_jit=2`. 
If your code is complex, or has dynamically sized tensors (tensors where the 
shape changes every iteration), XLA can be detrimental: the overhead for 
compiling functions can be large enough to mitigate performance improvements. 
XLA is particularly powerful when combined with reduced precision, 
yielding speedups > 100% in some models. 

Intel provides initial intel GPU support for TensorFlow models with XLA 
acceleration through 
[Intel Extension for OpenXLA](https://github.com/intel/intel-extension-for-openxla).
Full TensorFlow and PyTorch support is planned for development.

### A simple example

A simple example on how to use Intel GPU with TensorFlow is the following:

```Python
import tensorflow as tf   # TensorFlow registers PluggableDevices here.
tf.config.list_physical_devices()  # XPU device is visible to TensorFlow.

#Section 1 Run implicitly
a = tf.random.normal(shape=[5], dtype=tf.float32)  # Runs on XPU.
b = tf.nn.relu(a)         # Runs on XPU .

#Section 2 Run with explicit device setting
with tf.device("/XPU:0"):  # Users can also use 'with tf.device' syntax.
  c = tf.nn.relu(a)        # Runs on XPU.
with tf.device("/CPU:0"):
  c = tf.nn.relu(a)        # Runs on CPU.

#Section 3 Run with graph mode
@tf.function  # Defining a tf.function
def run():
  d = tf.random.uniform(shape=[100], dtype=tf.float32)
  e = tf.nn.relu(d)
run()  # PluggableDevices also work with tf.function and graph mode. Runs on XPU
```

## Multi-GPU / Multi-Node Scale Up

TensorFlow is compatible with scaling up to multiple GPUs per node, and across 
multiple nodes. Good performance with tensorFlow has been seen with horovod in 
particular. For details, please see the 
[Horovod documentation](https://horovod.readthedocs.io/en/stable/tensorflow.html).
Some Aurora specific details might be helpful to you.

### Environment Variables

The following environmental variables should be set on the batch submission 
script (PBSPro script) in the case of attempting to run beyond 16 nodes.

```bash
# This is a fix for running over 16 nodes:
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi

export MPIR_CVAR_ENABLE_GPU=0
# This is to disable certain GPU optimizations like the use of XeLinks between
# GPUs, collectives with GPU-placed data etc., in order to reduce `MPI_Init`
# overheads. Benefits are application dependent.
```

**oneCCL optimal setup**

Please refer to [oneCCL](./oneCCL.md) for details. 

```shell
# This is a fix for running over 16 nodes:
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi

export CCL_KVS_GET_TIMEOUT=600

export LD_LIBRARY_PATH=$CCL_ROOT/lib:$LD_LIBRARY_PATH
export CPATH=$CCL_ROOT/include:$CPATH
export LIBRARY_PATH=$CCL_ROOT/lib:$LIBRARY_PATH

export CCL_PROCESS_LAUNCHER=pmix  
export CCL_ATL_TRANSPORT=mpi
export CCL_ALLREDUCE=topo
export CCL_ALLREDUCE_SCALEOUT=rabenseifner  # currently best allreduce algorithm at large scale
export CCL_BCAST=double_tree # currently best bcast algorithm at large scale

export CCL_KVS_MODE=mpi
export CCL_CONFIGURATION_PATH=""
export CCL_CONFIGURATION=cpu_gpu_dpcpp
export CCL_KVS_CONNECTION_TIMEOUT=600 

export CCL_ZE_CACHE_OPEN_IPC_HANDLES_THRESHOLD=1024
export CCL_KVS_USE_MPI_RANKS=1
```

Other optional setup for oneCCL. 

```bash
export FI_MR_ZE_CACHE_MONITOR_ENABLED=0
export FI_MR_CACHE_MONITOR=disabled
export FI_CXI_RX_MATCH_MODE=hybrid
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_DEFAULT_CQ_SIZE=1048576
export FI_CXI_CQ_FILL_PERCENT=30
export MPI_PROVIDER=$FI_PROVIDER
unset MPIR_CVAR_CH4_COLL_SELECTION_TUNING_JSON_FILE
unset MPIR_CVAR_COLL_SELECTION_TUNING_JSON_FILE
export INTELGT_AUTO_ATTACH_DISABLE=1
export PALS_PING_PERIOD=240
export PALS_RPC_TIMEOUT=240
export MPIR_CVAR_GATHERV_INTER_SSEND_MIN_PROCS=-1 #to solve the sync send issue in Horovod seg fault
export CCL_ATL_SYNC_COLL=1 #to avoid potential hang at large scale
export CCL_OP_SYNC=1 #to avoid potential hang at large scale
```

These setup will probably be included in the framework module file in future. But for now, users need to explicitly set these in the submission script. 


### CPU Affinity

The CPU affinity should be set manually through mpiexec. 
You can do this the following way:

```bash
export CPU_BIND="verbose,list:2-4:10-12:18-20:26-28:34-36:42-44:54-56:62-64:70-72:78-80:86-88:94-96"
mpiexec ... --cpu-bind=${CPU_BIND}
```

These bindings should be use along with the following oneCCL and Horovod 
environment variable settings:

```bash
HOROVOD_THREAD_AFFINITY="4,12,20,28,36,44,56,64,72,80,88,96"
CCL_WORKER_AFFINITY="5,13,21,29,37,45,57,65,73,81,89,97"
```

When running 12 ranks per node with these settings the `framework`s use 3 cores, 
with Horovod tightly coupled with the `framework`s using one of the 3 cores, and 
oneCCL using a separate core for better performance, eg. with rank 0 the 
`framework`s would use cores 2,3,4, Horovod would use core 4, and oneCCL would 
use core 5.

Each workload may perform better with different settings. 
The criteria for choosing the cpu bindings are:

- Binding for GPU and NIC affinity – To bind the ranks to cores on the proper 
    socket or NUMA nodes.
- Binding for cache access – This is the part that will change per application 
    and some experimentation is needed.

__Important__: This setup is a work in progress, and based on observed 
performance. The recommended settings are likely to changed with new `framework`
releases.

### Distributed Training 

Distributed training with TensorFlow  on Aurora is facilitated through Horovod,
using [Intel Optimization for Horovod](https://github.com/intel/intel-optimization-for-horovod).

The key steps in performing distributed training are laid out in the following example:

- [Tensorflow examples with Intel Optimization for Horovod](https://github.com/intel/intel-optimization-for-horovod/blob/main/xpu_docs/tensorflow_example.md)

Detailed implementation of the same example is here:

- [TensorFlow with Keras and Horovod](https://github.com/intel/intel-optimization-for-horovod/blob/main/examples/tensorflow2/tensorflow2_keras_mnist.py)

A suite of detailed and well documented examples is part of Intel's optimization for Horovod repository:

- [Distributed Training Example Suite](https://github.com/intel/intel-optimization-for-horovod/tree/main/examples)

### A simple Job Script

Below we give a simple job script:

```bash
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
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
# These are workaround for a known Cassini overflow issue

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi
# These allow for logging from a specific provider (libfabric)

export MPIR_CVAR_ENABLE_GPU=0

#####################################################################
# FRAMEWORK Variables that make a performance difference
#####################################################################

# Toggle tf32 on (or don't):
export ITEX_FP32_MATH_MODE=TF32

#####################################################################
# End of perf-adjustment section
#####################################################################

#####################################################################
# Environment set up, using the latest frameworks drop
#####################################################################

module use /soft/modulefiles
module load frameworks

export NUMEXPR_NUM_THREADS=64
# This is to resolve an issue due to a package called "numexpr".
# It sets the variable
# 'numexpr.nthreads' to available number of threads by default, in this case
# to 208. However, the 'NUMEXPR_MAX_THREADS' is also set to 64 as a package
# default. The solution is to either set the 'NUMEXPR_NUM_THREADS' to less than
# or equal to '64' or to increase the 'NUMEXPR_MAX_THREADS' to the available
# number of threads. Both of these variables can be set manually.


## CCL setup
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20

export FI_LOG_LEVEL=warn
#export FI_LOG_PROV=tcp
export FI_LOG_PROV=cxi

export CCL_KVS_GET_TIMEOUT=600

export LD_LIBRARY_PATH=$CCL_ROOT/lib:$LD_LIBRARY_PATH
export CPATH=$CCL_ROOT/include:$CPATH
export LIBRARY_PATH=$CCL_ROOT/lib:$LIBRARY_PATH

export CCL_PROCESS_LAUNCHER=pmix  
export CCL_ATL_TRANSPORT=mpi
export CCL_ALLREDUCE=topo
export CCL_ALLREDUCE_SCALEOUT=rabenseifner  # currently best allreduce algorithm at large scale
export CCL_BCAST=double_tree # currently best bcast algorithm at large scale

export CCL_KVS_MODE=mpi
export CCL_CONFIGURATION_PATH=""
export CCL_CONFIGURATION=cpu_gpu_dpcpp
export CCL_KVS_CONNECTION_TIMEOUT=600 

export CCL_ZE_CACHE_OPEN_IPC_HANDLES_THRESHOLD=1024
export CCL_KVS_USE_MPI_RANKS=1

#####################################################################
# End of environment setup section
#####################################################################

#####################################################################
# JOB LAUNCH
######################################################################


export CCL_LOG_LEVEL="WARN"
export CPU_BIND="verbose,list:2-4:10-12:18-20:26-28:34-36:42-44:54-56:62-64:70-72:78-80:86-88:94-96"
HOROVOD_THREAD_AFFINITY="4,12,20,28,36,44,56,64,72,80,88,96"
CCL_WORKER_AFFINITY="5,13,21,29,37,45,57,65,73,81,89,97"

ulimit -c 0

# Launch the script
mpiexec -np ${NRANKS} -ppn ${NRANKS_PER_NODE} \
--cpu-bind ${CPU_BIND} \
python path/to/application.py
```

