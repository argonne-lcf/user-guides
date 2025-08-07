# scikit-learn on Aurora

[scikit-learn](https://scikit-learn.org/stable/) is a popular open-source Python library for machine learning. It has wide coverage of machine learning algorithms (other than neural networks), such as k-means clustering and random forests.

scikit-learn (abbreviated "sklearn") is built for CPUs. However, the "Extension to Scikit-learn" (formerly the "Intel(R) Extension for Scikit-learn"), abbreviated "sklearnex", is a free Python package that speeds up scikit-learn on Intel CPUs & GPUs and adds support for additional functionality, such as incremental and distributed algorithms. For more information, see the [scikit-learn-intelex GitHub page](https://github.com/uxlfoundation/scikit-learn-intelex), [the documentation](https://uxlfoundation.github.io/scikit-learn-intelex/latest/index.html), or [Intel's website](https://www.intel.com/content/www/us/en/developer/tools/oneapi/scikit-learn.html#gs.b2f4sw).

## Environment Setup

Extension for Scikit-learn is not currently in the `frameworks` module. Below is an example of building Extension for Scikit-learn in a `venv` on top of the `conda` environment in the `frameworks` module. For more information about virtual environments, see the [Python page](../python.md). Please note the warning about importing Python packages at large scale. When you build Extension for Scikit-learn, it should find the [oneDAL library](../../applications-and-libraries/libraries/onedal.md) that is part of the oneAPI installation on Aurora. 
```bash linenums="1"
git clone https://github.com/intel/scikit-learn-intelex.git
module load frameworks
python -m venv sklearnex_build --system-site-packages
source sklearnex_build/bin/activate
pip install pybind11==3.0.0 cmake==4.0.3 clang-format

cd scikit-learn-intelex
bash conda-recipe/build.sh
```

## Usage

### Patching

To accelerate existing scikit-learn code with minimal code changes, Extension for Scikit-learn uses patching: replacing stock scikit-learn algorithms with versions that utilize Intel(R) oneAPI Data Analytics Library (oneDAL).

Note that patching only affects supported algorithms and parameters. To see the current support, check Intel's page [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/algorithms.html). Otherwise, the Extension will fall back on stock scikit-learn, which has to run on the CPU. To know which version is being used, enable [Verbose Mode](https://uxlfoundation.github.io/scikit-learn-intelex/latest/verbose.html), for example, with the environment variable `SKLEARNEX_VERBOSE=INFO`. However, verbose mode is only available for supported algorithms.

There are multiple ways to patch scikit-learn with the Extension, as Intel documents [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/quick-start.html#patching). For example, you can patch within the script, like this:

```python linenums="1"
from sklearnex import patch_sklearn
patch_sklearn()
```

It is important to note that this needs to happen before importing scikit-learn. To explicitly only patch certain estimators, you can import particular functions from `sklearnex` instead of `sklearn`, like this:

```python linenums="1"
from sklearnex.neighbors import NearestNeighbors
```

### GPU Acceleration

Extension for Scikit-learn can execute algorithms on the GPU via the [dpctl](https://intelpython.github.io/dpctl/latest/index.html) package, which should be included in the frameworks module. If not, refer to [Aurora's Python page > dpctl section](../python.md#dpctl). dpctl implements oneAPI concepts like queues and devices.

As described in more detail in Intel's documentation [here](https://uxlfoundation.github.io/scikit-learn-intelex/latest/oneapi-gpu.html), there are two ways to run on the GPU.

1. Pass the input data to the algorithm as `dpctl.tensor.usm_ndarray`. Then the algorithm will run on the same device as the data and return the result as a usm_array on the same device.
2. Configure Extension for Scikit-learn, for example, by setting a context: `sklearnex.config_context`.

Patching (described above) can be helpful in the case of functionality that already exists in scikit-learn because you can import the functions from `sklearn` instead of `sklearnex`.

### Distributed Mode

To distribute an `sklearnex` algorithm across multiple GPUs, we need several ingredients demonstrated in an example below. We recommend using the MPI backend rather than the CCL backend since it is tested more thoroughly on Aurora.

!!! warning "Multi-GPU scaling performance"
    The current version of Extension to scikit-learn does not scale well to multiple GPUs. The cause is that scikit-learn includes some array checks before starting an algorithm, and Intel has not implemented performing those checks on the GPU. For now, the data gets copied to the host to perform these checks, which can be a significant bottleneck. However, you can use a parameter to bypass those checks. Either run a function within a `with sklearnex.config_context(use_raw_input=True)` block or run `sklearnex.set_config(use_raw_input=True).` Alternatively, you could use [the oneDAL C++ API](../../applications-and-libraries/libraries/onedal.md) directly.

1. Use dpctl to create a SYCL queue (connection to the GPU devices you choose).
2. Using dpctl and your queue, move your data to the GPU devices.
3. Run the algorithm on that data. The compute will happen where the data is. The algorithm should be from `sklearnex.spmd`.

Since you are importing the algorithm from `sklearnex` instead of `sklearn`, patching is not necessary here.

### An Example Python Script

This example is adapted from [an example](https://github.com/uxlfoundation/scikit-learn-intelex/blob/main/examples/sklearnex/knn_bf_classification_spmd.py) in Intel's scikit-learn-intelex GitHub repo.

```python linenums="1" title="knn_mpi4py_spmd.py"
import dpctl
import dpctl.tensor as dpt
from mpi4py import MPI
from sklearn.datasets import make_classification
from sklearn.model_selection import train_test_split
import sklearnex
from sklearnex.spmd.neighbors import KNeighborsClassifier

# Temporary solution until Intel implements array checks on GPU
sklearnex.set_config(use_raw_input=True)

# Create a GPU SYCL queue to store data on device.
q = dpctl.SyclQueue("gpu")

# mpi4py is one way to handle arranging data across ranks.
# For the sake of a concise demo, each rank is generating different random training data.
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
X, y = make_classification(n_samples=100000, n_features=8, random_state=rank)
X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.2)

# Move the data to the GPU devices.
dpt_X_train = dpt.asarray(X_train, usm_type="device", sycl_queue=q)
dpt_X_val = dpt.asarray(X_val, usm_type="device", sycl_queue=q)
dpt_y_train = dpt.asarray(y_train, usm_type="device", sycl_queue=q)

# Run the algorithm.
model_spmd = KNeighborsClassifier(
    algorithm="brute", n_neighbors=20, weights="uniform", p=2, metric="minkowski"
)
model_spmd.fit(dpt_X_train, dpt_y_train)

# for this algorithm, predict is more expensive than fit
y_val_predict = model_spmd.predict(dpt_X_val)
```

### An Example Job Script

Below we give an example job script. Note that we are using Aurora MPICH (the default MPI library on Aurora) and not using oneCCL, so we don't need special oneCCL settings. For more about pinning ranks to CPU cores and GPUs, see the [Running Jobs page](../../running-jobs-aurora.md).

```bash linenums="1" title="example_scikit-learn_distributed.sh" hl_lines="14"
module load frameworks
# Activate venv where you installed Extension to scikit-learn
source sklearnex_build/bin/activate

# This is to resolve an issue due to a package called "numexpr".
# It sets the variable
# 'numexpr.nthreads' to available number of threads by default, in this case
# to 208. However, the 'NUMEXPR_MAX_THREADS' is also set to 64 as a package
# default. The solution is to either set the 'NUMEXPR_NUM_THREADS' to less than
# or equal to '64' or to increase the 'NUMEXPR_MAX_THREADS' to the available
# number of threads. Both of these variables can be set manually.
export NUMEXPR_NUM_THREADS=64

export CPU_BIND="verbose,list:2-4:10-12:18-20:26-28:34-36:42-44:54-56:62-64:70-72:78-80:86-88:94-96"

# Launch the script
mpiexec -np 12 -ppn 12 --cpu-bind ${CPU_BIND} gpu_tile_compact.sh python knn_mpi4py_spmd.py
```
The highlighted line, which pins each of the 12 MPI ranks to specific CPU physical cores, is essential to achieving good performance across all 12 GPU Tiles on an Aurora node.

### An incremental statistics calculation example

Below, we give an example of a basic incremental calculation. This example is adapted from [an example](https://github.com/uxlfoundation/scikit-learn-intelex/blob/main/examples/sklearnex/incremental_basic_statistics.py) in Intel's scikit-learn-intelex GitHub repo. The incremental interface is accessible via the methods of the `sklearnex.basic_statistics.IncrementalBasicStatistics` class. There are two options: use the `partial_fit`method for each chunk of available data, or, if you have access to the whole data, call the `fit` method.

```python linenums="1" title="incremental_basic_statistics.py"
import numpy as np

from sklearnex.basic_statistics import IncrementalBasicStatistics

incbs = IncrementalBasicStatistics(result_options=["mean", "max", "sum"])

# We do partial_fit for each batch and then print final result.
X_1 = np.array([[0, 1], [0, 1]])
result = incbs.partial_fit(X_1)

X_2 = np.array([[1, 2]])
result = incbs.partial_fit(X_2)

X_3 = np.array([[1, 1], [1, 2], [2, 3]])
result = incbs.partial_fit(X_3)

print(f"Mean:\n{result.mean_}")
print(f"Max:\n{result.max_}")
print(f"Sum:\n{result.sum_}")
print(f"Sum Squares:\n{result.sum_squares_}")

# We put the whole data to fit method, it is split automatically and then
# partial_fit is called for each batch.
incbs = IncrementalBasicStatistics(result_options=["mean", "max", "sum"], batch_size=3)
X = np.array([[0, 1], [0, 1], [1, 2], [1, 1], [1, 2], [2, 3]])
result = incbs.fit(X)

print(f"Mean:\n{result.mean_}")
print(f"Max:\n{result.max_}")
print(f"Sum:\n{result.sum_}")
print(f"Sum Squares:\n{result.sum_squares_}")
```
To execute on the GPU via the [dpctl](https://intelpython.github.io/dpctl/latest/index.html) package, one should add the following highlighted modifications:
```python linenums="1" title="incremental_basic_statistics_dpctl.py" hl_lines="1-2" "11"
import dpctl
import dpctl.tensor as dpt

import numpy as np

from sklearnex.basic_statistics import IncrementalBasicStatistics

# We create GPU SyclQueue and then put data to dpctl tensor using
# the queue. It allows us to do computation on GPU.

queue = dpctl.SyclQueue("gpu")

incbs = IncrementalBasicStatistics(result_options=["mean", "max", "sum"])

# We do partial_fit for each batch and then print final result.
X_1 = np.array([[0, 1], [0, 1]], sycl_queue=queue)) # <-- Highlighted: sycl_queue=queue
result = incbs.partial_fit(X_1)

X_2 = np.array([[1, 2]], sycl_queue=queue)) # <-- Highlighted: sycl_queue=queue
result = incbs.partial_fit(X_2)

X_3 = np.array([[1, 1], [1, 2], [2, 3]], sycl_queue=queue)) # <-- Highlighted: sycl_queue=queue
result = incbs.partial_fit(X_3)

print(f"Mean:\n{result.mean_}")
print(f"Max:\n{result.max_}")
print(f"Sum:\n{result.sum_}")
print(f"Sum Squares:\n{result.sum_squares_}")

# We put the whole data to fit method, it is split automatically and then
# partial_fit is called for each batch.
incbs = IncrementalBasicStatistics(result_options=["mean", "max", "sum"], batch_size=3)
X = np.array([[0, 1], [0, 1], [1, 2], [1, 1], [1, 2], [2, 3]], sycl_queue=queue)) # <-- Highlighted: sycl_queue=queue
result = incbs.fit(X)

print(f"Mean:\n{result.mean_}")
print(f"Max:\n{result.max_}")
print(f"Sum:\n{result.sum_}")
print(f"Sum Squares:\n{result.sum_squares_}")
```
More examples about how to compute [covariance](https://github.com/uxlfoundation/scikit-learn-intelex/blob/main/examples/sklearnex/incremental_covariance.py) and perform [linear regression](https://github.com/uxlfoundation/scikit-learn-intelex/blob/main/examples/sklearnex/incremental_linear_regression.py) incrementally can be found in Intel's scikit-learn-intelex GitHub repo.
