## Machine Learning Tools on Theta

The ALCF is working to support scalable machine learning on Theta. Our focus has been supporting TensorFlow on Theta as it has broad support from Cray & Intel giving large performance boosts on the Intel KNL architecture versus other frameworks. 

We support two installations of TensorFlow, one via the [Conda environment](conda.md) and one via a custom Cray plugin environment. We also provide easy to use datascience modules for mpi4py, TensorFlow, Keras, PyTorch, and Horovod.

## Generic Environment Settings

First we'll mention some generic settings to play with while doing TensorFlow training on Theta.

The Google TensorFlow guide describes these variables here: [https://www.tensorflow.org/performance/performance_guide](https://www.tensorflow.org/tfx/serving/performance)

In your batch submit script use the following:
- ```export OMP_NUM_THREADS=62``` This setting should be set to the number of physical cores, although, our local Cray expert suggested using 62.
- ```export KMP_BLOCKTIME=0``` Sets the time, in milliseconds, that a thread should wait, after completing the execution of a parallel region, before sleeping.
- ```export KMP_AFFINITY="granularity=fine,compact,1,0"```  Enables the run-time library to bind threads to physical processing units.

In addition, TensorFlow has the following internal settings that should be used to optimize performance:
- **intra_op_parallelism_threads:** Setting this equal to the number of physical cores is recommended. Setting the value to 0, which is the default and will result in the value being set to the number of logical cores, is an option to try for some architectures. This value and OMP_NUM_THREADS should be equal.
- **inter_op_parallelism_threads:** Setting this equal to the number of sockets is recommended. Setting the value to 0, which is the default, results in the value being set to the number of logical cores.

This can be added to your model using this code:
```
config = tf.ConfigProto() 
config.allow_soft_placement = True 
config.intra_op_parallelism_threads = FLAGS.num_intra_threads 
config.inter_op_parallelism_threads = FLAGS.num_inter_threads 
tf.session(config=config)
```
## TensorFlow via Conda Environment
We've installed a [Conda environment](conda.md) which has the latest [TensorFlow Wheel](https://software.intel.com/en-us/articles/intel-optimized-tensorflow-installation-guide) from Intel installed. We've also installed the [Horovod tool](https://github.com/uber/horovod) which uses MPI to run Tensorflow in a distributed way. This enables the training of Tensorflow models on Theta at large scales. Horovod provides examples for running [Tensorflow](https://github.com/horovod/horovod/blob/master/examples/tensorflow/tensorflow_mnist.py) natively or [Tensorflow using Keras](https://github.com/horovod/horovod/blob/master/examples/keras/keras_mnist.py). This can be run using:
```
#!/bin/bash
#COBALT -n <num-nodes>
#COBALT -t <wall-time>
#COBALT -q <queue>
#COBALT -A <project>

module load miniconda-3.6/conda-4.5.12

aprun -n <num-ranks> -N <mpi-ranks-per-node> python script.py
```
## TensorFlow via Cray ML Plugin

Cray has provided a custom ML Plugin for running TensorFlow on Theta that provides performance benefits when using smaller local mini-batch sizes. 

There are two example batch scripts for Python 2.7 or 3.6 which show how to setup the local environment:
```
/lus/theta-fs0/projects/SDL_Workshop/mendygra/cpe_plugin_py2.batch
/lus/theta-fs0/projects/SDL_Workshop/mendygra/cpe_plugin_py3.batch
```
This is the environment setup for python 2.7:
```
module load cray-python
export PYTHONUSERBASE=/lus/theta-fs0/projects/SDL_Workshop/mendygra/pylibs
module load /lus/theta-fs0/projects/SDL_Workshop/mendygra/tmp_inst/modulefiles/craype-ml-plugin-py2/1.1.0
```
and for python 3.6:
```
module load cray-python/3.6.1.1
export PYTHONUSERBASE=/lus/theta-fs0/projects/SDL_Workshop/mendygra/pylibs
module load /lus/theta-fs0/projects/SDL_Workshop/mendygra/tmp_inst/modulefiles/craype-ml-plugin-py3/1.1.0
```
After setting up one of these environments, you can see an example of implementing the plugin in this script
```
less $CRAYPE_ML_PLUGIN_BASEDIR/examples/tf_mnist/mnist.py
```
## References
<iframe width="560" height="315" src="https://www.youtube.com/embed/mUZRD-HQtz0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>










