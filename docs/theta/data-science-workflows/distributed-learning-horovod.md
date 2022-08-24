# Distributed Learning with Horovod

There are two schemes for distributed learning: 
1. Model parallelization: in this scheme, disjoint subsets of a neural network are assigned to different devices. Therefore, all the computations associated to the subsets are distributed. Communication happens between devices whenever there is dataflow between two subsets. Model parallelization is suitable when the model is too large to be fitted into a single device (CPU/GPU) because of the memory capacity. However, partitioning the model into different subsets is not an easy task, and there might potentially introduce load imbalance issues limiting the scaling efficiency. 

2. Data parallelization: in this scheme, all the workers own a replica of the model. The global batch of data is split into multiple minibatches, and processed by different workers. Each worker computes the corresponding loss and gradients with respect to the data it possesses. Before the updating of the parameters at each epoch, the loss and gradients are averaged among all the workers through a collective operation. This scheme is relatively simple to implement. MPI_Allreduce is the only communication overhead that introduced. 

On Theta, we support data parallelization through Horovod. Horovod is an open source distributed deep learning framework developed by Uber. It is based on a bandwidth-optimal ring-allreduce algorithm proposed by Baidu "[Bringing HPC Techniques to Deep Learning](http://andrew.gibiansky.com/)." One could refer [https://eng.uber.com/horovod](https://eng.uber.com/horovod) and [https://arxiv.org/abs/1802.05799](https://arxiv.org/abs/1802.05799) for a more thorough discussion of the detailed implementation and benchmarks. 

## Datascience Horovod Module

We built Horovod module in the Cray programming environment on Theta using GCC/7.3.0.  It was linked to Cray MPICH library. This module could be loaded using "module load datascience/horovod-0.13.11". This module could NOT run on Login node/mom node. It must be run through "aprun -n ... -N ..." (mpirun does not work).

## How to use Horovod

### Loading the module
```module load datascience/horovod-0.15.2```

### Changing the codes
The followings are the list of changes that one has to make changes to a Python script in order to run it using Horovod: 
  - Import Horovod and initialize it: "import horovod.PACKAGE as hvd; hvd.init()". PACKAGE could be tensorflow, pytorch, or Keras. After this initialization, the total number of ranks and the rank id could be access through hvd.rank(), hvd.size() functions.
  - Scale the learning rate in the optimizer. Typically, since we use multiple workers, the global batch is usually increases n times (n is the number of workers). The learning rate should increase proportionally. 
  - Wrap the optimizer with Distributed Optimizer "hvd.DistributedOptimizer"
  - Broadcast the model parameters from rank 0, so that all the workers will have the same starting point. 
  - Loading data according to rank ID: TensorFlow has some functions for parallel distribution of data. But for specific applications, the user might have to write their own data loader. 
  - Write check point files only on rank 0. 

Specific instructions on how to make changes for different packages are shown on the respective documentations: [TensorFlow](https://anl.box.com/s/8dxu1eqnnpqfxw1602hwk4qm54jz1r3g), [PyTorch](https://anl.box.com/s/adxknsprih7cs62r8500bdyy3ed4nizn). 

### Profiling with Horovod timeline
Horovod has the ability to record the timeline of its activity, called [Horovod Timeline](https://github.com/horovod/horovod/blob/master/horovod/common/timeline.h). To record a Horovod Timeline, set the HOROVOD_TIMELINE environment variable to the location of the timeline file to be created. This file is only recorded on rank 0, but it contains information about activity of all workers.
```
aprun -n ... -N ... -e HOROVOD_TIMELINE=/path/to/timeline.json python train.py 
```
One can then open the timeline file using ```chrome://tracing``` facility of the Chrome browser. Users can use Horovod Timelines to view exactly what each node was doing at each time step throughout a training job. This helps identify bugs and debug performance issues. 

We put examples of MNIST training using TensorFlow/PyTorch. The folders contain the python scripts, datasets and COBALT submission script. 

- TensorFlow + Horovod: `/projects/SDL_Workshop/hzheng/examples/tensorflow/MNIST`
- PyTorch + Horovod: `/projects/SDL_Workshop/hzheng/examples/pytorch/MNIST`

<iframe width="560" height="315" src="https://www.youtube.com/embed/930yrXjNkgM" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


