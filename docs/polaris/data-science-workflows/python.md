# Python

## Conda
We provide a pre-built `conda` environment containing GPU-supported builds of both `pytorch` and `tensorflow` (with `horovod` support).

Users can activate this environment by first loading the `conda` module, and then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
$ module load conda/2022-07-19
$ conda activate base
(base) $ which python3
/soft/datascience/conda/2022-07-19/mconda3/bin/python3
```

### Clone the Base Environment

To extend this environment (e.g. install additional packages), users can create a clone which they are free to modify.

This can be done by:

```bash
$ module load conda/2022-07-19
$ conda activate base
(base) $ conda create --clone base --prefix /path/to/envs/base-clone
(base) $ conda activate /path/to/envs/base-clone
(base-clone) $ which python3
/path/to/base-clone/bin/python3
```

!!! warning

    In the above commands, `path/to/envs/base-clone` should be replaced by a
    suitably chosen path.


