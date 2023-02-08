# Conda on Theta
[Conda](https://conda.io/docs/) is a popular package and virtual environment management framework that is used for managing python packages. ALCF has installed this framework, with some default package that users can use for simulation, analysis, and machine learning on Theta.

## Getting Started
Adding Conda to your environment:

```
module load conda/2021-09-22
conda activate
```
This will load Python 3. The installed modules are listed at the bottom of this page. At the time of writing, this is the most recent Anaconda build available on Theta that includes an optimized `mpi4py` library linked against the Cray MPICH installed on the machine. 

If all the Python packages you need are installed, then you can use this module as is. However, if you need custom modules installed, have a look at the section below about Installing Custom Python Modules.

## Probing the environment
Full Conda documentation can be [found](https://conda.io/docs/user-guide/getting-started.html) here, but we'll cover a few useful things here. After the module is loaded, one can list the Python modules installed and their versions using `conda list`


### Adding custom python modules via `pip`
You can add custom python modules via `pip install --user <module-name>` and this module will be installed in `$HOME/.local/lib/python3.X/site-packages` which is always part of the `sys.path` in Python. This can help if you only need a few extra packages or small additions. For big changes see the next section on custom environments.

## Installing Custom Conda Environment
In order to add custom Python modules to the conda environment, one must create a custom conda environment. This can be done using:

```
conda create -p </path/to/new/env> --clone $CONDA_PREFIX
```

This creates a custom environment in the path you specify and installs everything that existed in the base conda installation. Be aware that the actual conda binary will always come from the base installation.

Next you can move into this custom environment with `conda activate </path/to/new/env>`

Now you can install your own packages.

```
conda install <python-module>
```

## References
[Python for HPC: Best Practices](https://www.alcf.anl.gov/support-center/training-assets/python-hpc-best-practices)

<iframe width="560" height="315" src="https://www.youtube.com/embed/Dt8-wKLFCRw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>




