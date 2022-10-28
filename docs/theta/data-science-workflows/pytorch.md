# PyTorch

PyTorch is an open source python-based library built to provide flexibility as a deep learning development platform. The workflow of PyTorch is as close as you can get to python’s scientific computing library – numpy. For detailed instruction of PyTorch package, please visit <https://pytorch.org>. 

## Datascience PyTorch Module

The datascience PyTorch module was built with GCC/7.3.0 and Intel Python 3.5. It was built with KNL specific optimizations (e.g., AVX512 instruction), and is linked to high performance math libraries, such as MKL, MKL-DNN (home built with AVX512). Other dependent libraries, such as NumPy, SciPy, are also built with AVX512. Because the PyTorch package is built with AVX512, it only runs on KNL nodes on Theta. Since the login node, and mom nodes are not KNL, one has to run it on compute node through "aprun ... python script.py".

## Running PyTorch on Theta

### Loading modules
```
module load datascience/pytorch-0.5.0-mkldnn (change to other version number)
module load datascience/horovod-0.13.11
```

What this will do is essentially prepend PyTorch related path to your PYTHONPATH and PATH. 

- **Hyper-threading:** on Theta, on could have 1, 2, or 4 threads per core (this corresponds to the -j option in aprun command). -j 2 is suggested for deep learning applications since it involved a lot of matrix multiplication kernels. 
- **OMP_NUM_THREADS:**  The number of threads could be set in aprun command, for example: "```aprun -n ... -N ... -e OMP_NUM_THRADS=4"```sets 4 threads per MPI rank.
- **OMP affinity settings:** the user can specify the environmental variable KMP_AFFINITY to change the thread affinity. We suggest to use "-cc depth"in the aprun command, which corresponds to the following setting:  "KMP_AFFINITY=granularity=fine,verbose,compact,1,0!". The other option the user could try is "-cc none". We have found that "-cc depth" gives best performance for most cases.
- **Submitting jobs (sample scripts in /soft/datascience/):** Below is a typical submission script on Theta (sub.sc)

```
#!/bin/sh
#COBALT -n 128 -t 1:00:00
#COBALT -q default --attrs mcdram=cache:numa=quad
#COBALT -A YOUR_PROJECT_NAME
module load datascience/pytorch-0.5.0-mkldnn datascience/horovod-0.13.11
NPROC_PER_NODE=4
NPROC=$((NPROC_PER_NODE*COBALT_JOBSIZE))
aprun -n $NPROC -N $NPROC_PER_NODE -e KMP_BLOCKTIME=0 -j 2 -e OMP_NUM_THREADS=32 -cc depth -d 32 python PYTHON_SCRIPT ARG1 ARG2 ...
```

## Data parallelization through Horovod

PyTorch has its own distributed communication package -- torch.distributed, which provides an MPI-like interface for exchanging tensor data across multi-machine network, including send/recv, reduce/all_reduce, gather/all_gather, scatter, barrier, etc. The PyTorch on Theta, however, does not have this MPI support yet. We instead, provide Horovod package or distributing training through a data parallelization framework. It is easy to change your serial code to run data parallelization through Horovod. The followings are the procedures: 

1. Initialize Horovod

```
import horovod.torch as hvd
hvd.init()
```

After this initialization, the total number of ranks and the rank id could be access through hvd.rank(), hvd.size() functions.

2. Scale the learning rate in the optimizer function. Typically, when we use multiple workers, the global batch increases n times (n is the number of workers). The learning rate should increase proportionally as follows (assuming that the initial learning rate is 0.01).

```
optimizer = optim.SGD(model.parameters(), lr=0.01*hvd.size(), momentum = args.momentum)
```

In some case, 0.01*hvd.size() might be too large, so one might want to have some warming up steps with a smaller learning rate.

3. Wrap the optimizer with Distributed Optimizer

```
optimizer = hvd.DistributedOptimizer(optimizer, ...)
```

In such case, "optimizer" will automatically average the loss and gradients among all the workers and then perform update.

4. Broadcast the model from rank 0, so that all the workers will have the same starting point.

```
hvd.broadcast_parameters(model.stat_dict(), root_rank=0)
```

5. Loading data according to rank ID: Torch has some functions for parallel distribution of data. But for specific applications, the user might have to write their own data loader.

## Profiling/post-processing with TensorBoardX

TensorBoardX is for creating events in PyTorch, which can be process by Tensorboard. One could check [tensorboardX](https://github.com/lanpa/tensorboardX) for details on how to use it. Tensorboard events, including scalar, image, figure, histogram, audio, text, graph, onnx_graph, embedding, pr_curve and video summaries, could be created with a simple function call "writer.add_XXX(...)" as follows: 

```
from tensorboardX 
import SummaryWriter
writer = SummaryWriter() 
writer.add_scale(...) 
writer.add_audio(...) 
writer.add_text(...) 
writer.export_scalars_to_json("output.json")
writer.close()
```

The created log files could then be read with "tensorboard --logdir runs". The data could be visualized on your local machine through ssh tunneling (see TensorBoard usage in TensorFlow).

### Examples on Theta (MNIST, imagenet benchmarks)

Please check the test example on Theta:

```
/projects/SDL_Workshop/hzheng/examples/pytorch/MNIST
```

## FAQ and common issues

**Illegal instruction" or "AVX512F" error**
This happens when you are trying to run AVX512 compiled applications on login nodes or mom nodes. Run use aprun on KNL nodes through qsub instead. If you hit this error while you are building python package, try to use "aprun -n 1 -cc node python setup.py build ..."

**Cannot download dataset**
When the job is submitted, it is submitted to KNL nodes which are not connected to outside internet. Therefore, it is suggested that the users download the datasets on login node (e.g., through wget), or transfer the data through scp or Globus. 

## References: 

- [PyTorch Website](https://pytorch.org)
- [PyTorch on GitHub](https://github.com/pytorch/pytorch)
- [An introduction to PyTorch](https://www.analyticsvidhya.com/blog/2018/02/pytorch-tutorial)
- [Introduction to PyTorch](https://link.springer.com/chapter/10.1007/978-1-4842-2766-4_12)

