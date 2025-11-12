# Job Queueing and Submission

## Introduction

ALCF's Graphcore POD64 system uses Slurm for job submission and queueing. Below are some of the important commands for using Slurm. For more information refer to [Slurm Documentation](https://slurm.schedmd.com/).

>**NOTE: Jobs that require IPUs will fail unless launched with `srun` or `sbatch`.**
>**NOTE: There is a single Slurm scheduler for the Graphcore POD64.**

## SRun

The Slurm command `srun` can be used to run individual Python scripts (or other programs) in parallel with other scripts on a cluster managed by Slurm. An example of `srun` usage is shown below. Use the `--ipus=` option to specify the number of IPUs required for the run.

Example:

```console
srun --ipus=1 python mnist_poptorch.py

```

## SBatch

Alternatively, these jobs can be submitted to the Slurm workload manager through a batch script by using the `sbatch` command. To do this, create a bash script (submit-mnist-poptorch-job.sh here as an example) with the commands that you want to execute.

```bash
#!/bin/sh

python mnist_poptorch.py
```

Then pass the bash script as an input to the `sbatch` command as shown below, requesting the number of IPUs required:

```console
sbatch --ipus=1 --output=mnist-poptorch-output.log submit-mnist-poptorch-job.sh
```

<!--- See [DataParallel](DataParallel.md) for additional information. --->

## SQueue

The `squeue` command provides information about jobs located in the Slurm scheduling queue.

```console
$ squeue
             JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
              2572       p64 Graphcor username  R       1:12      1 gc-poplar-02
```

## SInfo

SInfo is used to view partition and node information for a system running Slurm.

```console
$ sinfo
PARTITION AVAIL  TIMELIMIT  NODES  STATE NODELIST
p64*         up   infinite      3   idle gc-poplar-[02-04]
```

For more information, see [SInfo](https://slurm.schedmd.com/sinfo.html).

## SCancel

SCancel is used to signal or cancel jobs, job arrays, or job steps.

```bash
scancel job_id
```
