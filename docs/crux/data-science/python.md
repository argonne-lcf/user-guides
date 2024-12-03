# Python

At a future date, we will provide prebuilt `conda` environments containing CPU-optimized
builds `torch`, `tensorflow` (both with `horovod` support for multi-node calculations),
`jax`, and many other commonly-used Python modules.

Users can activate this environment by first loading the `conda` module, and
then activating the base environment.

Explicitly (either from an interactive job, or inside a job script):

```bash
module use /soft/modulefiles; module load conda ; conda activate base
```

This will load and activate the base environment.
