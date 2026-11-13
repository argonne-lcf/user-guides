---
description: "Use Python on Crux: venv with mpi4py, batch jobs, and shared-filesystem guidance."
tags:
  - Python
---

# Python on Crux

For general guidance on creating and using Python environments, see [Python Environments](../../dev-environment/python-environments.md).

Crux does not yet provide a centrally managed conda module with GPU- or CPU-optimized frameworks. Until a prebuilt environment is available from operations, create a local `venv` and install the packages you need. The example below installs `mpi4py`:

```bash linenums="1"
python3 -m venv /lus/eagle/projects/<project>/venvs/crux-test
source /lus/eagle/projects/<project>/venvs/crux-test/bin/activate
pip install --upgrade pip
pip install mpi4py
```

Prefer a project file system (e.g. `/lus/eagle` or `/lus/grand`) over `$HOME` for the environment to avoid quota pressure and to keep I/O off the smaller home file system. For large-scale imports, avoid placing many small files in `$HOME`.

This environment can then be used in a batch job. The following `hello_world.py` example is available in the [Getting Started repository](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Crux/python):

```python linenums="1" title="hello_world.py"
--8<-- "./GettingStarted/Examples/Crux/python/hello_world.py"
```

Submit with a job script that activates the environment on the compute nodes:

```bash linenums="1" title="submit.sh"
#!/bin/bash -l
#PBS -l select=2:system=crux
#PBS -l place=scatter
#PBS -l walltime=00:10:00
#PBS -q debug
#PBS -A <project>
#PBS -l filesystems=home:eagle

cd $PBS_O_WORKDIR
source /lus/eagle/projects/<project>/venvs/crux-test/bin/activate

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=4
NDEPTH=2
NTHREADS=1
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth --env OMP_NUM_THREADS=${NTHREADS} python3 ./hello_world.py
```

Example output:

``` { .output .no-copy }
NUM_OF_NODES= 2 TOTAL_NUM_RANKS= 8 RANKS_PER_NODE= 4 THREADS_PER_RANK= 1
Hello World from rank 0 of 8 on x1000c0s0b1n1
Hello World from rank 1 of 8 on x1000c0s0b1n1
Hello World from rank 2 of 8 on x1000c0s0b1n1
Hello World from rank 3 of 8 on x1000c0s0b1n1
Hello World from rank 4 of 8 on x1000c0s1b0n0
Hello World from rank 5 of 8 on x1000c0s1b0n0
Hello World from rank 6 of 8 on x1000c0s1b0n0
Hello World from rank 7 of 8 on x1000c0s1b0n0
```

For general Python packaging guidance, see [Python Environments](../../dev-environment/python-environments.md) and [Support](../../support/ticket.md) if you need a package added to the system.

