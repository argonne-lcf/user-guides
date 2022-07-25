# Distributed Training on ThetaGPU Using Data Parallelism
There are two schemes for distributed learning:

1. Model parallelization: in this scheme, disjoint subsets of a neural network are assigned to different devices. Therefore, all the computations associated to the subsets are distributed. Communication happens between devices whenever there is dataflow between two subsets. Model parallelization is suitable when the model is too large to be fitted into a single device (CPU/GPU) because of the memory capacity. However, partitioning the model into different subsets is not an easy task, and there might potentially introduce load imbalance issues limiting the scaling efficiency.  

2. Data parallelization: in this scheme, all the workers own a replica of the model. The global batch of data is split into multiple minibatches, and processed by different workers. Each worker computes the corresponding loss and gradients with respect to the data it posseses. Before the updating of the parameters at each epoch, the loss and gradients are averaged among all the workers through a collective operation. This scheme is relatively simple to implement. MPI_Allreduce is the only commu

Our recent presentation about the data parallel training can be found here: https://youtu.be/930yrXjNkgM

In this documentation, we would like to show how to do data parallel training on ThetaGPU. 

## 1. Software environement setup
We are still in the process of setting up the software stacks on ThetaGPU. Currently, one can get TensorFlow, PyTorch, and Horovod with the following setup script.
