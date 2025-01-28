# Jax on Aurora

## Overview

Jax is a library for high-performance machine learning research and numerical computing. It is designed to enable fast and flexible experimentation with new models and algorithms. Aurora is a supercomputer that provides the computational power needed to run large-scale Jax applications.

## Getting Started

To get started with Jax on Aurora, you need to follow these steps:

1. **Access Aurora**: Ensure you have the necessary permissions and access to the Aurora supercomputer.
2. **Set Up Environment**: Load the appropriate modules and set up your environment for Jax.
3. **Run Jax Applications**: Submit your Jax applications to the Aurora job scheduler.

## Access Aurora

To access Aurora, you must have an account and be part of a project that has been granted access. You can find more information on how to apply for access on the [Aurora Access Page](https://www.alcf.anl.gov/support-center/aurora-access).

## Set Up Environment

Once you have access, you need to set up your environment to use Jax. Load the necessary modules by executing the following commands:

```bash
module load jax
module load python
```

Ensure that you have the correct version of Python and Jax installed. You can verify the installation by running:

```bash
python -c "import jax; print(jax.__version__)"
```

## Run Jax Applications

To run your Jax applications on Aurora, you need to submit a job script to the job scheduler. Below is an example of a simple job script:

```bash
#!/bin/bash
#SBATCH --job-name=jax_job
#SBATCH --output=output.txt
#SBATCH --error=error.txt
#SBATCH --time=01:00:00
#SBATCH --partition=compute

module load jax
module load python

python your_jax_script.py
```

Submit the job script using the following command:

```bash
sbatch your_job_script.sh
```

## Troubleshooting

If you encounter any issues, refer to the [Aurora Troubleshooting Guide](https://www.alcf.anl.gov/support-center/aurora-troubleshooting) for assistance.

## Additional Resources

For more information on Jax and its capabilities, visit the [Jax Documentation](https://jax.readthedocs.io/).