# Job Queueing and Submission

## Introduction

SambaNova uses Slurm for job submission and queueing. Below are some of the important commands for using Slurm. For more information refer to [Slurm Documentation](https://slurm.schedmd.com/).

> **Note**: Run the Python scripts using 'srun' or 'sbatch', to ensure that concurrent jobs do not interfere with each other.

> **Note**: There is just one scheduler for all of the SambaNova nodes.

## SRun

The Slurm command `srun` can be used to run individual Python scripts in parallel with other scripts on a cluster managed by Slurm. Examples of `srun` usage are shown below.

Slurm will assign a **nodelist/host** to run a job if a host is not specified.

Example:

```bash
srun python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
srun python lenet.py run --pef="pef/lenet/lenet.pef"
```

You may specify which node/host on which to run a job.

Reasons to specify a node list:

- One wants to test a specific node to verify the function of the HW and SW  (daily smoke tests do this)
- The nodes are at different software levels and one wants to use a node that has the needed software level for one's application.

Example:

```bash
srun --nodelist=sn30-r1-h1 python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
```

## SBatch

Alternatively, these jobs can be submitted to the Slurm workload manager through a batch script by using the `sbatch` command. To do this, create a bash script (submit-lenet-job.sh here as an example) with the commands that you want to execute.

```bash
#!/bin/sh

python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
python lenet.py run --pef="pef/lenet/lenet.pef"
```

Then pass the bash script as an input to the `sbatch` command as shown below.

```bash
sbatch --output=pef/lenet/output.log submit-lenet-job.sh
```

In case of the need to use multiple RDUs (2 in the example shown below), the `sbatch` command would be altered as:

```bash
sbatch --gres=rdu:2 <your_script.sh>
```
<!--- See [DataParallel](DataParallel.md) for additional information. --->

## SQueue

The `squeue` command provides information about jobs located in the Slurm scheduling queue.

```bash
squeue
```

## SInfo

SInfo is used to view partition and node information for a system running Slurm.

Here is a suggested command:

```bash
sinfo -O AllocNodes, GresUsed, Gres, NodeList
```

For more information, see [SInfo](https://slurm.schedmd.com/sinfo.html).

## SCancel

SCancel is used to signal or cancel jobs, job arrays, or job steps.

```bash
scancel job_id
```
