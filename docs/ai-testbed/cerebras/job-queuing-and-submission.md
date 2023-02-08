## Job Queuing and Submission
The CS-2 systems use slurm for job submission and queueing.<br>
**`csrun_cpu`** is used to run a cpu-only job on one or more worker nodes.<br>
**`csrun_wse`** is used to run a job on both the wafer scale engine and one or more worker nodes.

Your job will be blocked until there are available resources.<br>
Scheduling is in first in, first out (FIFO) order.
```bash
# csrun_cpu [--help] [--alloc_node] [--mount_dirs] command_to_execute
csrun_cpu --help
# csrun_wse [--help] [--total-nodes] [--tasks_per_node] [--cpus_per_task] [--mount_dirs] command_for_cs_execution
csrun_wse --help
#
# squeue is used to inspect the job queue
# squeue [OPTIONS]
squeue -a
# scancel is used to cleanly kill a job
# scancel [OPTIONS] [job_id[_array_id][.step_id]]
scancel JOBID
```

Note: slurm jobs using the wafer (started using `csrun_wse`) will show with two parts in the `squeue -a` output. The jobids will be the base ID plus 0 or 1, e.g. 555+0 and 555+1. To cancel using `scancel`, give just the base ID as the argument, e.g. `scancel 555`. 

You can find a detailed documentation for slurm
[here](https://slurm.schedmd.com/documentation.html) or use `--help` to see a
summary of options for slurm [commands](https://slurm.schedmd.com/quickstart.html#commands), i.e. `squeue --help`.<br>
See some examples of how these commands are used to submit and queue jobs in section [Steps to run a model/program](Steps-to-run-a-model-or-program.md).
