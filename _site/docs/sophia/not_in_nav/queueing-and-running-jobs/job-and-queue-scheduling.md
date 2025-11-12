# Job and Queue Scheduling on ThetaGPU

## Queues and Job Scheduling

### Nodes vs Queue vs MIG mode

The GPU nodes are NVIDIA DGX A100 nodes, and each node contains eight (8) A100 GPUs. You may request either entire nodes or a single GPU based on your job needs. What you will get is determined by the queue you submit to (see the Queues section below). If it has "node" in the name, you will get nodes. If it has "GPU" in the name, you will get a single GPU. Note that if you need more than a single GPU, you should submit to the full-node queue.

Additionally, the NVIDIA A100 GPUs support a feature called “Multi-Instance GPU” (MIG) mode. This allows a single GPU to be shared by up to 7 different processes. We do not schedule at this level, but to use MIG capabilities, you may pass `--attrs mig-mode=True` with your qsub and use the `nvidia-smi_mig` command (note the UNDERSCORE) just as you would call `nvidia-smi mig ...` directly. Attempts to call `nvidia-smi mig ...` (no underscore) directly will result in an error message. The single-GPU host will, by default, not create a MIG instance, and users will have direct access to the GPU. If you are not using MIG mode, your session will appear as if it were a normal full-node system with only one GPU. Note that as of 12/13/21, MIG mode is unavailable for full-node jobs.

### Queues

There are three primary queues:

- **full-node**: This is the general production queue for jobs that require full nodes. The -n parameter in your qsub will match the resource type in this queue, i.e., -n 2 in the node queue will get you two full nodes.
- **bigmem**: 2 of the nodes have 640 GB of memory compared to the other 22 nodes with 320 GB. Use this queue to access these 2 nodes by specifying `-q bigmem` in your script. A max of 2 nodes (-n 2) can be requested in this queue.
- **single-gpu**: This is the general production queue for jobs that operate best on a single GPU. The -n parameter in your qsub should always be 1 as you can only submit to a single GPU. If you need more than 1 GPU, use the full-node queue.

Here are the initial queue limits. You may not violate either of these policies.

#### full-node queue:

- MinTime is 5 minutes
- MaxTime is 12 hours
- MaxQueued will be 20 jobs
- MaxRunning will be 10 jobs

#### bigmem queue:

- MinTime is 5 minutes
- MaxTime is 12 hours
- MaxQueued is 2 jobs
- MaxRunning is 1 job

#### single-gpu queue:

- MinTime is 5 minutes
- MaxTime is 1 hour
- MaxQueued is 1 job
- MaxRunning is 1 job

The initial queue policy will be simple First-In-First-Out (FIFO) based on priority with EASY backfill.

## Running Jobs On ThetaGPU

**Note:** Users will need an allocation on ThetaGPU to utilize the GPU nodes. Request an allocation by filling out this form: [Allocation request](https://accounts.alcf.anl.gov/#!/allocationRequest). ThetaGPU is listed under Theta on the form.

### Running on multiple GPU nodes

Until there is tighter integration of Cobalt and mpirun on GPU nodes, the user will have to identify the nodes Cobalt assigned to their job and pass them as options to mpirun along with some other mpirun options. The following shows 2 different code snippets on how to get the hosts allocated to the job and pass them to mpirun.

**Option 1 - Simplest**

```bash
mpirun -hostfile $COBALT_NODEFILE -n 16 -npernode 8 mpi-example-code
```

where `$COBALT_NODEFILE` is a file that the -hostfile option can use.

**Option 2 - A little more complicated**

```bash
HOSTS=$(cat $COBALT_NODEFILE | sed ':a;N;$!ba;s/\n/,/g')
mpirun --np 16 --host $HOSTS --oversubscribe ./mpi-example-code
```

To specifically see how the MPI ranks were assigned, one could add `--display-map --display-allocation` to the mpirun options.

### Controlling which Cobalt instance (GPU) for my commands

IF YOU ARE ONLY USING KNL NODES, NOTHING CHANGES, AND YOU CAN IGNORE THIS.

Because of the difference in architectures and limitations in Cobalt V1, we are running two Cobalt instances: the existing one for the KNL nodes, which remains as is, and a second one for the GPU nodes. You need to be able to control which instance you are interacting with, and there are several ways to do so.

- As was true in the past, if you do nothing, the commands will default to the architecture associated with the host you are on when you issue it.
- If you are on the Theta login nodes, commands will default to the KNL instance.
- If you are on a GPU node, for instance, the build nodes, then commands will default to the GPU instance.
- You can set an environment variable to control which instance the default commands (qsub, qstat, etc.) will interact with. The primary use case here will be users who only use GPU nodes but are working from the Theta login nodes. To do so, you may:
  - `module load cobalt/cobalt-knl` which would make Cobalt commands interact with the original Cobalt instance and launch jobs on the KNL nodes.
  - `module load cobalt/cobalt-gpu` which would make Cobalt commands interact with the new Cobalt instance and launch jobs on the GPU nodes.
  - You can also set `COBALT_CONFIG_FILES=<path to cobalt config>`
    - KNL config: /etc/cobalt.knl.conf
    - GPU config: /etc/cobalt.gpu.conf

You can use suffixed commands to explicitly control which instance you are interacting with. If you regularly use both types of nodes, this is the recommended path to avoid confusion and to prevent launching jobs on the wrong architecture.

All the commands you are used to are there; they take the same command line parameters, etc., they just have either -knl or a -gpu suffix on them. For instance:

- `qsub-knl <parameters>` would submit a job to the KNL nodes.
- `qstat-gpu` would check the queue status for the GPU nodes.

### Requesting DGX nodes or individual GPUs

The DGX nodes, which contain (8) A100 GPUs, are extremely powerful, and it can be very difficult for a single job to efficiently use an entire node. For this reason, you may request either full nodes (all 8 GPUs) or individual GPUs. What you are assigned (a node or a GPU) is dependent on the queue you submit to:

- If the queue name ends in -node, you will get full nodes (8 A100 GPUs).
- If the queue name ends in -gpu, you will get an individual GPU.
- The -n parameter on the qsub is the number of resources of the type in that queue. So, for example:
  - `qsub -n 2 -q full-node <rest of command line>` would get you two full DGX nodes, which would be a total of (16) A100 GPUs.
  - `qsub -n 2 -q single-gpu <rest of command line>` would get you two A100 GPUs.

For reservations, you can only have one queue, and the resources in the queue need to be consistent, so your entire reservation must be in nodes or GPUs. If you need both, you will need two reservations, one for each type of resource.

- Node names are of the form thetagpu## where ## ranges from 01 to 24. This is an entire node (8 GPUs).
- GPU names are of the form thetagpu##-gpu# where the GPU numbers range from 0-7.

### Multi-Instance GPU (MIG) mode

The A100 GPUs have a capability known as [Multi-Instance GPU (MIG)](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html). This allows a single A100 GPU to be reconfigured at a hardware level down to a maximum of 7 instances. The valid configurations are shown in a table on the MIG page referenced above. These instances appear as a GPU to the application. In order to use this feature, the GPU must be put into MIG mode, and this requires a reset of the GPU. At the current time, we are not supporting scheduling at the MIG level. However, a user can request that their GPU be put in MIG mode, and then they can reconfigure the GPU into a supported configuration from their job script.

If you wish to have the resources you have requested put into MIG mode, you can add either of these to your qsub command line: `--attrs mig-mode=True`.

### Details of a job submission

Details of the job submission are recorded in the `<jobid>.cobaltlog`. This file contains the qsub command and environment variables. The location of this file can be controlled with the `qsub --debuglog <path>` that defaults to the same place as the `.output` and `.error` files.

### Jobs stuck in "starting" state

If you submit a job and qstat shows it in "starting" state for 5 minutes or more, most likely your memory/numa mode selection requires rebooting some or all of the nodes your job was assigned. This process takes about 15 minutes, during which your job appears to be in the "starting" phase. When no reboots are required, the "starting" phase only lasts a matter of seconds.

### "utime" and "stime"

At the bottom of a `<jobid>.output` file, there is usually a line like:

```
Application 3373484 resources: utime ~6s, stime ~6s, Rss ~5036, inblocks ~0, outblocks ~8
```

The "utime" and "stime" values are user CPU time and system CPU time from the aprun and getrusage commands. They are rounded aggregate numbers scaled by the number of resources used and are approximate. The aprun man page has more information about them.

### `#COBALT` directives on the second line of job script

If `#COBALT` directives are used inside a job submission script, then they must appear at the topmost lines of the script. `#COBALT` directives following a blank line will be ignored. Attempting to qsub the following example script will lead to the error message below.

```
> cat submit.csh #!/bin/csh #COBALT -n 2 -t 2:00:00 -q full-node mpirun -np 20 -npernode 10 ./my_app > qsub submit.csh Usage: qsub.py [options] [] Refer to man pages for JOBID EXPANSION and SCRIPT JOB DIRECTIVES. No required options provided
```

A correct submission script would look like the following with the blank line removed.

```
> cat submit.csh

#!/bin/csh

#COBALT -n 2 -t 2:00:00 -q full-node

mpirun -np 20 -npernode 10 ./my_app

> qsub submit.csh

12345
```

## Job Submission on ThetaGPU

The queuing system used on ThetaGPU is [Cobalt](https://git.cels.anl.gov/aig-public/cobalt). On ThetaGPU, Cobalt jobs may run either as script jobs or interactive mode jobs.

## Script Method

In the script method, Cobalt will execute a user-supplied script when running a user’s job. Following are the required flags to `qsub`, as well as some of the more common options. A complete list of options may be found as a part of the `qsub` manpage, available on any login node.

### Required Flags

```
-n NN - number of nodes (-n 16 for 16 nodes)
-t time - running time (-t 30 for 30 minutes, -t 01:10:20 for 1 hr 10 min 20 sec) 
-A Project - project (-A YourProject)
```

### Common options

```
--attrs - you may specify additional attributes for your job. 
          Multiple attribute key-value pairs are colon-delimited. 
          The following are common on the KNL nodes: 
          - filesystem: a comma-separated list of filesystems used while running your job
          - location: a comma-separated list of node ids. Ranges may be hyphenated. 
          - mcdram: The desired MCDRAM mode of a job (default: cache) 
          - numa: The desired NUMA mode of a job (default: quad)

          The following are common on the GPU nodes:          
          - location: a comma-separated list of node names (not IDs). Ranges may NOT be hyphenated on the GPU nodes. 
          - mig-mode: Should the GPUs be put in Multi Instance GPU (MIG) mode (default: False) 
          - pubnet: Enable public network connectivity from compute nodes
        
See: https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html


--env VAR1=1:VAR2=2:… - specify required environment variables
-i file - give a file name to be used for stdin 
-O Name - name your job and stdout/stderr (-O Job1) 
-q queue - queue name (full-node, single-gpu, bigmem) (default: full-node)
```

**Note:** Remember to give all options before the executable name.

Users will need an allocation on ThetaGPU to utilize the GPU nodes. [Request for an allocation](https://accounts.alcf.anl.gov/#!/allocationRequest) on ThetaGPU through our Director's Discretionary award. ThetaGPU is listed under Theta on the form.

Users will need to load the `cobalt/cobalt-gpu` module before issuing a `qsub` command to access ThetaGPU.

### Example Script

```bash
module load cobalt/cobalt-gpu

qsub -A YourProject -n 4 -t 30 -q full-node \
--env MYVAR=value1 -i inputdata -O Project1_out \
--attrs filesystems=home,eagle \
program.exe progarg1
```

The syntax for Cobalt scripting is slightly different than that of a PBS script. For more information, see [Cobalt scripting](https://git.cels.anl.gov/aig-public/cobalt/-/wikis/cmdref/CommandReference).

## Interactive Method

To run an “interactive mode” job on ALCF Cray resources, add the “-I” (uppercase "i", not a lowercase "L") flag or “--mode interactive” to your qsub line and omit any executable. Your qsub submission will then wait until nodes are allocated to your job, and Cobalt will start a shell on a job-launch node on your behalf. You may aprun against your assigned resources and run other interactive commands from this node. It is important to note that your shell is executed from a launch node and not from your compute head-node. Once your allocation ends, all apruns will be terminated, but your shell will remain for any cleanup actions that you choose to take.

## Ensemble Jobs

Users may run an “ensemble job” and combine runs into a single script. This can provide major enhancements to throughput, especially for large ensemble jobs. Users may run multiple jobs in sequence or may use multiple backgrounded apruns to subset their resources among multiple backend executables. There is a system limitation of 1000 simultaneous apruns per Cobalt script job.

### Submitted Job with the Wrong Arguments

If you submit a job with the wrong arguments, you can modify it without deleting it and resubmitting it. Most settings can be changed using `qalter`.

#### Example

```bash
Usage: qalter [-d] [-v] -A <project name> -t <time in minutes>
             --attrs filesystems=<filesystem>
             -e <error file path> -o <output file path>
             --dependencies <jobid1>:<jobid2>
             -n <number nodes of> -h --proccount <processor count>
             -M <email address> <jobid1> <jobid2>
```

**Note:** To change the queue, use `qmove`.

```bash
Usage: qmove <queue name> <jobid> <jobid>
```

## Changing Executable after Job Submission

When a job is submitted via qsub, Cobalt records the path to the executable or script, but it does not make a copy. As a result, if the executable or script is modified when there is a deletion or modification, it will affect any jobs already submitted that use that executable. To avoid confusion, it is generally best to avoid making changes after job submission.

## Holding and Releasing Jobs

### User Holds

To hold a job (prevent it from running), use `qhold`. This will put the job in the `user_hold` state.

```bash
qhold <jobid>
```

To release a job in a user hold (`user_hold`) state, use `qrls`.

```bash
qrls <jobid>
```

A job may also be put into a user hold immediately upon submission by passing `qsub` the `-h` flag.

```bash
qsub -n 8 -t 60 --attrs filesystems=home,eagle -A MyProject -h myExe
```

### Dependency Holds

For jobs in the `dep_hold` or `dep_fail` state, see the section on job dependencies.

### Admin Holds

Jobs in the state `admin_hold` may be released only by a system administrator.

### MaxRun Holds

Jobs may temporarily enter the state `maxrun_hold` if the user has reached the limit of per-user running jobs in a particular queue. No action is required; as running jobs complete, jobs in the `maxrun_hold` state will be automatically changed back to queued and eligible to run.

## Job Dependencies

To submit a job that waits until another job or jobs have completed, use the dependencies argument to qsub. For example, to submit a job that depends on job 12345:

```bash
qsub -n 2 -t 10 --attrs filesystems=theta-fs0,eagle,home -A yourproject --dependencies 12345 a.out
```

For multiple dependencies, list and separate with colons.

```bash
qsub -n 2 -t 30 -A yourproject --attrs filesystems=theta-fs0,eagle,home --dependencies 12345:12346 a.out
```

Jobs submitted with dependencies will remain in the state `dep_hold` until all the dependencies are fulfilled, then will proceed to the state queued.

**Note:** In the event any of the dependencies do not complete successfully (nonzero exit status), the job will instead go into the state `dep_fail`. To manually release a job that is in either `dep_hold` or `dep_fail`:

```bash
qrls --dependencies <jobid>
```

or alternatively change the job's dependencies setting to "none":

```bash
qalter --dependencies none <jobid>
```

## Customizing the Output of `qstat`

Default fields displayed by the `qstat` command may be changed by setting the `QSTAT_HEADER` environment variable.

```bash
export QSTAT_HEADER="JobID:JobName:User:WallTime:RunTime:Nodes:State:attrs:Queue"
qstat

JobID   JobName                           User      WallTime  RunTime   Nodes  State      attrs             Queue
     =======================================================================================================================================
     104927  N/A                               user1     02:00:00  01:20:45  128    running    {'numa': 'quad', 'mcdram': 'cache'}  backfill
     104941  N/A                               user2     00:20:00  N/A       2048   queued     {'numa': 'quad', 'mcdram': 'flat'}   backfill
     104934  xxxx.yyyyy                        user3     04:00:00  01:10:12  32     running    {'numa': 'quad', 'mcdram': 'cache'}  default
     104948  Xxx-YY_ZZ                         user4     02:00:00  00:15:03  128    running    {'numa': 'quad', 'mcdram': 'cache'}  default
     104919  aaaaa_0000_bbbb_c                 user5     06:00:00  01:50:21  64     running    {'numa': 'quad', 'mcdram': 'cache'}  default
     104945  aaaa_bb_cccc-d_eee_f.hhhhhh.iiii  user6     06:00:00  00:18:25  100    running    {'numa': 'quad', 'mcdram': 'cache'}  default
     104848  bbbbbb                            user7     01:00:00  N/A       3624   queued     {'numa': 'quad', 'mcdram': 'cache'}  default
     ....
```

One may specify column headers via the `--header` flag to `qstat`.

Available field names can be seen by entering `qstat -fl <jobid>` for any current jobid.

## Redirecting Standard Input

To redirect the standard input to a job, do not use the `<` redirection operator on the `qsub` command line. This simply redirects standard input to `qsub`, not the job itself. Instead, use the qsub option `-i`.

```bash
# WRONG
qsub -t 10 -n 1 --attrs filesystems=home a.out < my_input_file.dat

# RIGHT
qsub -t 10 -n 1 --attrs filesystems=home -i my_input_file.dat a.out
```

## Project Names

You can find active project names that your account is associated with by running the command:

```bash
sbank allocations
```

If an account is associated with more than one project, a job must be submitted by using a specific project name using `-A` or by setting the environment variable `COBALT_PROJ`.

## Sbank

The sbank database is updated hourly. This means transactions against your account can take up to an hour before they show up.

## Submitting into Backfill Nodes

Sometimes the scheduler will try to clear up room for a large job. During these times, although not many jobs may be running, new jobs are not being scheduled as expected.

At such times, backfill nodes may be available. While nodes are being drained for a larger job, other user jobs may be backfilled onto these resources, provided that their requested wall time is less than the remaining drain time of the set of resources. For instance, suppose that 16 nodes are being drained to allow a 16-node job to run. Of the 16 nodes, perhaps eight are empty, and the other eight are running an eight-node job that has 2 hours of wall time left. This allows the opportunity to run a 2-hour, eight-node job in the backfill here.

To discover available backfill, run the nodelist command. This command can only be run on the service nodes (`thetagpusn1-2`), it cannot be run on the compute nodes.

### Example

```bash
nodelist
Host             Queue                                       State      Backfill
==================================================================================
[...]
thetagpu16       CompBioAffin:backfill:full-node             down       -       
thetagpu16-gpu0  single-gpu                                  idle       -       
thetagpu16-gpu1  single-gpu                                  allocated  -       
thetagpu16-gpu2  single-gpu                                  allocated  -       
thetagpu16-gpu3  single-gpu                                  allocated  -       
thetagpu16-gpu4  single-gpu                                  idle       -       
thetagpu16-gpu5  single-gpu                                  idle       -       
thetagpu16-gpu6  single-gpu                                  idle       -       
thetagpu16-gpu7  single-gpu                                  idle       -       
thetagpu17       CompBioAffin:backfill:full-node             allocated  -       
thetagpu17-gpu0  single-gpu                                  down       -       
thetagpu17-gpu1  single-gpu                                  down       -       
thetagpu17-gpu2  single-gpu                                  down       -       
thetagpu17-gpu3  single-gpu                                  down       -       
thetagpu17-gpu4  single-gpu                                  down       -       
thetagpu17-gpu5  single-gpu                                  down       -       
thetagpu17-gpu6  single-gpu                                  down       -       
thetagpu17-gpu7  single-gpu                                  down       -
[...]
```

In this example, a four-node job with a maximum wall time of 4 hours and 59 minutes can be run during this backfill. The backfill times will not always be identical and will depend on the mix of jobs on the partitions that are being drained.

## Submitting to Specific Nodes

In rare cases, there may be a need to target specific hardware. This may be accomplished using `--attrs location=`.

### Example

```bash
qsub -t 10 -n 2 --attrs filesystems=eagle location=thetagpu01:thetagpu02 myprog.exe 
```

This will force the job to run on those specific nodes. Should that location become unschedulable, for instance, due to a failed node, the job will not be allowed to run anywhere else, without resetting the location attribute. If more nodes are specified in the location field than are required to fill a job’s requested node count, then the first n nodes available in the location set will be used.

## Running with a Group of Users

Sometimes it is useful to allow other users to run Cobalt commands on a given job such as `qhold`, `qrls`, or `qdel`. A list of users can be allowed to run commands on your job by submitting a list of users to `qsub`, `cqsub`, or `qalter` using the flag `--run_users`. Specified users need not be in the same project under which the job was submitted.

### Example

```bash
qsub -t 10 -n 16 --attrs filesystems=eagle,eagle,home location=thetagpu01:thetagpu02 --run_users frodo:sam:pippin myprog.exe
```

As a convenience, all users belonging to the project under which a job was submitted can be added to a list of users that may control a job by using the `--run_project` flag.

Users who have been added to the list can run any command that the job-submitter could run on a job. This includes `qhold`, `qrls`, `qalter`, and `qdel`.

### Group Running and File System Groups

While setting this list of users allows any of the listed users to run Cobalt commands on a job, it does not do anything about the permissions of any files involved with the job. Those must be handled by the user(s) setting appropriate permissions on their directories to allow users in their group to read and write files as appropriate. If your project needs a group on the file system to share files or a user needs to be added, email [User Support](mailto:support@alcf.anl.gov).

## More Information

For more information on Cobalt commands and their options, consult the manpages on the system. The same information may be found online in the [Cobalt Command Reference](http://trac.mcs.anl.gov/projects/cobalt/wiki/CommandReference).

## Using the Job Resource Manager: Commands, Options, and Examples

This document provides examples of how to submit jobs on our systems. It also provides examples of commands that can be used to query the status of jobs, what partitions are available, etc. For an introduction to using the job resource manager and running jobs on ThetaGPU, see [Running Jobs on ThetaGPU](https://github.com/argonne-lcf/alcf-userguide/blob/main/docs/theta-gpu/queueing-running-jobs/index.md).

### Submit a Job Request

Use `qsub` to submit a job. Unlike jobs on the ALCF Blue Gene systems, all jobs on ThetaGPU are either script or interactive.

Run the script `jobscript.sh` with 2 nodes for a maximum of 15 minutes:

```bash
qsub -n 2 -t 15 --attrs filesystems=theta-fs0 jobscript.sh
```

To submit jobs to a particular queue, use `qsub -q <queue_name>`.

### Charge a Job to a Project

Use `qsub -A <project_name>` to charge a job to a particular project.

To run `jobscript.sh` with 2 nodes for a maximum of 15 minutes and charge the job to MyProject:

```bash
qsub -n 2 -t 15 --attrs filesystems=eagle,home -A MyProject jobscript.sh
```

To see which projects you are a member of:

```bash
projects
```

You can use the environment variable `COBALT_PROJ` to set your default project. qsub -A takes precedence over `COBALT_PROJ`.

### Delete a Job from the Queue

To delete a job from the queue, use the qdel command. For example, for a job with ID of 34586.

```bash
qdel 34586
```

Depending on the stage of a job’s lifetime, qdel may not complete immediately, especially if the delete is issued during startup on a job that is changing memory modes and rebooting a node. If the job does not ultimately terminate, contact [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with the jobid so that an administrator can take appropriate cleanup actions and administratively terminate the job.

### Query Partition Availability

To determine which partitions are currently available to the scheduler, use the nodelist command. This command provides a list of node ids, names, queue, and state as well as any backfill windows. This command can only be run on the service nodes (`thetagpusn1-2`), it cannot be run on the compute nodes.

For example, on `thetagpusnX`, it displays:

```bash
Host             Queue                                       State      Backfill
================================================================================== 
[...]
thetagpu16       CompBioAffin:backfill:full-node             down       -       
thetagpu16-gpu0  single-gpu                                  idle       -       
thetagpu16-gpu1  single-gpu                                  allocated  -       
thetagpu16-gpu2  single-gpu                                  allocated  -       
thetagpu16-gpu3  single-gpu                                  allocated  -       
thetagpu16-gpu4  single-gpu                                  idle       -       
thetagpu16-gpu5  single-gpu                                  idle       -       
thetagpu16-gpu6  single-gpu                                  idle       -       
thetagpu16-gpu7  single-gpu                                  idle       -       
thetagpu17       CompBioAffin:backfill:full-node             allocated  -       
thetagpu17-gpu0  single-gpu                                  down       -       
thetagpu17-gpu1  single-gpu                                  down       -       
thetagpu17-gpu2  single-gpu                                  down       -       
thetagpu17-gpu3  single-gpu                                  down       -       
thetagpu17-gpu4  single-gpu                                  down       -       
thetagpu17-gpu5  single-gpu                                  down       -       
thetagpu17-gpu6  single-gpu                                  down       -       
thetagpu17-gpu7  single-gpu                                  down       -       
thetagpu18       CompBioAffin:backfill:full-node             allocated  -       
[...]
```

## Specifying Filesystems

On ThetaGPU and other systems running Cobalt at the ALCF, your job submission should specify which filesystems you will be using. In the event that a filesystem becomes unavailable, this information is used to preserve jobs that would use that filesystem while allowing other jobs that are not using an affected filesystem to proceed to run normally.

You may specify your filesystem by adding `filesystems=<list of filesystems>` to the `--attrs` argument of qsub in Cobalt. Valid filesystems are `home`, `eagle`, and `theta-fs0`. The list is comma-delimited.

For example, to request the `home` and `eagle` filesystems for your job, you would add `filesystems=home,eagle` to your `qsub` command. If this is not specified, a warning will be printed, and then **the job will be tagged as requesting all filesystems and may be held unnecessarily if a filesystem is not currently available**. The warnings are written to stderr of `qsub` and `qalter` commands that change the value of the `--attrs` flag. Scripts that are parsing stderr from these utilities may encounter errors from the additional warnings if filesystems are not specified in these commands.

If a job is submitted while a filesystem it requested is marked down, the job will automatically be placed into a `user_hold` status, and a warning message will be printed, but the job will be otherwise queued. The job is also placed into `admin_hold` status by a sysadmin script. Once the affected filesystem has been returned to normal operation, the `admin_hold` is released. You are responsible for releasing the `user_hold` once you receive the message that the affected filesystem has been returned to normal operation. The job cannot run until both the holds are released.

If a job requesting a filesystem that is marked down is already in the queue, it will be placed on `admin_hold` status and will be released once the filesystem is operational.

```bash
qsub -n 1 -t 30 -q full-node --attrs filesystems=home,eagle -A Project ./my_job.sh
```

To update the filesystems list for your job, use `qalter`. Note that `qalter --attrs` is a replace and not an update operation. This means that you should once again specify all the attributes that you had in the original `qsub` command.

```bash
qalter --attr filesystems=home,eagle:mig-mode=True <jobid>
```

To release user hold:

```bash
qrls <jobid>
```

## References

<iframe width="560" height="315" src="https://www.youtube.com/embed/s2U18IO88-s" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

---