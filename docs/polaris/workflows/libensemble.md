# libEnsemble

[libEnsemble](https://libensemble.readthedocs.io/en/main/) is a Python toolkit for running dynamic ensembles of calculations.

Users provide generator and simulator functions to express their ensembles, where the generator can steer the ensemble based on previous results. These functions can portably submit external executables at any scale.

System details are detected, and dynamic resource management is provided. This includes automatically detecting, assigning, and reassigning
GPUs for ensemble members.

libEnsemble can be used in a consistent manner on laptops, clusters, and supercomputers with minimal required dependencies.

## Getting libEnsemble on Polaris

libEnsemble is provided on Polaris in the **conda** module:

    module load conda
    conda activate base

See the docs for more details on using [python on Polaris](https://docs.alcf.anl.gov/polaris/data-science-workflows/python/).

<details>
  <summary>Example: creating virtual environment and updating libEnsemble</summary>

    E.g., to create a virtual environment that allows installation of
    further packages with pip:

    ```bash
    python -m venv /path/to-venv --system-site-packages
    . /path/to-venv/bin/activate
    ```

    Where ``/path/to-venv`` can be anywhere you have write access.
    For future uses just load the conda module and run the activate line.

    You can also ensure you are using the latest version of libEnsemble:

    ```bash
    pip install libensemble
    ```
</details>


## libEnsemble examples

For a very simple example of using libEnsemble see the [Simple Introduction tutorial](https://libensemble.readthedocs.io/en/main/tutorials/local_sine_tutorial.html)

For an example that runs a small ensemble using a C application (offloading work to the GPU), see
[the GPU app tutorial](https://libensemble.readthedocs.io/en/main/tutorials/forces_gpu_tutorial.html).
The required files for this tutorial can be found
in [this directory](https://github.com/Libensemble/libensemble/tree/main/libensemble/tests/scaling_tests/forces).
A [video demo](https://youtu.be/Ff0dYYLQzoU) is also available.

## Job Submission

libEnsemble runs on the compute nodes on Polaris using either Python's
``multiprocessing`` or ``mpi4py``. The user can set the number of workers for
maximum concurrency. libEnsemble will detect the nodes available
from the PBS environment and use these for running simulations. Polaris supports
running multiple concurrent simulations on each node if desired.

A simple example batch script for a libEnsemble use case that runs five workers on one node:

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

Docs: <https://libensemble.readthedocs.io> <br>
GitHub: <https://github.com/Libensemble/libensemble>

