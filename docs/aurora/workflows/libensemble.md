# libEnsemble on Aurora

[libEnsemble](https://libensemble.readthedocs.io/en/main/) is a Python toolkit for running dynamic ensembles of calculations.

Users provide generator and simulator functions to express their ensembles, where the generator can steer the ensemble based on previous results. These functions can portably submit user executables at any scale.

System details are detected, and dynamic resource management is provided. This includes automatically detecting, assigning, and reassigning GPUs for ensemble members.

libEnsemble can be used in a consistent manner on laptops, clusters, and supercomputers with minimal required dependencies.

## Configuring Python and Installation

To obtain Python use:

    module use /soft/modulefiles
    module load frameworks

To obtain libEnsemble::

    pip install libensemble

See the ALCF docs for more details on using [Python on Aurora](https://docs.alcf.anl.gov/aurora/data-science/python/).


# Example

To run the [forces_gpu](https://libensemble.readthedocs.io/en/main/tutorials/forces_gpu_tutorial.html) tutorial on Aurora.

To obtain the example, you can clone the `libEnsemble` repository—although only the `forces` sub-directory is needed:

```bash
git clone https://github.com/Libensemble/libensemble
cd libensemble/libensemble/tests/scaling_tests/forces/forces_app
```

To compile `forces` (a C application with OpenMP target):

```bash
mpicc -DGPU -O3 -fiopenmp -fopenmp-targets=spir64 -o forces.x forces.c
```

Now go to the `forces_gpu` directory:

```bash
cd ../forces_gpu
```

To use all available GPUs, open `run_libe_forces.py` and adjust the exit criteria to perform more simulations. The following will run two simulations per worker:

```python
# Instruct libEnsemble to exit after this many simulations
ensemble.exit_criteria = ExitCriteria(sim_max=nsim_workers*2)
```

Now grab an interactive session on two nodes (or use the batch script at `../submission_scripts/submit_pbs_aurora.sh`):

```bash
qsub -A <myproject> -l select=2 -l walltime=15:00 -lfilesystems=home -q EarlyAppAccess -I
```

Once in the interactive session, you may need to reload the frameworks module:

```bash
cd $PBS_O_WORKDIR
module use /soft/modulefiles
module load frameworks
```

Then, run:

```bash
python run_libe_forces.py --comms local --nworkers 13
```

This provides twelve workers for running simulations (one for each GPU across two nodes). An extra worker runs the persistent generator. GPU settings for each worker simulation are printed.

Looking at `libE_stats.txt` will provide a summary of the runs.

## Using Tiles as GPUs

To treat each tile as its own GPU, add the `use_tiles_as_gpus=True` option to the `libE_specs` block in `run_libe_forces.py`:

```python
ensemble.libE_specs = LibeSpecs(
    num_resource_sets=nsim_workers,
    sim_dirs_make=True,
    use_tiles_as_gpus=True,
)
```

Now, rerun with twice the workers:

```bash
python run_libe_forces.py --comms local --nworkers 25
```

The `forces` example will automatically use the GPUs available to each worker (one MPI rank per GPU). If fewer workers are provided, multiple GPUs will be used per simulation.

Also, see `forces_gpu_var_resources` and `forces_multi_app` examples for cases using varying processor/GPU counts per simulation.

## Demonstration

A [video demonstration](https://youtu.be/H2fmbZ6DnVc) of the `forces_gpu` example on *Frontier* is available. The workflow is identical when running on *Aurora*, except for different compiler options and numbers of workers due to differing GPU counts per node.

More details:
- [libEnsemble Documentation](https://libensemble.readthedocs.io)
- [libEnsemble github page](https://github.com/Libensemble/libensemble)
- [libEnsemble Documentation Aurora page](https://libensemble.readthedocs.io/en/main/platforms/aurora.html)
