# Python

We provide prebuilt `conda` environments containing GPU-supported builds of
`torch`, `tensorflow` (both with `horovod` support for multi-node
calculations), `jax`, and many other commonly-used Python modules.

Users can activate this environment by first loading the `conda` module, and
then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
module use /soft/modulefiles; module load conda ; conda activate base
```

This will load and activate the base environment.

## Virtual environments via `venv`

To install additional packages that are missing from the `base` environment,
we can build a `venv` on top of it.

!!! success "Conda `base` environment + `venv`"

    If you need a package that is **not** already
    installed in the `base` environment,
    this is generally the recommended approach.

    We can create a `venv` on top of the base
    Anaconda environment (with
    `#!bash  --system-site-packaes` to inherit
    the `base` packaes):

    ```bash
    module use /soft/modulefiles ; module load conda; conda activate base
    CONDA_NAME=$(echo ${CONDA_PREFIX} | tr '\/' '\t' | sed -E 's/mconda3|\/base//g' | awk '{print $NF}')
    VENV_DIR="$(pwd)/venvs/${CONDA_NAME}"
    mkdir -p "${VENV_DIR}"
    python -m venv "${VENV_DIR}" --system-site-packages
    source "${VENV_DIR}/bin/activate"
    ```

You can always retroactively change the `#!bash --system-site-packages` flag
state for this virtual environment by editing `#!bash ${VENV_DIR}/pyvenv.cfg` and
changing the value of the line `#!bash include-system-site-packages=false`.

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
path, which would then allow for root-like installations via `#!bash conda install
<module>` or `#!bash pip install <module>`.

Unlike the `venv` approach, using a cloned Anaconda environment requires you to
copy the entirety of the base environment, which can use significant storage
space.

To clone the `base` environment:

```bash
module load conda ; conda activate base
conda create --clone base --prefix /path/to/envs/base-clone
conda activate /path/to/envs/base-clone
```

where, `#!bash path/to/envs/base-clone` should be replaced by a suitably chosen
path.

**Note**: The cloning process can be _quite_ slow.

## Using `pip install --user` (not recommended)

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
transparent when compared to `#!bash --user` installs.

## Existing issue and solution

There is an issue with the current conda environment. One may encounter the following error message: 

```bash
aborting job:
MPIDI_CRAY_init: GPU_SUPPORT_ENABLED is requested, but GTL library is not linked
```

To addresss this, please add the following line in the very beginning of your python script. 

```python
from mpi4py import MPI
```
## Creating a Jupyter Kernel
If you need to use your python `venv` on JupyterHub, you will need to create a Jupyter kernel for it.
1. Install ipykernel: install `ipykernel` on your `venv` <br>
`pip install ipykernel`
2. Create a Jupyter Kernel:
`python -m ipykernel install --user --name=venv --display-name "Jupyter (venv)"` <br>
Replace `"Jupyter (myenv)"` with the display name you want for the kernel in JupyterHub