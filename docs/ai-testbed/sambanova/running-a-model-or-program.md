# Running a Model/Program

> **Note**:  Please be mindful of how you are using the system.
For example, consider running larger jobs in the evening or on weekends

> **Note**: Please use only Slurm commands, i.e., srun and sbatch, to run your code.
If you run your code directly using the 'python' command, it may cause conflicts
on the system.

> **Note**: If you have conda installed and a conda environment is active, you will see something like `(base)` at the beginning of the command prompt. If so, you will need to deactivate it with `conda deactivate`. Conda is not used on the SambaNova SN30 cluster.

## Introduction

The SambaNova workflow includes the following main steps to run a model.

1. Compile
2. Run
3. Test (optional)

The system uses the [Slurm job
scheduler](https://slurm.schedmd.com/quickstart.html) to schedule the jobs and manage the workload on the system. For more information on Slurm, see [Job Queueing and Submission](job-queuing-and-submission.md).

[Example Programs](example-programs.md) lists the different example applications with corresponding commands for each of the above steps.

## Compile

Compiles the model and generates a **.pef** file. This file contains
information on how to reconfigure the hardware, and map the compute and
memory resources required to run an application on RDUs.
The **pef** files are by default saved in the 'out' directory; the
SambaNova documentation advises saving **pef** files in separate
directories with the '--output-folder' option.

It is necessary to re-compile only when the model changes, or parameters specific to the model graph change, including the batch size.

Compile times can be significant. Compiling the UNet sample, for example, when using images of size 32x32 pixels, takes 358(s), and 1844(s) for images of size 256x256.

The entire compile process is executed on the host and no RDUs are involved in the compile step.

Example of compiling the LeNet application:

```bash
srun python lenet.py compile -b=1 --pef-name="lenet" --output-folder="pef"
```

where

| Argument               | Default   | Help                           |
|------------------------|-----------|--------------------------------|
| -b                     | 1         | Batch size for training        |
|                        |           |                                |

## Run

As part of this step, the model is trained on the RDUs by passing in the PEF file and the training dataset. The location of the **pef** file generated in the compile step is passed as an argument to the run command. Below is the example of the ```run``` command that trains a LeNet model.

```bash
srun python lenet.py run --pef="pef/lenet/lenet.pef"
```

The location of the **pef** file generated in the compile step is passed as an argument to the run command.

## Test (Optional)

This command is used to run the model on both the host CPU and a SambaNova RDU.  It compares the results from the CPU and RDU and will report if any discrepancies are found. Pass the **pef** file generated as part of the compile step as the input to this command.

```bash
srun python lenet.py test --pef="pef/lenet/lenet.pef"
```
