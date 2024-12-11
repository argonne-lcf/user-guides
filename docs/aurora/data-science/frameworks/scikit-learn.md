# scikit-learn on Aurora

[scikit-learn](https://scikit-learn.org/stable/) is a popular open-source Python library for machine learning. It has wide coverage of machine learning algorithms (other than neural networks), such as k-means clustering and random forests. 

scikit-learn (abbreviated "sklearn") is built for CPUs. However, Intel(R) Extension for Scikit-learn (abbreviated "sklearnex") is a free Python package that speeds up scikit-learn on Intel CPUs & GPUs.  For more information, see the [scikit-learn-intelex Github page](https://github.com/uxlfoundation/scikit-learn-intelex), [the documentation](https://uxlfoundation.github.io/scikit-learn-intelex/latest/index.html), or [Intel's website](https://www.intel.com/content/www/us/en/developer/tools/oneapi/scikit-learn.html#gs.b2f4sw). 

The acceleration is by patching: replacing stock scikit-learn algorithms with the versions from Intel(R) oneAPI Data Analytics Library (oneDAL). 

Note that patching only affects supported algorithms and parameters. To see the current support, check Intel's page [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/algorithms.html). Otherwise, sklearnex will fall back on stock scikit-learn, which has to run on the CPU. To know which version is being used, enable [Verbose Mode](https://uxlfoundation.github.io/scikit-learn-intelex/latest/verbose.html), for example with the environment variable `SKLEARNEX_VERBOSE=INFO`. However, verbose mode is only available for supported algorithms.

## Environment Setup

Intel Extension for Scikit-learn is already pre-installed on Aurora, available in the `frameworks` 
module. You can load the frameworks module as described [here](../python.md), which will activate a conda environment. 

## Usage
### Patching
There are multiple ways to patch scikit-learn with the Intel Extension, as Intel documents [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/what-is-patching.html). For example, you can patch within the script, like this:
```
from sklearnex import patch_sklearn
patch_sklearn()
```
It is important to note that this needs to happen before importing scikit-learn. To explicitly only patch certain estimators, you can import particular functions from sklearnex instead of sklearn, like this:
```
from sklearnex.neighbors import NearestNeighbors
```

### GPU Acceleration
Intel Extension for Scikit-learn can execute algorithms on the GPU via the [dpctl](https://intelpython.github.io/dpctl/latest/index.html) package, which should be included in the frameworks module. (If not, see the [Python page](../python.md)) dpctl implements oneAPI concepts like queues and devices. 

As described in more detail in Intel's documentation [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/oneapi-gpu.html), there are two ways to run on the GPU. 

1. Pass the input data to the algorithm as `dpctl.tensor.usm_ndarray`. Then the algorithm will run on the same device as the data and return the result as a usm_array on the same device. 
2. Configure Intel Extension for Scikit-learn, for example, by setting a context: `sklearnex.config_context`. 

### Distributed Mode
To distribute an sklearnex algorithm across multiple GPUs, we need several ingredients, which we will demonstrate in an example below. We recommend using the MPI backend rather than the CCL backend since it is more tested on Aurora.

!!! warning "Warning"
    The current version of Intel Extension to scikit-learn does not scale well to multiple GPUs. The cause has been identified, and we're waiting on a fix. However, if you use the oneDAL C++ API, the scaling is much better.

1. Create an MPI communicator using mpi4py if you need to use the rank. (mpi4py is also included in the frameworks module.)
2. Check for GPU devices. 
3. Use dpctl to create a SYCL queue (connection to the GPU devices you choose).
4. Using dpctl and your queue, move your data to the GPU devices.
5. Run the algorithm on that data. The compute will happen where the data is. The algorithm should be from `sklearnex.spmd`. 

### An Example Python Script
This example is adapted from [an example](https://github.com/uxlfoundation/scikit-learn-intelex/blob/main/examples/sklearnex/knn_bf_classification_spmd.py) in Intel's scikit-learn-intelex Github repo.

```Python
import dpctl
import dpctl.tensor as dpt
from mpi4py import MPI
from sklearn.datasets import make_classification
from sklearnex.spmd.neighbors import KNeighborsClassifier

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

if dpctl.has_gpu_devices():
    q = dpctl.SyclQueue("gpu")
else:
    raise RuntimeError(
        "GPU devices unavailable."
    )

# For the sake of a concise demo, each rank is generating different random training data. 
X, y = make_classification(n_samples=100000, n_features=8, random_state=rank)
dpt_X = dpt.asarray(X, usm_type="device", sycl_queue=q)
dpt_y = dpt.asarray(y, usm_type="device", sycl_queue=q)

model_spmd = KNeighborsClassifier(
    algorithm="brute", n_neighbors=20, weights="uniform", p=2, metric="minkowski"
)
model_spmd.fit(dpt_X, dpt_y)
```

### An Example Job Script

Below we give an example job script, adapted from [the PyTorch page](pytorch.md). Note that we are using Aurora MPICH (the default MPI library on Aurora) and not using oneCCL, so we don't need special oneCCL settings. For more about pinning ranks to CPU cores and GPUs, see the [Running Jobs page](../../running-jobs-aurora.md).

```shell
#!/bin/bash -l
#PBS -l select=1                                # selecting 1 node
#PBS -l place=scatter
#PBS -l walltime=10:00
#PBS -q EarlyAppAccess                          # a specific queue
#PBS -A Aurora_deployment                       # project allocation
#PBS -l filesystems=home                        # specific filesystem, can be a list separated by :
#PBS -k doe
#PBS -e /home/$USER/path/to/errordir            
#PBS -o /home/$USER/path/to/outdir              # path to `stdout` or `.OU` files
#PBS -j oe                                      # output and error placed in the `stdout` file
#PBS -N a.name.for.the.job

#####################################################################
# This block configures the total number of ranks, discovering
# it from PBS variables.
# 12 Ranks per node, if doing rank/tile
#####################################################################

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=12
let NRANKS=${NNODES}*${NRANKS_PER_NODE}

#####################################################################
# Environment set up, using the latest frameworks drop
#####################################################################

module use /soft/modulefiles
module load frameworks

export NUMEXPR_NUM_THREADS=64
# This is to resolve an issue due to a package called "numexpr". 
# It sets the variable 
# 'numexpr.nthreads' to available number of threads by default, in this case 
# to 208. However, the 'NUMEXPR_MAX_THREADS' is also set to 64 as a package 
# default. The solution is to either set the 'NUMEXPR_NUM_THREADS' to less than 
# or equal to '64' or to increase the 'NUMEXPR_MAX_THREADS' to the available 
# number of threads. Both of these variables can be set manually.

#####################################################################
# End of environment setup section
#####################################################################

#####################################################################
# JOB LAUNCH
######################################################################

export CPU_BIND="verbose,list:2-4:10-12:18-20:26-28:34-36:42-44:54-56:62-64:70-72:78-80:86-88:94-96"

# Launch the script
mpiexec -np ${NRANKS} -ppn ${NRANKS_PER_NODE} \
--cpu-bind ${CPU_BIND} gpu_tile_compact.sh \
python path/to/application.py

```