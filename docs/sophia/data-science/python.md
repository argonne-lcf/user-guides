# Python

We provide prebuilt `conda` environments containing GPU-supported builds of `torch`, `tensorflow` (both with `horovod` support for multi-node calculations), `jax`, and many other commonly-used Python modules.

Users can activate this environment by first loading the `conda` module, and then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
module use /soft/modulefiles; module load conda; conda activate base
```

This will load and activate the base environment.

## Virtual environments via `venv`

To install additional packages that are missing from the `base` environment, we can build a `venv` on top of it.

!!! success "Conda `base` environment + `venv`"

    If you need a package that is **not** already installed in the `base` environment, this is generally the recommended approach.

    We can create a `venv` on top of the base Anaconda environment (with `--system-site-packages` to inherit the `base` packages):

    ```bash
    module use /soft/modulefiles; module load conda; conda activate base
    CONDA_NAME=$(echo ${CONDA_PREFIX} | tr '\/' '\t' | sed -E 's/mconda3|\/base//g' | awk '{print $NF}')
    VENV_DIR="$(pwd)/venvs/${CONDA_NAME}"
    mkdir -p "${VENV_DIR}"
    python -m venv "${VENV_DIR}" --system-site-packages
    source "${VENV_DIR}/bin/activate"
    ```

You can always retroactively change the `--system-site-packages` flag state for this virtual environment by editing `${VENV_DIR}/pyvenv.cfg` and changing the value of the line `include-system-site-packages=false`.

To install a different version of a package that is already installed in the base environment, you can use:

```bash
python3 -m pip install --ignore-installed <package> # or -I
```

The shared base environment is not writable, so it is impossible to remove or uninstall packages from it. The packages installed with the above `pip` command should shadow those installed in the base environment.

## Cloning the base Anaconda environment

!!! warning

    This approach is generally not recommended as it can be quite slow and can use significant storage space.

If you need more flexibility, you can clone the conda environment into a custom path, which would then allow for root-like installations via `conda install <module>` or `pip install <module>`.

Unlike the `venv` approach, using a cloned Anaconda environment requires you to copy the entirety of the base environment, which can use significant storage space.

To clone the `base` environment:

```bash
module load conda; conda activate base
conda create --clone base --prefix /path/to/envs/base-clone
conda activate /path/to/envs/base-clone
```

where `/path/to/envs/base-clone` should be replaced by a suitably chosen path.

**Note**: The cloning process can be _quite_ slow.

## Using `pip install --user` (not recommended)

!!! danger

    This is typically _not_ recommended.

With the conda environment setup, one can install common Python modules using `python3 -m pip install --user '<module-name>'` which will install packages in `$PYTHONUSERBASE/lib/pythonX.Y/site-packages`.

The `$PYTHONUSERBASE` environment variable is automatically set when you load the base conda module, and is equal to `/home/$USER/.local/polaris/conda/YYYY-MM-DD`.

Note, Python modules installed this way that contain command line binaries will not have those binaries automatically added to the shell's `$PATH`. To manually add the path:

```bash
export PATH="$PYTHONUSERBASE/bin:$PATH"
```

Be sure to remove this location from `$PATH` if you deactivate the base Anaconda environment or unload the module.

Cloning the Anaconda environment, or using `venv` are both more flexible and transparent when compared to `--user` installs.

## Default Python Version

The default Python on Sophia is located at `/usr/bin/python` with version 3.9.18.

## Creating a Jupyter Kernel

If you would like to use your Python virtual environment on JupyterHub, you will need to create a Jupyter kernel for it.

1. **Install `ipykernel`**:
    Ensure `ipykernel` is installed in your virtual environment:
    ```bash
    pip install ipykernel
    ```

2. **Create a Jupyter Kernel**:
    ```bash
    python -m ipykernel install --user --name=myenv --display-name "Jupyter (myenv)"
    ```
    Replace `myenv` with the name of your virtual environment and `"Jupyter (myenv)"` with the display name you want for the kernel in JupyterHub.