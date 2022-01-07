# Running TensorFlow with Conda

Beware that these builds use CUDA and will not work on login nodes, which does not have CUDA installed as there are no GPUs.

One can test these software in an interactive session:
```bash
qsub -I -n 1 -t 30 -A <project-name>
```

## Tensorflow (master build)

Given A100 and CUDA 11 are very new, we have a build of the master branch of Tensorflow which includes better performance and support for these architectures.

Users can find the latest builds via the `module avail tensorflow` command, which will list available builds such as `conda/tensorflow/2021-03-02` which is a version of the master branch git repo as of the date `2021-03-02`. This version can be used by
```bash
module load conda/tensorflow/2021-03-02  # loads conda into your environment, sets up appropriate CUDA libraries
conda activate # activates conda with python 
```

This will setup a conda environment with a recent "from scratch" build of the Tensorflow repository on the master branch. 

This package will also include the latest Horovod tagged release.

## Installing Packages

### Using `pip install --user`

With the conda environment setup, one can install common Python modules using `pip install --users <module-name>` which will install packages in `$HOME/.local/lib/pythonX.Y/site-packages`.

### Using Conda Environments

If you need more flexibility, you can clone the conda environment into a custom path, which would then allow for root-like installations via `conda install <module>` or `pip install <module>`.

1. Setup the conda environment you want to use as instructed above.
2. Create/edit your `$HOME/.condarc` file to include this these lines, replacing `<project-name>` with your project name. By default, Conda will your `$HOME/.conda/*` area for caching files. Since home directories are limited to 100GB, this fills up quickly. This addition tells Conda to use your project space for cache storage instead.
```bash
pkgs_dirs:
  - /lus/theta-fs0/projects/<project-name>/conda/${ENV_NAME}/pkgs
envs_dirs:
  - /lus/theta-fs0/projects/<project-name>/conda/${ENV_NAME}/envs
```
3. Clone the environment into a local path to which you have write access
```bash
conda create --clone $CONDA_PREFIX -p <path/to/env>
```
4. Activate that environment:
```bash
conda activate <path/to/env>
```

One should then be able to install modules natively.
