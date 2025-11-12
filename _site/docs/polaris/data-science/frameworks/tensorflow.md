# TensorFlow on Polaris

TensorFlow is a popular, open-source deep learning framework developed and released by Google. The [TensorFlow home page](https://www.tensorflow.org/) has more information about TensorFlow, which you can refer to. For troubleshooting on Polaris, please contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).

## Installation on Polaris

TensorFlow is already pre-installed on Polaris, available in the `conda` module. To use it from a compute node, please do:

```bash
module load conda
conda activate
```

Then, you can load TensorFlow in `python` as usual (below showing results from the `conda/2024-04-29` module):

```python
>>> import tensorflow as tf
>>> tf.__version__
'2.16.1'
>>>
```

This installation of TensorFlow was built from source, and the CUDA libraries it uses are found via the `CUDA_HOME` environment variable (below showing results from the `conda/2024-04-29` module):

```bash
$ echo $CUDA_HOME
/soft/compilers/cudatoolkit/cuda-12.4.1/
```

If you need to build applications that use this version of TensorFlow and CUDA, we recommend using these CUDA libraries to ensure compatibility. We periodically update the TensorFlow release, though updates will come in the form of new versions of the `conda` module.

TensorFlow is also available through NVIDIA containers that have been translated to Apptainer containers. For more information about containers, please see the [Containers](../../containers/containers.md) documentation page.

## TensorFlow Best Practices on Polaris

### Single Node Performance

When running TensorFlow applications, we have found the following practices to be generally, if not universally, useful and encourage you to try some of these techniques to boost the performance of your own applications.

1. Use Reduced Precision. Reduced Precision is available on A100 via tensor cores and is supported with TensorFlow operations. In general, the way to do this is via the `tf.keras.mixed_precision` Policy, as described in the [mixed precision documentation](https://www.tensorflow.org/guide/mixed_precision). If you use a custom training loop (and not `keras.Model.fit`), you will also need to apply [loss scaling](https://www.tensorflow.org/guide/mixed_precision#training_the_model_with_a_custom_training_loop).

2. Use TensorFlow's graph API to improve the efficiency of operations. TensorFlow is, in general, an imperative language, but with function decorators like `@tf.function`, you can trace functions in your code. Tracing replaces your Python function with a lower-level, semi-compiled TensorFlow Graph. More information about the `tf.function` interface is available [here](https://www.tensorflow.org/api_docs/python/tf/function). When possible, use jit_compile, but be aware of sharp bits when using `tf.function`: Python expressions that aren't tensors are often replaced as constants in the graph, which may or may not be your intention.

3. Use [XLA compilation](https://www.tensorflow.org/xla) on your code. XLA is the Accelerated Linear Algebra library that is available in TensorFlow and critical in software like JAX. XLA will compile a `tf.Graph` object, generated with `tf.function` or similar, and perform optimizations like operation-fusion. XLA can give impressive performance boosts with almost no user changes except to set an environment variable `TF_XLA_FLAGS=--tf_xla_auto_jit=2`. If your code is complex or has dynamically sized tensors (tensors where the shape changes every iteration), XLA can be detrimental: the overhead for compiling functions can be large enough to mitigate performance improvements. XLA is particularly powerful when combined with reduced precision, yielding speedups > 100% in some models.

## Multi-GPU / Multi-Node Scale up

TensorFlow is compatible with scaling up to multiple GPUs per node and across multiple nodes. Good scaling performance has been seen up to the entire Polaris system, > 2048 GPUs. Good performance with TensorFlow has been seen with Horovod in particular. For details, please see the [Horovod documentation](https://horovod.readthedocs.io/en/stable/tensorflow.html). Some Polaris-specific details that may be helpful to you:

--8<-- "./docs/polaris/data-science/frameworks/pytorch.md:scalingsetup"

# TensorFlow Dataloaders

It is crucial to enable multiple workers in the data pipeline for best performance. For details, please refer to [TensorFlow Data Performance Guide](https://www.tensorflow.org/guide/data_performance).