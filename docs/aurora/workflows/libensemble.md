# libEnsemble on Aurora

[libEnsemble](https://libensemble.readthedocs.io/en/main/) is a Python toolkit for running dynamic ensembles of calculations.

Users provide generator and simulator functions to express their ensembles, where the generator can steer the ensemble based on previous results. These functions can portably submit user executables at any scale.

System details are detected, and dynamic resource management is provided. This includes automatically detecting, assigning, and reassigning GPUs for ensemble members.

libEnsemble can be used in a consistent manner on laptops, clusters, and supercomputers with minimal required dependencies.

## Configuring Python and Installation

To obtain Python and create a virtual environment:

```bash linenums="1"
module load frameworks
python -m venv /path/to-venv --system-site-packages
. /path/to-venv/bin/activate
```

where `/path/to-venv` can be anywhere you have write access. For future sessions, just load the frameworks module and run the activate line.

To obtain libEnsemble, execute:

```bash linenums="1"
pip install libensemble
```

See the ALCF docs for more details on using [Python on Aurora](../data-science/python.md).


# Example

To run the [forces_gpu](https://libensemble.readthedocs.io/en/main/tutorials/forces_gpu_tutorial.html) tutorial on Aurora.

To obtain the example, you can clone the `libEnsemble` repository--- although only the `forces` sub-directory is needed:

```bash linenums="1"
git clone https://github.com/Libensemble/libensemble
cd libensemble/libensemble/tests/scaling_tests/forces/forces_app
```

To compile `forces` (a C application with OpenMP target):

```bash linenums="1"
mpicc -DGPU -O3 -fiopenmp -fopenmp-targets=spir64 -o forces.x forces.c
```

Now go to the `forces_gpu` directory:

```bash linenums="1"
cd ../forces_gpu
```

To use all available GPUs, open `run_libe_forces.py` and adjust the exit criteria to perform more simulations. The following will run two simulations per worker:

```python linenums="1"
# Instruct libEnsemble to exit after this many simulations
ensemble.exit_criteria = ExitCriteria(sim_max=nsim_workers*2)
```

Now grab an interactive session on two nodes (or use the batch script at `../submission_scripts/submit_pbs_aurora.sh`):

```bash linenums="1"
qsub -A <myproject> -l select=2 -l walltime=15:00 -lfilesystems=home:flare -q debug -I
```

Once in the interactive session, you may need to reactivate your virtual environment:

```bash linenums="1"
cd $PBS_O_WORKDIR
. /path/to-venv/bin/activate
```

Then, run:

```bash linenums="1"
python run_libe_forces.py -n 13
```

This provides twelve workers for running simulations (one for each GPU across two nodes). An extra worker runs the persistent generator. GPU settings for each worker simulation are printed.

Looking at `libE_stats.txt` will provide a summary of the runs.

Try running

```bash linenums="1"
./cleanup.sh
python run_libe_forces.py -n 7
```

And you will see it runs with two cores (MPI ranks) and two GPUs are used per worker.

## Live viewing GPU usage

To see GPU usage, SSH into a compute node you are on in another window and run:

```bash linenums="1"
module load xpu-smi
watch -n 0.1 xpu-smi dump -d -1 -m 0 -n 1
```

## Using Tiles as GPUs

To treat each tile as its own GPU, add the `use_tiles_as_gpus=True` option to the `libE_specs` block in `run_libe_forces.py`:

```python linenums="1"
ensemble.libE_specs = LibeSpecs(
    num_resource_sets=nsim_workers,
    sim_dirs_make=True,
    use_tiles_as_gpus=True,
)
```

Now, rerun with twice the workers:

```bash linenums="1"
python run_libe_forces.py -n 25
```

The `forces` example will automatically use the GPUs available to each worker (one MPI rank per GPU). If fewer workers are provided, multiple GPUs will be used per simulation.

Also, see `forces_gpu_var_resources` and `forces_multi_app` examples for cases using varying processor/GPU counts per simulation.


## Running generator on the manager

An alternative is to run the generator on a thread on the manager. The
number of workers can then be set to the number of simulation workers.

Change the `libE_specs` in **run_libe_forces.py** as follows:

```python linenums="1"
nsim_workers = ensemble.nworkers

# Persistent gen does not need resources
ensemble.libE_specs = LibeSpecs(
    gen_on_manager=True,
```

then the first run we did will use 12 instead of 13 workers:

```bash linenums="1"
python run_libe_forces.py -n 12
```

## Dynamic resource assignment

In the forces directory you will also find:

- `forces_gpu_var_resources` uses varying processor/GPU counts per simulation.
- `forces_multi_app` uses varying processor/GPU counts per simulation and also uses two different user executables, one which is CPU-only and one which uses GPUs. This allows highly efficient use of nodes for multi-application ensembles.

## Demonstration

A [video demonstration](https://youtu.be/H2fmbZ6DnVc) of the `forces_gpu` example on *Frontier* is available. The workflow is identical when running on *Aurora*, except for different compiler options and numbers of workers due to differing GPU counts per node.

More details:

- [libEnsemble Documentation](https://libensemble.readthedocs.io)
- [libEnsemble GitHub page](https://github.com/Libensemble/libensemble)
- [libEnsemble Documentation Aurora page](https://libensemble.readthedocs.io/en/main/platforms/aurora.html)
