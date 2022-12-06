# Keras

## Introduction
[Keras](https://keras.io) is a high-level neural networks API, written in Python and capable of running on top of [TensorFlow](https://github.com/tensorflow/tensorflow), [CNTK](https://github.com/Microsoft/cntk), or [Theano](https://github.com/Theano/Theano). It was developed with a focus on enabling fast experimentation. It allows for easy and fast prototyping, and support both convolutional networks and recurrent networks and the combination of the two. It runs seamlessly on CPU and GPU.

## Datascience module
On Theta, we support TensorFlow backend for Keras. To use the datascience Keras module on Theta, please load the following two modules:
```
module load datascience/keras-2.2.4
 
module load datascience/tensorflow-1.12
```

Notice that the datascience/tensorflow-* modules were compiled with AVX512 extension on Theta. Therefore, it could not run on login node, otherwise it will issue an “illegal instruction” fault. One has to submit the job to KNL nodes (see TensorFlow documentation for details). 

Since we use TensorFlow as the backend, all the optimal environmental setups (Threading + affinity) are applicable here. Please visit the [Tensorflow documentation page](tensorflow) for the optimal setting.  

We do not see any incompatibility issues in using different versions of keras and tensorflow as those specified above. Feel free to change other versions of keras or TensorFlow. Currently, we support version 2.2.2 and 2.2.4.  

## Distributed learning using Horovod
We support distributed learning using Horovod. To use it please load datascience/horovod-0.15.2 module. Please change your python script accordingly

### Initialize Horovod by adding the following lines to the beginning of your python script. 
```
import horovod.keras as hvd
 
hvd.init()
```
After this initialization, the total number of ranks and the rank id could be access through hvd.rank(), hvd.size() functions. 

## Scale the learning rate.
Typically, since we use multiple workers, the global batch is usually increased n times (n is the number of workers). The learning rate should increase proportionally as follows (assuming that the learning rate initially is 0.01).

```
opt = keras.optimizers.Adadelta(1.0 * hvd.size()
```
In some case, 0.01*hvd.size() might be too large, one might want to have some warming up steps with smaller learning rate. 

### Wrap the optimizer with Distributed Optimizer
```
opt = hvd.DistributedOptimizer(opt)
```
In such case, opt will automatically average the loss and gradients among all the workers and then perform update. 

### Broadcast the model from rank 0, so that all the workers will have the same starting point
```
callbacks = [hvd.callbacks.BroadcastGlobalVariablesCallback(0)]
```
Notice that by default, TensorFlow will initialize the parameters randomly. Therefore, by default, different workers will have different parameters. So it is crucial to broadcast the model from rank 0 to other ranks. 

### Letting only rank 0 to write checkpoint
```
if hvd.rank() == 0:
```
callbacks.append(keras.callbacks.ModelCheckpoint('./checkpoint-{epoch}.h5'))

### Loading data according to rank ID
Since we are using data parallel scheme. Different ranks shall process different data. One has to change the data loader part of the python script to ensure different ranks read different mini batches of data. 

#### Example
A simple example for doing linear regression using Keras + Horovod is put in the follwoing directory on Theta:
/projects/SDL_Workshop/hzheng/examples/keras/linreg 
 
linreg_keras.py is the python script, and qsub.sc is the COBALT submission script. 

<iframe width="709" height="399" src="https://www.youtube.com/embed/930yrXjNkgM" title="Deep Learning with Keras, TensorFlow, PyTorch, and Horovod on Theta" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
