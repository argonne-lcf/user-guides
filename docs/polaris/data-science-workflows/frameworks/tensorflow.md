# TensorFlow on Polaris

TensorFlow is a popular, open-source deep learning framework developed and released by Google.  The [TensorFlow home page](https://www.tensorflow.org/) has more information about TensorFlow, which you can refer to.  For trouble shooting on Polaris, please contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Installation on Polaris

TensorFlow is already pre-installed on Polaris, available in the `conda` module.  To use it from a compute node, please do:

```bash
module load conda
conda activate
```

Then, you can load TensorFlow in `python` as usual (below showing results from the `conda/2022-07-19` module):

```python
>>> import tensorflow as tf
>>> tf.__version__
'2.9.1'
>>>
```

This installation of TensorFlow was built from source and the CUDA libraries it uses are found via the `CUDA_HOME` environment variable (below showing results from the `conda/2022-07-19` module):

```bash
$ echo $CUDA_HOME
/soft/datascience/cuda/cuda_11.5.2_495.29.05_linux
```

If you need to build applications that use this version of TensorFlow and CUDA, we recommend using these cuda libraries to ensure compatibility.  We periodically update the TensorFlow release, though updates will come in the form of new versions of the `conda` module.

TensorFlow is also available through NVIDIA containers that have been translated to Singularity containers.  For more information about containers, please see the [Containers](../containers/containers.md) documentation page.

## TensorFlow Best Practices on Polaris

### Single Node Performance

When running TensorFlow applications, we have found the following practices to be generally, if not universally, useful and encourage you to try some of these techniques to boost performance of your own applications.

1. Use Reduced Precision. Reduced Precision is available on A100 via tensorcores and is supported with TensorFlow operations.  In general, the way to do this is via the `tf.keras.mixed_precision` Policy, as descibed in the [mixed precision documentation](https://www.tensorflow.org/guide/mixed_precision).  If you use a custom training loop (and not `keras.Model.fit`), you will also need to apply [loss scaling](https://www.tensorflow.org/guide/mixed_precision#training_the_model_with_a_custom_training_loop).

2. Use TensorFlow's graph API to improve efficiency of operations.  TensorFlow is, in general, an imperative language but with function decorators like `@tf.function` you can trace functions in your code.  Tracing replaces your python function with a lower-level, semi-compiled TensorFlow Graph. More information about the `tf.function` interface is available [here](https://www.tensorflow.org/api_docs/python/tf/function).  When possible, use jit_compile, but be aware of sharp bits when using `tf.function`: python expressions that aren't tensors are often replaced as constants in the graph, which may or may not be your intention.

3. Use [XLA compilation](https://www.tensorflow.org/xla) on your code.  XLA is the Accelerated Linear Algebra library that is available in tensorFlow and critical in software like JAX.  XLA will compile a `tf.Graph` object, generated with `tf.function` or similar, and perform optimizations like operation-fusion.  XLA can give impressive performance boosts with almost no user changes except to set an environment variable `TF_XLA_FLAGS=--tf_xla_auto_jit=2`.  If your code is complex, or has dynamically sized tensors (tensors where the shape changes every iteration), XLA can be detrimental: the overhead for compiling functions can be large enough to mitigate performance improvements.  XLA is particularly powerful when combined with reduced precision, yielding speedups > 100% in some models.

## Multi-GPU / Multi-Node Scale up

TensorFlow is compatible with scaling up to multiple GPUs per node, and across multiple nodes.  Good scaling performance has been seen up to the entire Polaris system, > 2048 GPUs.  Good performance with tensorFlow has been seen with horovod in particular.  For details, please see the [Horovod documentation](https://horovod.readthedocs.io/en/stable/tensorflow.html).  Some polaris specific details that may be helpful to you:

1. CPU affinity and NCCL settings can improve scaling performance, particularly at the largest scales.  In particular, we encourage users to try their scaling measurements with the following settings:
 - Set the environment variable `NCCL_COLLNET_ENABLE=1`
 - Set the environment varialbe `NCCL_NET_GDR_LEVEL=PHB`
 - Manually set the CPU affinity via mpiexec, such as with `--cpu-bind verbose,list:0,8,16,24
`

2. Horovod works best when you limit the visible devices to only one GPU.  Note that if you import `mpi4py` or `horovod`, and then do something like `os.environ["CUDA_VISIBLE_DEVICES"] = hvd.local_rank()`, it may not actually work!  You must set the `CUDA_VISIBLE_DEVICES` environment variable prior to doing `MPI.COMM_WORLD.init()`, which is done in `horovod.init()` as well as implicitly in `from mpi4py import MPI`.   On Polaris specifically, you can use the environment variable `PMI_LOCAL_RANK` (as well as `PMI_LOCAL_SIZE`) to learn information about the node-local MPI ranks.  

# TensorFlow Dataloaders
Additional information to be provided.
