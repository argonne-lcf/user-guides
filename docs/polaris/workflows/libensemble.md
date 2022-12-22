# libEnsemble

libEnsemble is a Python toolkit for running dynamic ensembles of calculations. Users provide generator and simulator functions to express their ensembles, where the generator can steer the ensemble based on previous results. A library of example functions is available which can be modified as needed. These functions can submit external executables at any scale and in a portable way. System details are detected, and dynamic resource management is provided. libEnsemble can be used in a consistent manner on laptops, clusters, and supercomputers with minimal required dependencies.

## Getting libEnsemble on Polaris

libEnsemble is provided on Polaris in the **conda** module:

    module load conda
    conda activate base

See the docs for more details on using [python on Polaris](https://www.alcf.anl.gov/support/user-guides/polaris/data-science-workflows/python/index.html).

## libEnsemble examples

For a very simple example of using libEnsemble see the [Simple Sine tutorial](https://libensemble.readthedocs.io/en/main/tutorials/local_sine_tutorial.html)

For an example that runs a small ensemble using a C application (offloading work to the GPU), see
[the tutorial](https://libensemble.readthedocs.io/en/main/tutorials/forces_gpu_tutorial.html).

The required files for the second tutorial can be found in [this directory](https://github.com/Libensemble/libensemble/tree/develop/libensemble/tests/scaling_tests/forces).

## Job Submission

libEnsemble runs on the compute nodes on Polaris using either
``multi-processing`` or ``mpi4py``. The user can set the number of workers for
maximum concurrency. libEnsemble will detect the nodes available
from the PBS environment and use these for running simulations. Polaris supports
running multiple concurrent simulations on each node if desired,

A simple example batch script for a libEnsemble use case that runs 5 workers (one
generator and four simulators) on one node:

```shell
    #!/bin/bash -l
    #PBS -l select=1:system=polaris
    #PBS -l walltime=00:15:00
    #PBS -l filesystems=home:grand
    #PBS -q debug
    #PBS -A <myproject>

    export MPICH_GPU_SUPPORT_ENABLED=1
    cd $PBS_O_WORKDIR
    python run_libe_forces.py --comms local --nworkers 5
```

The script can be run with:

    qsub submit_libe.sh

Or you can run an interactive session with:

    qsub -A <myproject> -l select=1 -l walltime=15:00 -lfilesystems=home:grand -qdebug -I

## Further links

Docs:  https://libensemble.readthedocs.io
github: https://github.com/Libensemble/libensemble

