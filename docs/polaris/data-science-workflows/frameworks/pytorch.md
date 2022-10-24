# PyTorch on Polaris

PyTorch is a popular, open source deep learning framework developed and released by Facebook.  The [PyTorch home page](https://pytorch.org/) has more information about PyTorch, which you can refer to.  For trouble shooting on Polaris, please contact support@alcf.anl.gov.

## Installation on Polaris

PyTorch is installed on Polaris already, available in the `conda` module.  To use it from a compute node, please do:

```bash
module load conda
conda activate
```

Then, you can load PyTorch in `python` as usual (below showing results from the `conda/2022-07-19` module):

```python
>>> import torch
>>> torch.__version__
'1.12.0a0+git67ece03'
>>>
```

This installation of PyTorch was built from source and the cuda libraries it uses are found via the `CUDA_HOME` environment variable (below showing results from the `conda/2022-07-19` module):

```bash
$ echo $CUDA_HOME
/soft/datascience/cuda/cuda_11.5.2_495.29.05_linux
```

If you need to build applications that use this version of PyTorch and CUDA, we recommend using these cuda libraries to ensure compatibility.  We periodically update the PyTorch release, though updates will come in the form of new versions of the `conda` module.

PyTorch is also available through nvidia containers that have been translated to Singularity containers.  For more information about containers, please see the [containers](../containers/containers.md) documentation page.

## PyTorch Best Practices on Polaris

### Single Node Performance

When running PyTorch applications, we have found the following practices to be generally, if not universally, useful and encourage you to try some of these techniques to boost performance of your own applications.

1. Use Reduced Precision. Reduced Precision is available on A100 via tensorcores and is supported with PyTorch operations.  In general, the way to do this is via the PyTorch Automatic Mixed Precision package (AMP), as descibed in the [mixed precision documentation](https://pytorch.org/docs/stable/amp.html).  In PyTorch, users generally need to manage casting and loss scaling manually,  though context managers and function decorators can provide easy tools to do this.

2. PyTorch has a `JIT` module as well as backends to support op fusion, similar to TensorFlow's `tf.function` tools.  However, PyTorch JIT capabilities are newer and may not yield performance improvements.  Please see [TorchScript](https://pytorch.org/docs/stable/jit.html) for more information.


## Multi-GPU / Multi-Node Scale up

PyTorch is compatible with scaling up to multiple GPUs per node, and across multiple nodes.  Good scaling performance has been seen up to the entire Polaris system, > 2048 GPUs.  Good performance with PyTorch has been seen with both DDP and Horovod.  For details, please see the [Horovod documentation](https://horovod.readthedocs.io/en/stable/pytorch.html) or the [Distributed Data Parallel documentation](https://pytorch.org/tutorials/intermediate/ddp_tutorial.html).  Some Polaris-specific details that may be helpful to you:

1. CPU affinity and NCCL settings can improve scaling performance, particularly at the largest scales.  In particular, we encourage users to try their scaling measurements with the following settings:
 - Set the environment variable `NCCL_COLLNET_ENABLE=1`
 - Set the environment varialbe `NCCL_NET_GDR_LEVEL=PHB`
 - Manually set the CPU affinity via mpiexec, such as with `--cpu-bind verbose,list:0,8,16,24
`

2. Horovod and DDP work best when you limit the visible devices to only one GPU.  Note that if you import `mpi4py` or `horovod`, and then do something like `os.environ["CUDA_VISIBLE_DEVICES"] = hvd.local_rank()`, it may not actually work!  You must set the `CUDA_VISIBLE_DEVICES` environment variable prior to doing `MPI.COMM_WORLD.init()`, which is done in `horovod.init()` as well as implicitly in `from mpi4py import MPI`.   On Polaris specifically, you can use the environment variable `PMI_LOCAL_RANK` (as well as `PMI_LOCAL_SIZE`) to learn information about the node-local MPI ranks.  

### DeepSpeed

DeepSpeed is also available and usable on Polaris.  For more information, please see the [DeepSpeed](./deepspeed.md) documentation directly.

## PyTorch Dataloaders

Please note there is a bug that causes a hang when using pytorch data loaders + distributed training (Horovod, DDP, etc).  To workaround this, NVIDIA recommends setting `num_workers=0` in the dataloader configuration.

