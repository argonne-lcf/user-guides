# Conda on Theta
[Conda](https://conda.io/docs/) is a popular package and virtual environment management framework that is used for managing python packages. ALCF has installed this framework, with some default package that users can use for simulation, analysis, and machine learning on Theta.

## Getting Started
Adding Conda to your environment

```
module load miniconda-3
```

This will load Python 3. The installed modules are listed at the bottom of this page.

If all the python packages you need are installed, then you can use this module as is. However, if you need custom modules installed, have a look at the section below about Installing Custom Python Modules.

## Probing the environment
Full Conda documentation can be [found](https://conda.io/docs/user-guide/getting-started.html) here, but we'll cover a few useful things here. After the module is loaded, one can list the python modules installed and their versions using ```conda list``

### Adding custom python modules via pip
You can add custom python modules via `pip install --user <module-name>` and this module will be installed in `$HOME/.local/lib/python3.X/site-packages` which is always part of the `sys.path` in python. This can help if you only need a few extra packages or small additions. For big changes see the next section on custom environments.

## Installing Custom Conda Environment
In order to add custom python modules to the conda environment, one must create a custom conda environment. This can be done using (example for python 3.6 conda):

```
conda create -p </path/to/new/env> --clone $CONDA_PREFIX
```

This creates a custom environment in the path you specify and installs everything that existed in the base conda installation. Be aware that the actual conda binary will always come from the base installation.

Next you can move into this custom environment with `source activate </path/to/new/env>`

Now you can install your own packages.

```
conda install <python-module>
```

## Installing Horovod for Distributed Deep Learning
If you install conda manually, or find your conda environment does not have Horovod. We provide a [Conda Channel alcf-theta](https://anaconda.org/alcf-theta) which contains pre-compiled Horovod packages that work with most of the latest PyTorch and TensorFlow versions. In principle you can install Horovod using pip install horovod however this can be a painful process to get the environment correctly lined up to work on Theta. Thus, we provide the precompiled versions. Therefore we suggest using the following on Theta:

```
conda install -c alcf-theta horovod
```


## References
[Python for HPC: Best Practices](https://www.alcf.anl.gov/support-center/training-assets/python-hpc-best-practices)

<iframe width="560" height="315" src="https://www.youtube.com/embed/Dt8-wKLFCRw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>




