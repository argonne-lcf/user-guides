# Python

We provide prebuilt `conda` environments containing GPU-supported builds of
`torch`, `tensorflow` (both with `horovod` support for multi-node
calculations), `jax`, and many other commonly-used Python modules.

Users can activate this environment by first loading the `conda` module, and
then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
module load conda ; conda activate base
```

This will load and activate the base environment.

## Virtual environments via `venv`

!!! success

    If you need a package that is already installed in the base environment,
    this is generally the recommended approach.

Creating your own (empty) virtual Python environment in a directory that is
writable to you is simple:

```bash
python3 -m venv /path/to/new/virtual/environment
```

This creates a new folder that is fairly lightweight folder (`<20 MB`) with its
own Python interpreter where you can install whatever packages you'd like.

First, you must activate the virtual environment to make this Python
interpreter the default interpreter in your shell session.

You activate the new environment whenever you want to start using it via
running the activate script in that folder:

```bash
source /path/to/new/virtual/environment/bin/activate
```

In many cases, you do not want an empty virtual environment, but instead want
to start from the `conda` base environment's installed packages, only adding
and/or changing a few modules.

!!! example "Create a `venv`"

    We can create a `venv` on top of the base Anaconda environment
    (with `#!bash  --system-site-packaes` to inherit the `base` packaes):

    ```bash
    module load conda; conda activate
    VENV_DIR="venvs/polaris"
    mkdir -p "${VENV_DIR}"
    python -m venv "${VENV_DIR}" --system-site-packages
    source "${VENV_DIR}/bin/activate"
    ```

You can always retroactively change the `--system-site-packages` flag state for
this virtual environment by editing `my_env/pyvenv.cfg` and changing the value
of the line `include-system-site-packages = false`.

To install a different version of a package that is already installed in the
base environment, you can use:

```bash
python3 pip install --ignore-installed <package> # or -I
```

The shared base environment is not writable, so it is impossible to remove or
uninstall packages from it. The packages installed with the above `pip` command
should shadow those installed in the base environment.

## Cloning the base Anaconda environment

!!! warning

    This approach is generally not recommended as it can be quite slow and can
    use significant storage space.

If you need more flexibility, you can clone the conda environment into a custom
path, which would then allow for root-like installations via `conda install
<module>` or `pip install <module>`.

Unlike the `venv` approach, using a cloned Anaconda environment requires you to
copy the entirety of the base environment, which can use significant storage
space.

- This can be performed by:

  ```bash
  $ module load conda ; conda activate base
  $ conda create --clone base --prefix /path/to/envs/base-clone
  $ conda activate /path/to/envs/base-clone
  (base-clone) $ which python3
  /path/to/base-clone/bin/python3
  ```

  where, `path/to/envs/base-clone` should be replaced by a suitably chosen
  path.

**Note**: The cloning process can be _quite_ slow.

### Using `pip install --user` (not recommended)

!!! danger

    This is typically _not_ recommended.

With the conda environment setup, one can install common Python modules using
`#!bash python3 pip install --users '<module-name>'` which will install
packages in `#!bash $PYTHONUSERBASE/lib/pythonX.Y/site-packages`.

The `#!bash $PYTHONUSERBASE` environment variable is automatically set when you
load the base conda module, and is equal to  `#!bash
/home/$USER/.local/polaris/conda/YYYY-MM-DD`.

Note, Python modules installed this way that contain command line binaries will
not have those binaries automatically added to the shell's `#!bash $PATH`. To
manually add the path:

```bash
export PATH="$PYTHONUSERBASE/bin:$PATH"
```

Be sure to remove this location from `#!bash $PATH` if you deactivate the base
Anaconda environment or unload the module.

Cloning the Anaconda environment, or using `venv` are both more flexible and
transparent when compared to `--user` installs.
