# Python

## Conda
We provide prebuilt `conda` environments containing GPU-supported builds of `torch`, `tensorflow` (both with `horovod` support for multi-node calculations), `jax`, and many other commonly-used Python modules. 

Users can activate this environment by first loading the `conda` module, and then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
$ module load conda
$ conda activate base
(base) $ which python3
/soft/datascience/conda/2022-07-19/mconda3/bin/python3
```
In one line, `module load conda; conda activate`. This can be performed on a compute node, as well as a login node. 

As of writing, the latest `conda` module on Polaris is built on Miniconda3 version 4.13.0 and contains Python 3.8.13. Future modules may contain entirely different major versions of Python, PyTorch, TensorFlow, etc.; however, the existing modules will be maintained as-is as long as feasible. 

While the shared Anaconda environment encapsulated in the module contains many of the most commonly used Python libraries for our users, you may still encounter a scenario in which you need to extend the functionality of the environment (i.e. install additional packages)

There are two different approaches that are currently recommended.

### Virtual environments via `venv`

Creating your own (empty) virtual Python environment in a directory that is writable to you is simple:
```bash
python3 -m venv /path/to/new/virtual/environment
```
This creates a new folder that is fairly lightweight folder (<20 MB) with its own Python interpreter where you can install whatever packages you'd like. First, you must activate the virtual environment to make this Python interpreter the default interpreter in your shell session.

You activate the new environment whenever you want to start using it via running the activate script in that folder:
```bash
/path/to/new/virtual/environment/bin/activate
```

In many cases, you do not want an empty virtual environment, but instead want to start from the `conda` base environment's installed packages, only adding and/or changing a few modules.

To extend the base Anaconda environment with `venv` (e.g. `my_env` in the current directory) and inherit the base enviroment packages, one can use the `--system-site-packages` flag:

```bash
module load conda; conda activate
python -m venv --system-site-packages my_env
source my_env/bin/activate
# Install additional packages here...
```
You can always retroactively change the `--system-site-packages` flag state for this virtual environment by editing `my_env/pyvenv.cfg` and changing the value of the line `include-system-site-packages = false`.

To install a different version of a package that is already installed in the base
environment, you can use:
```
pip install --ignore-installed  ... # or -I
```
The shared base environment is not writable, so it is impossible to remove or uninstall
packages from it. The packages installed with the above `pip` command should shadow those
installed in the base environment.

### Cloning the base Anaconda environment

If you need more flexibility, you can clone the conda environment into a custom path, which would then allow for root-like installations via `conda install <module>` or `pip install <module>`. Unlike the `venv` approach, using a cloned Anaconda environment requires you to copy the entirety of the base environment, which can use significant storage space. 

This can be performed by:

```bash
$ module load conda
$ conda activate base
(base) $ conda create --clone base --prefix /path/to/envs/base-clone
(base) $ conda activate /path/to/envs/base-clone
(base-clone) $ which python3
/path/to/base-clone/bin/python3
```
The cloning process can be quite slow. 

!!! warning

    In the above commands, `path/to/envs/base-clone` should be replaced by a
    suitably chosen path.

### Using `pip install --user` (not recommended)
With the conda environment setup, one can install common Python modules using `pip install --users <module-name>` which will install packages in `$PYTHONUSERBASE/lib/pythonX.Y/site-packages`. The `$PYTHONUSERBASE` environment variable is automatically set when you load the base conda module, and is equal to  `/home/$USER/.local/polaris/conda/YYYY-MM-DD`.

Note, Python modules installed this way that contain command line binaries will not have those binaries automatically added to the shell's `$PATH`. To manually add the path:
```
export PATH=$PYTHONUSERBASE/bin:$PATH"
```
Be sure to remove this location from `$PATH` if you deactivate the base Anaconda environment or unload the module. 

Cloning the Anaconda environment, or using `venv` are both more flexible and transparent when compared to `--user` installs. 
