# JAX

JAX is another popular Python package for accelerated computing. JAX is built on XLA (the same XLA TensorFlow uses) as well as AutoGrad, and additionally has acceleration tools that operate on functions such as `vmap`, `jit`, etc. JAX is not as widespread in machine learning as TensorFlow and PyTorch for traditional models (Computer Vision, Language Models), though it is quickly gaining prominence. JAX is very powerful when a program needs non-traditional autodifferentiation or vectorization, such as forward-mode AD, higher-order derivatives, Jacobians, Hessians, or any combination of the above. Users of JAX on Polaris are encouraged to read the [user documentation](https://jax.readthedocs.io/en/latest/) in detail, particularly the details about pure-functional programming, no in-place operations, and the common mistakes in writing functions for the `@jit` decorator.

## JAX on Polaris

JAX is installed on Polaris via the `jax` module, available with:
```bash
module use /soft/modulefiles; module load jax
```

Then, you can load JAX in `python` as usual (below showing results from the `conda/2024-04-29` module):

```python
>>> import jax
>>> jax.__version__
'0.4.26'
>>>
```

## Notes on JAX 0.4.26

On Polaris, due to a bug, an environment variable must be set to use JAX on GPUs. The following code will crash:
```python
import jax.numpy as numpy
a = numpy.zeros(1000)
```
outputting an error that looks like:
```python
jaxlib.xla_extension.XlaRuntimeError: UNKNOWN: no kernel image is available for execution on the device
```

You can fix this by setting an environment variable:
```bash
export XLA_FLAGS="--xla_gpu_force_compilation_parallelism=1"
```

## Scaling JAX to multiple GPUs and multiple Nodes

JAX has intrinsic scaling tools to use multiple GPUs on a single node, via the `pmap` function. If this is sufficient for your needs, excellent. If not, another alternative is to use the newer package [mpi4jax](https://github.com/mpi4jax/mpi4jax).

mpi4jax is a relatively new project and requires setting some environment variables for good performance and usability:
- Set `MPI4JAX_USE_CUDA_MPI=1` to use CUDA-Aware MPI, supported in the `conda` module, to do operations directly from the GPU.
- Set `MPICH_GPU_SUPPORT_ENABLED=1` to use CUDA-Aware MPI.

The following code, based on a test script from the mpi4jax repository, can help you verify you are using mpi4jax properly:

```python
import os
from mpi4py import MPI
import jax
import jax.numpy as jnp
import mpi4jax

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
local_rank = int(os.environ["PMI_LOCAL_RANK"])

available_devices = jax.devices("gpu")
if len(available_devices) <= local_rank:
    raise Exception("Could not find enough GPUs")

target_device = available_devices[local_rank]


@jax.jit
def foo(arr):
    arr = arr + rank
    arr_sum, _ = mpi4jax.allreduce(arr, op=MPI.SUM, comm=comm)
    return arr_sum

with jax.default_device(target_device):
    a = jnp.zeros((3, 3))
    print(f"Rank {rank}, local rank {local_rank}, a.device is {a.device()}")
    result = foo(a)
    print(f"Rank {rank}, local rank {local_rank}, result.device is {result.device()}")

    import time
    print("Sleeping for 5 seconds if you want to look at nvidia-smi ... ")
    time.sleep(5)
    print("Done sleeping")

if rank == 0:
    print(result)
```

JAX and mpi4jax are both still somewhat early in their software lifecycles. Updates are frequent, and if you require assistance please contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov).