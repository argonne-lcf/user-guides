# Running PyTorch with Conda

Be aware that these builds use CUDA and will not work on login nodes, which do not have CUDA installed as there are no GPUs.

One can test these software packages in an interactive session:
```bash
qsub -I -q single-gpu -n 1 -t 30 -A <project-name> --attrs filesystems=<list of filesystems>
```

## PyTorch (master build)

Users can find the latest builds via the `module avail` conda command, which will list available builds such as `conda/2021-06-26`, which is a module that was built on 2021-06-26. Use `module show conda/2021-06-26` or `module help conda/2021-06-26` to get high-level info on which versions of the key packages and libraries this particular module contains.

This version can be used by:
```bash
module load conda/2021-06-26 # loads conda into your environment, sets up appropriate CUDA libraries and environment variables
conda activate # add entries to PATH for the environment and run any activation scripts that the environment may contain
```

This will set up a conda environment with the "from scratch" build of PyTorch.

This package will also include builds of TensorFlow and Horovod tagged releases.

## Installing Packages

### Using `pip install --user` (not recommended)

With the conda environment set up, one can install common Python modules using `pip install --user <module-name>`, which will install packages in `$PYTHONUSERBASE/lib/pythonX.Y/site-packages`. The `$PYTHONUSERBASE` environment variable is automatically set when you load the base conda module and is typically equal to `/home/$USER/.local/conda/YYYY-MM-DD` or `/home/$USER/.local/thetagpu/conda/YYYY-MM-DD`, depending on the date of the module.

Note, Python modules installed this way that contain command-line binaries will not have those binaries automatically added to the shell's `$PATH`. To manually add the path:
```bash
export PATH=$PYTHONUSERBASE/bin:$PATH
```
Be sure to remove this location from `$PATH` if you deactivate the base Anaconda environment or unload the module.

Cloning the Anaconda environment or using `venv` are both more flexible and transparent when compared to `--user` installs.

### Using Conda Environments

If you need more flexibility, you can clone the conda environment into a custom path, which would then allow for root-like installations via `conda install <module>` or `pip install <module>`.

1. Set up the conda environment you want to use as instructed above.
2. Create/edit your `$HOME/.condarc` file to include these lines, replacing `<project-name>` with your project name. `<path-to-your-project>` is the path to the file system your project is on (e.g., `/lus/theta-fs0` or `/eagle`). By default, Conda will use your `$HOME/.conda/*` area for caching files.

**Note:** Since home directories are limited to 100GB, this fills up quickly. This addition tells Conda to use your project space for cache storage instead.

```yaml
pkgs_dirs: 
  - <path-to-your-project>/<project-name>/conda/pkgs 
envs_dirs: 
  - <path-to-your-project>/<project-name>/conda/envs
```

3. Clone the environment into a local path to which you have write access:
```bash
conda create --clone $CONDA_PREFIX -p <path/to/env>
```
4. Activate that environment:
```bash
conda activate <path/to/env>
```

One should then be able to install modules natively.