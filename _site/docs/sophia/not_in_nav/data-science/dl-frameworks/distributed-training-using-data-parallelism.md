# Distributed Training on ThetaGPU Using Data Parallelism

There are two schemes for distributed learning:

1. Model parallelization: in this scheme, disjoint subsets of a neural network are assigned to different devices. Therefore, all the computations associated with the subsets are distributed. Communication happens between devices whenever there is data flow between two subsets. Model parallelization is suitable when the model is too large to be fitted into a single device (CPU/GPU) because of the memory capacity. However, partitioning the model into different subsets is not an easy task, and it might potentially introduce load imbalance issues limiting the scaling efficiency.

2. Data parallelization: in this scheme, all the workers own a replica of the model. The global batch of data is split into multiple minibatches and processed by different workers. Each worker computes the corresponding loss and gradients with respect to the data it possesses. Before updating the parameters at each epoch, the loss and gradients are averaged among all the workers through a collective operation. This scheme is relatively simple to implement. MPI_Allreduce is the only communication operation needed.

Our recent presentation about data parallel training can be found here: [https://youtu.be/930yrXjNkgM](https://youtu.be/930yrXjNkgM)

In this documentation, we would like to show how to do data parallel training on ThetaGPU.

## Software environment setup

We are still in the process of setting up the software stacks on ThetaGPU. Currently, one can get TensorFlow, PyTorch, and Horovod with the following setup script.

```bash
source /lus/theta-fs0/software/datascience/thetagpu/anaconda3/setup.sh
```

## TensorFlow with Horovod

### 1. Initialize Horovod

```python
import horovod.tensorflow as hvd
hvd.init()
```

After this initialization, the rank ID and the number of processes can be referred to as `hvd.rank()` and `hvd.size()`. Besides, one can also call `hvd.local_rank()` to get the local rank ID within a node. This is useful when we are trying to assign GPUs to each rank.

### 2. Assign GPU to each rank

```python
gpus = tf.config.experimental.list_physical_devices('GPU')
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)
if gpus:
    tf.config.experimental.set_visible_devices(gpus[hvd.local_rank()], 'GPU')
```

In this case, we set one GPU per process: `ID=hvd.local_rank()`

### 3. Scale the learning rate

Typically, since we use multiple workers, the global batch is usually increased n times (n is the number of workers). The learning rate should increase proportionally as follows (assuming that the learning rate initially is 0.01).

```python
opt = tf.train.AdagradOptimizer(0.01 * hvd.size())
```

### 4. Wrap the optimizer with Distributed Optimizer

```python
opt = hvd.DistributedOptimizer(opt)
```

### 5. Broadcast the model from rank 0

This is to make sure that all the workers will have the same starting point.

```python
hooks = [hvd.BroadcastGlobalVariablesHook(0)]
```

### 6. Loading data according to rank ID

TensorFlow has some functions for parallel distribution of data. But for specific applications, the user might have to write their own data loader.

In general, one has two ways to deal with the data loading:

1. Each worker randomly selects one batch of data from the dataset at each step. In such a case, each worker can see the entire dataset. It is important to make sure that the different workers have different random seeds so that they will get different data at each step.

2. Each worker accesses a subset of the dataset. One manually partitions the entire dataset into different partitions, and each rank accesses one of the partitions.

In both cases, the total number of steps per epoch is `nsamples / hvd.size()`.

### 7. Checkpointing on root rank

It is important to let only one process do the checkpointing I/O lest perhaps the file be corrupted.

```python
if hvd.rank() == 0:
    checkpoint.save(checkpoint_dir)
```

### 8. Average metric across all the workers

Notice that in the distributed training, any tensor is local to each worker. In order to get the global averaged value, one can use Horovod allreduce. Below is an example of how to do the average.

```python
def tensor_average(val, name):
    tensor = torch.tensor(val)
    if with_hvd:
        avg_tensor = hvd.allreduce(tensor, name=name)
    else:
        avg_tensor = tensor
    return avg_tensor.item()
```

We provided some examples in: [https://github.com/argonne-lcf/sdl_ai_workshop/blob/master/01_distributedDeepLearning/Horovod/tensorflow2_mnist.py](https://github.com/argonne-lcf/sdl_ai_workshop/blob/master/01_distributedDeepLearning/Horovod/tensorflow2_mnist.py)

### PyTorch with DDP

PyTorch has its own native parallelization library called DDP. We will provide more details on how to run this on ThetaGPU. The current PyTorch on ThetaGPU does not have DDP built-in. We will update our users once we have DDP.

For now, please refer to [https://pytorch.org/tutorials/intermediate/ddp_tutorial.html](https://pytorch.org/tutorials/intermediate/ddp_tutorial.html)

### MPI Profiling for data parallel training

We support two ways for profiling the performance of data parallel training.

1. mpitrace library MPI trace allows us to get a flat profiling of all the MPI function calls involved during the training. To enable this, one can set the environment variable

```bash
export LD_PRELOAD=/lus/theta-fs0/software/datascience/thetagpu/hpctw/lib/libmpitrace.so
```

Then run the application as usual. MPI profiling results will be generated after the run finishes `mpi_profile.XXXX.[rank_id]`.

Below is an example output:

```
Data for MPI rank 0 of 8:
Times and statistics from MPI_Init() to MPI_Finalize().
-----------------------------------------------------------------------
MPI Routine #calls avg. bytes time(sec)
-----------------------------------------------------------------------
MPI_Comm_rank 3 0.0 0.000
MPI_Comm_size 3 0.0 0.000
MPI_Bcast 520 197140.6 0.518
MPI_Allreduce 24561 208138.3 162.080
MPI_Gather 126 4.0 0.363
MPI_Gatherv 126 0.0 0.434
MPI_Allgather 2 4.0 0.000
-----------------------------------------------------------------
MPI task 0 of 8 had the maximum communication time.
total communication time = 163.396 seconds.
total elapsed time = 187.298 seconds.
user cpu time = 4127.728 seconds.
system time = 728.100 seconds.
max resident set size = 8403.938 MBytes.

Rank 0 reported the largest memory utilization : 8403.94 MBytes
Rank 0 reported the largest elapsed time : 187.30 sec
-----------------------------------------------------------------
Message size distributions:
                       MPI_Bcast      #calls   avg. bytes       time(sec)
                                         126          4.0          0.008
                                           1          8.0          0.000
                                         121         25.0          0.006
                                          30        251.5          0.002
                                          32        512.0          0.002
                                          64       1024.0          0.005
                                          44       2048.0          0.003
                                          29       4092.8          0.003
                                          16       8192.0          0.032

                       MPI_Allreduce   #calls  avg. bytes       time(sec)
                                        19780         8.0         90.822
                                         4576        24.0         18.239
                                           43      4004.0          0.295
                                            5   2780979.2          0.469
                                           50   8160289.2         20.893
                                            9  11803392.0          0.964
                                           48  28060640.0          3.293
                                           50  64731668.5         27.105

                       MPI_Gather      #calls   avg. bytes      time(sec)
                                          126         4.0          0.363
```

The useful information here is the message size distribution.

2. Horovod Timeline To perform Horovod timeline analysis, one has to set the environment variable HOROVOD_TIMELINE which specifies the file for the output.

```bash
export HOROVOD_TIMELINE=timeline.json
```

This file is only recorded on rank 0, but it contains information about the activity of all workers. You can then open the timeline file using the chrome://tracing facility of the Chrome browser.

More details: [https://horovod.readthedocs.io/en/stable/timeline_include.html](https://horovod.readthedocs.io/en/stable/timeline_include.html)