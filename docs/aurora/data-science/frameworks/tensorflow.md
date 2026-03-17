# TensorFlow on Aurora

TensorFlow is a popular, open-source deep learning framework developed and
released by Google. The [TensorFlow home page](https://www.tensorflow.org/)
has more information about the framework. For troubleshooting on Aurora, contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Recent major changes
`TensorFlow` now has its own module, separate from the `frameworks` module. This will change again in the near future, as we are testing a containerized solution that will be made available to users.

## Provided installation

TensorFlow is preinstalled on Aurora and available through the `tensorflow`
module. To use it on a compute node, load the module:
```bash linenums="1"
module load tensorflow
module load mpich/opt/develop-git.6037a7a
```

Then you can `import` TensorFlow as usual. The following output is from the
`tensorflow/2025.2.0` module:

```python linenums="1"
import tensorflow as tf
tf.__version__
```
```text
'2.15.1'
```
This import will fail on login nodes because they do not have XPU devices.

A useful check is to verify that TensorFlow detects the available devices on a
compute node:

```python linenums="1"
tf.config.list_physical_devices()
```
``` { .console .no-copy }
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

Note that `tf.config` returns 12 tiles across 6 GPUs (the GPU resources on an
Aurora compute node), treating each tile as a separate device. You can set the
environment variable `ZE_FLAT_DEVICE_HIERARCHY` to control this behavior, as
described in the
[Level Zero Specification documentation](https://spec.oneapi.io/level-zero/latest/core/PROG.html#device-hierarchy).
This environment variable replaces `ITEX_TILE_AS_DEVICE`, which is deprecated.

Intel Extension for TensorFlow is available as an open-source project on
[GitHub](https://github.com/intel/intel-extension-for-tensorflow).

Consult the following resources for additional details and tutorials:

- [Intel's Documentation](https://intel.github.io/intel-extension-for-tensorflow/latest/get_started.html#documentation)
- [Intel's Examples](https://github.com/intel/intel-extension-for-tensorflow/tree/main/examples)
- [Intel's ITEX Features Guide](https://www.intel.com/content/www/us/en/developer/articles/technical/innovation-of-ai-software-extension-tensorflow.html)
- [Intel's Practice Guide](https://intel.github.io/intel-extension-for-tensorflow/latest/docs/guide/practice_guide.html#gpu-practice-guide)

## TensorFlow best practices on Aurora

### Single device performance

To expose a single tile out of the 12 available (6 GPUs, each with 2 tiles) on
a compute node, set the following environment variable:

```bash
export ZE_AFFINITY_MASK=0 ## Exposing tile 0 on GPU 0
```
More details are available in the
[Level Zero Specification Documentation - Affinity Mask](https://spec.oneapi.io/level-zero/latest/core/PROG.html?highlight=affinity#affinity-mask).

### Single node performance

The following practices have been found to generally improve performance of
TensorFlow applications on Aurora.

#### Reduced precision

Use reduced precision whenever your application allows it. The Intel Max 1550
GPUs support reduced precision through TensorFlow operations. The standard
approach is through the `tf.keras.mixed_precision` policy, as described in the
[mixed precision documentation](https://www.tensorflow.org/guide/mixed_precision).
Intel Extension for TensorFlow is fully compatible with the Keras mixed
precision API and also provides an advanced auto mixed precision feature. You
can set two environment variables to gain the performance benefits of
`FP16`/`BF16` without modifying application code:

```bash
export ITEX_AUTO_MIXED_PRECISION=1
export ITEX_AUTO_MIXED_PRECISION_DATA_TYPE="BFLOAT16" # or "FLOAT16"
```
If you use a custom training loop (and not `keras.Model.fit`), you will also 
need to apply [loss scaling](https://www.tensorflow.org/guide/mixed_precision#training_the_model_with_a_custom_training_loop).

#### TensorFlow's graph API

Use TensorFlow's graph API to improve the efficiency of operations. Although
TensorFlow operates in eager mode by default, the `@tf.function` decorator
traces Python functions and replaces them with lower-level, semi-compiled
TensorFlow graphs. See the
[`tf.function` documentation](https://www.tensorflow.org/api_docs/python/tf/function)
for details. When possible, use `jit_compile`. Be aware that when using
`tf.function`, Python expressions that are not tensors are often replaced with
constants in the graph, which may not be the intended behavior.

An experimental feature enables aggressive kernel fusion through the
[oneDNN Graph API](https://spec.oneapi.io/onednn-graph/latest/introduction.html).
Intel Extension for TensorFlow can offload performance-critical graph
partitions to the oneDNN library for more aggressive graph optimizations.
Enable it by setting the following environment variable:

```bash
export ITEX_ONEDNN_GRAPH=1
```
This feature is experimental and actively under development.

#### `TF32` math mode

The Intel Xe Matrix Extensions (Intel XMX) engines in the Intel Max 1550
Xe-HPC GPUs natively support `TF32` math mode. Enable it through Intel
Extension for TensorFlow by setting the following environment variable:

```bash
export ITEX_FP32_MATH_MODE="TF32"
```

#### XLA compilation (planned/upcoming)

XLA (Accelerated Linear Algebra) is a compiler available in TensorFlow and
central to frameworks like JAX. XLA compiles `tf.Graph` objects generated by
`tf.function` and performs optimizations such as operation fusion. XLA can
deliver significant performance improvements with minimal code changes—simply
set the environment variable `TF_XLA_FLAGS=--tf_xla_auto_jit=2`. However, if
your code is complex or uses dynamically sized tensors (where the shape changes
each iteration), XLA can be detrimental: the compilation overhead may outweigh
the performance gains. XLA is particularly effective when combined with reduced
precision, yielding speedups greater than 100% in some models.

Intel provides initial GPU support for TensorFlow models with XLA acceleration
through
[Intel Extension for OpenXLA](https://github.com/intel/intel-extension-for-openxla).
Full TensorFlow and PyTorch support is planned.

#### A simple example

The following example demonstrates how to use an Intel GPU with TensorFlow:

```python linenums="1" title="intel-xpu-tf-example.py"
import tensorflow as tf   # TensorFlow registers PluggableDevices here.
import intel_extension_for_tensorflow as itex # Intel extensions for GPU usage
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

### Multi-GPU / multi-node scale up

TensorFlow supports scaling across multiple GPUs per node and across multiple
nodes. Good performance has been observed with Horovod in particular. For
details, see the
[Horovod documentation](https://horovod.readthedocs.io/en/stable/tensorflow.html).
The following sections cover Aurora-specific considerations.

#### Environment variables

The following environment variables should be set in the batch submission
script (PBS script) when running on more than 16 nodes.

--8<-- "./docs/aurora/data-science/frameworks/oneCCL.md:onecclenv"

#### CPU affinity

CPU affinity should be set manually through `mpiexec`:

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

When running 12 ranks per node with these settings, the framework uses 4 cores
per rank, with Horovod pinned to one of those 4 cores and oneCCL using a
separate core for better performance. For example, rank 0 would use cores 4--7
for the framework, core 4 for Horovod, and core 42 for oneCCL.

The CPU binding list provides two options. The first assigns one CPU core per
rank; the second assigns 4 CPU cores per rank. In the first oneCCL worker
affinity option, 12 CPU cores are selected (one per rank) from the last 12
cores of each socket, consistent with the oneCCL default core selection
strategy: cores 42--47 belong to the first socket, and cores 94--99 belong to
the second socket. A few cores are left free for other services such as Cray
MPICH and DAOS. The second oneCCL option delegates core selection to the
system; in this case, do not declare or export the `CCL_WORKER_AFFINITY`
variable.

Each workload may perform better with different settings. The criteria for
choosing CPU bindings are:

- **GPU and NIC affinity** -- Bind ranks to cores on the appropriate socket or
    NUMA node.
- **Cache access** -- The optimal binding varies by application and may require
    experimentation.

!!! note

    This setup is a work in progress based on observed performance. The recommended settings are likely to change with new `frameworks` module releases.
    To learn more about CPU binding, see the
    [Running Jobs](https://docs.alcf.anl.gov/aurora/running-jobs-aurora/) page.

#### Distributed training

Distributed training with TensorFlow on Aurora is facilitated through Horovod,
using [Intel Optimization for Horovod](https://github.com/intel/intel-optimization-for-horovod).

The key steps for distributed training are outlined in the following example:

- [TensorFlow examples with Intel Optimization for Horovod](https://github.com/intel/intel-optimization-for-horovod/blob/main/xpu_docs/tensorflow_example.md)

A detailed implementation of the same example is available here:

- [TensorFlow with Keras and Horovod](https://github.com/intel/intel-optimization-for-horovod/blob/main/examples/tensorflow2/tensorflow2_keras_mnist.py)

A suite of detailed and well-documented examples is available in the Intel Optimization for Horovod repository:

- [Distributed Training Example Suite](https://github.com/intel/intel-optimization-for-horovod/tree/main/examples)

#### A simple job script

Below is a simple job script:

```bash linenums="1"
#!/bin/bash -l
#PBS -l select=512                              # selecting 512 Nodes
#PBS -l place=scatter
#PBS -l walltime=1:59:00
#PBS -q prod                                   # a specific queue
#PBS -A <ProjectName>                          # project allocation
#PBS -l filesystems=<fs1:fs2>                   # specific filesystem, can be a list separated by :
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

module load tensorflow
module load mpich/opt/develop-git.6037a7a

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
export CPU_BIND="verbose,list:2-8:10-16:18-24:26-32:34-40:42-48:54-60:62-68:70-76:78-84:86-92:94-100"
HOROVOD_THREAD_AFFINITY="4,12,20,28,36,44,56,64,72,80,88,96"
CCL_WORKER_AFFINITY="1,9,17,25,33,41,53,61,69,77,85,93"

ulimit -c 0

# Launch the script
mpiexec -np ${NRANKS} -ppn ${NRANKS_PER_NODE} \
--cpu-bind ${CPU_BIND} \
python path/to/application.py
```
