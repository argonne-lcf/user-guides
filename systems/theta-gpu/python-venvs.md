# Python virtual environments

## Base Python3.8 installation

A Miniconda base environment using Python 3.8 and containing optimized builds of Tensorflow and Horovod is available by sourcing the setup script:

```bash
source /lus/theta-fs0/software/thetagpu/conda/tf_master/latest/mconda3/setup.sh
```

## Extending with virtualenv

To extend this base environment with virtualenv and inherit the base enviroment packages, one can use the `--system-site-packages` flag:

```bash
python -m venv --system-site-packages my_env
source my_env/bin/activate
# Install additional packages here...
```

## Extending with conda
If you prefer to use `conda` to manage your environment, refer to the [conda
environments page](ml_frameworks/tensorflow/running_with_conda.md) for instructions on
cloning the base environment.

## Combining virtualenvs with containers
If you wish to extend the Python environment built into a Singularity container, refer
to the notes on [building on top of a container](building_python_packages.md).
