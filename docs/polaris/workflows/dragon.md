# Dragon

[DragonHPC](https://dragonhpc.org/portal/index.html) is a composable distributed runtime for managing processes, memory, and data at scale through high-performance communication.  Dragon is an open source project developed by HPE.

Dragon has a Python API and a C/c++ API.  The Python API is an extension of Python's `multiprocessing` API, and therefore DragonHPC can be used to scale Python `multiprocessing` code across multi-node jobs.  It can be installed with pip in conda or Python virtual environments.

DragonHPC allows parallel process launching, including PMI enabled processes for MPI applications, with fine-grained control of CPU/GPU affinity.  DragonHPC also has a distributed data layer or distributed dictionary that allows for in-memory data sharing between processes on different nodes.

Please see DragonHPC's [Introduction in their documenation](https://dragonhpc.github.io/dragon/doc/_build/html/start.html) for examples of how to use `dragon`.

## Installation

To install in a Python virtual environment on Polaris:

```shell
module use /soft/modulefiles
module load conda
conda activate base
python -m venv _env
source _env/bin/activate
pip install dragonhpc
dragon-config add --ofi-runtime-lib=/opt/cray/libfabric/1.22.0/lib64
```

The last installation step that calls `dragon-config` is necessary to enable `dragon` to use high-speed RDMA transfers across Polaris' Slingshot network.  Skipping this step will result in `dragon` using slower TCP transfers for cross node communication and data transfer.

## Execution of Dragon Driver Scripts

Dragon driver scripts written with the Python API should be executed with the `dragon` application:

```shell
dragon my_dragon_script.py
```

Alternatively, it can be launched with the python binary but the `-m` flag should be set to `dragon`:

```shell
python -m dragon my_dragon_script.py
```

Dragon needs access to the PBS `qstat` command in order to run (this is used to determine the nodes have been allocated by PBS as opposed to SLURM or LSF and determines how dragon will discover nodes and launch the runtime).  In some interactive jobs on Polaris, you may need to modify the `PATH` to ensure `qstat` is visible to `dragon`:

```shell
export PATH=/opt/pbs/bin:$PATH
```

Currently, we also recommend unloading the `xalt` module on Polaris when running Dragon:

```shell
module unload xalt
```

## Policies for Polaris Nodes

The `dragon` object that sets CPU, GPU and node affinities for processes is the Dragon `Policy`.

A common `Policy` setting on Polaris is to run one process per GPU (four per node).  

Dragon's Native Pool can use policies to run a pool of processes that bind each process to specific GPUs and CPUs on specific nodes. (Note that the `multiprocessing` Pool with `dragon` selected as the start method can only distribute processes across multiple nodes; it cannot set explicit GPU and CPU binding affinities).  Here is an example of how to run a pool across nodes on Polaris binding 1 pool process per GPU with the Native Dragon Pool:

```python
import dragon
from dragon.infrastructure.policy import Policy
from dragon.native.machine import System, Node
from dragon.native.pool import Pool

# List of optimal bindings for GPUs to CPUs on a Polaris node
gpu_affinities = [[3],[2],[1],[0]]
cpu_affinities = [list(range(c, c+8)) for c in range(0, 32, 8)]
num_gpus_per_node = 4

# A simple function to demonstrate task execution
def hello_world(message):
    # do some work on the gpu here...                        
    return f"Hello {message}"

if __name__ == '__main__':

    # Get list of nodes in the runtime       
    alloc = System()
    nodelist = alloc.nodes

    # Create a policy for every GPU in the runtime              
    polaris_policies_for_gpus = []
    for node in nodelist:
        node_name = Node(node).hostname
        for i in range(num_gpus_per_node):
            pol = Policy(host_name=node_name,
                        cpu_affinity=cpu_affinities[i],
                        gpu_affinity=gpu_affinities[i],
                        placement=Policy.Placement.HOST_NAME,)
            polaris_policies_for_gpus.append(pol)

    messages = [f'pool_task_{i}' for i in range(32)]
    dragon_pool = Pool(policy=polaris_policies_for_gpus, processes_per_policy=1)
    async_results = dragon_pool.map_async(hello_world, messages)
    results = async_results.get()
    for res in results:
        print(res, flush=True)
    dragon_pool.close()
    dragon_pool.join()

```

Here is an example of how to do a similar thing with a `ProcessGroup`:

```python
import os
import dragon
from dragon.infrastructure.policy import Policy
from dragon.native.machine import System, Node
from dragon.native.process_group import ProcessGroup
from dragon.native.process import ProcessTemplate

# List of optimal bindings for GPUs to CPUs on a Polaris node
gpu_affinities = [[3],[2],[1],[0]]
cpu_affinities = [list(range(c, c+8)) for c in range(0, 32, 8)]
num_gpus_per_node = 4

# A simple function to demonstrate task execution and GPU affinity
def hello_world(message):
    # do some work on the gpu here...
    return f"Hello {message}"

if __name__ == '__main__':

    # Get list of nodes in the runtime
    alloc = System()
    nodelist = alloc.nodes

    # Create a policy for every GPU in the runtime
    polaris_policies_for_gpus = []
    for node in nodelist:
        node_name = Node(node).hostname
        for i in range(num_gpus_per_node):
            pol = Policy(host_name=node_name,
                        cpu_affinity=cpu_affinities[i],
                        gpu_affinity=gpu_affinities[i],
                        placement=Policy.Placement.HOST_NAME,)
            polaris_policies_for_gpus.append(pol)

    pg = ProcessGroup()
    for pol in polaris_policies_for_gpus:
        pg.add_process(nproc=1, 
                       template=ProcessTemplate(target=hello_world,
                                                args=('hello',),
                                                cwd=os.getcwd(),
                                                policy=pol,))
    pg.init()
    pg.start()
    pg.join()
    pg.close()
```

## Distributed Dictionaries

Dragon offers a distributed data layer called a Dragon Dictionary.  The Dragon Dictionary or `DDict` can span all nodes or a subset of nodes in your runtime and can also use Policies for optimal node placement.  Regardless of whether a `DDict` spans all nodes or a subset, any process in the runtime on any node can access the dictionary.

To create a `DDict` that spans all nodes in the runtime:
```python
from dragon.native.machine import System
from dragon.data.ddict import DDict

alloc = System()
num_nodes = int(alloc.nnodes)
dict_mem_per_node = 1 * 1024**3 # ask for 1 GB per node, expressed in units of bytes
# Note that the total_mem is the total memory across all the nodes
dist_dict = DDict(managers_per_node=1, n_nodes=num_nodes, total_mem=num_nodes*dict_mem_per_node)
```
For more details on how to use Dragon Dictionaries, see the DragonHPC [documentation](https://dragonhpc.github.io/dragon/doc/_build/html/start.html#data).

## Running MPI applications

MPI applications can be run with the Dragon `ProcessGroup`.  To enable message passing between processes in a Dragon ProcessGroup on Polais set the `pmi` flag when creating the process group like this:

```python
from dragon.native.process_group import ProcessGroup
from dragon.infrastructure.facts import PMIBackend

pg = ProcessGroup(pmi=PMIBackend.CRAY) 
```
Processes can be added to the `ProcessGroup` according to your application needs.  See the DragonHPC [documentation](https://dragonhpc.github.io/dragon/doc/_build/html/uses/orchestrate_mpi.html) on orchestrating MPI applications.
