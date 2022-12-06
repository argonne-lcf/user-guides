# Job Scheduling and Submission on Theta 

## Job and Queue Scheduling for Theta

### General Policy
We ask that all users follow good etiquette and be kind to one another.

### Job Priority
As with all Argonne Leadership Computing Facility production systems, job priority in the queue is based on several criteria:

* positive balance of your project
* size (in nodes) of the job, larger jobs receive higher priority
* the type of project (e.g. INCITE, ALCC, or discretionary)
* job duration - shorter duration jobs will accumulate priority more quickly, so it is best to specify the job run time as accurately as possible

### Reservations and Scheduling Policy
Some work will require use of Theta that requires deviation from regular policy. On such occasions, normal reservation policy applies. Please send the [regular form](machine-reservations.md) no fewer than five (5) business days in advance.

### Monday Maintenance
When the ALCF is on a regular business schedule, preventitive maintenance is typically scheduled on alternate Mondays. The showres command may be used to view pending and active maintenance reservations.

### Big Run Mondays
As part of our regular maintenance procedures on Mondays, we will promote to the highest priority any jobs in the queued state requesting 802 nodes or more (.ie. capability jobs). Promotion is subject to operational discretion.

We may also, at our discretion, take the opportunity to promote the priority of capability jobs if the system has been drained of jobs for any other reason.

### INCITE/ALCC Overburn Policy
If an INCITE or ALCC project has exhausted its allocation in the first 11 months of its allocation year, it is eligible for overburn running. At this point, **capability jobs** submitted by INCITE and ALCC projects will run in the default queue (instead of backfill) for the first 11 months of the allocation year until 125% of the project allocation has been consumed. Note that non-capability jobs will be routed to backfill queue.

INCITE and ALCC projects needing additional overburn hours should e-mail [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with a short description of what they plan to do with the additional hours, highlighting specific goals or milestones and the time expected to accomplish them. This will be reviewed by the scheduling committee, allocations committee, and ALCF management. Requests should be submitted 15 days before the start of the next quarter of the allocation year for full consideration. Non-capability jobs from projects that have exhausted their allocation will continue to run in backfill. 

To be clear, this policy does not constitute a guarantee of extra time, and we reserve the right to prioritize the scheduling of jobs submitted by projects that have not yet used 100% of their allocations, so the earlier that an INCITE or ALCC project exhausts its allocation, the more likely it is to be able to take full advantage of this policy.

## Queues
[Information on ThetaGPU Queues](https://www.alcf.anl.gov/support/user-guides/theta-gpu/queueing-and-running-jobs/job-and-queue-scheduling/index.html)

### Theta (KNL) Queues
**Debugging Queues**

- There are two 16-node debugging queues:
> - debug-cache-quad
> - debug-flat-quad
- Hardware is dedicated to each queue
- Nodes are not rebootable to another mode
- Minimum allocation of 1 node
- Maximum allocation of 8 nodes
- Job wall-clock time is limited to 1:00:00 (1 hour).
- The maximum running job count is one (1) job per user.

**Production Queues**

- There is a single submission queue for the entire system: default
- Priority is given to jobs using at least 20% of Theta (>=802 nodes) (previously >=648 nodes)
- There is a global limit of ten (10) jobs running per user
- There is a global limit of twenty (20) jobs in queue per user
- There is a minimum job time of thirty (00:30:00) minutes for the default queue
- Beginning 21 May 2018 there is a minimum allocation of 128 nodes (previously 8 nodes)
- Jobs smaller than the minimum allocation will be allocated and charged for the minimum allocation of nodes
- While shorter jobs may accumulate priority faster, all requested wall-clock times (job durations) greater than or equal to 12 hours are treated equivalently.
- Wall-clock limits are a step-wise function designed to encourage scaling:
> - node count >= 128 nodes (minimum allocation): maximum 3:00:00 hours
> - node count >= 256 nodes : maximum 6:00:00 hours
> - node count >= 384 nodes : maximum 9:00:00 hours
> - node count >= 640 nodes : maximum 12:00:00 hours
> - node count >= 802 nodes : maximum 24:00:00 hours
- There is no default mode nodes may be assumed to be booted into. Failure to specify a mode will result in the assumption of cache-quad.

**Note:** Jobs for projects with a negative balance (with the exception of capability jobs for projects that have overburn enabled) are automatically routed to the backfill queue. Walltime limit for backfill queue is 20 minutes - 6 hours. Backfill jobs have low priority.

ALCF has 260 nodes in the production queue for a research project. The maximum job size is 4,100 nodes.

#### Running With Less Than 128 Nodes in Default Queue

It is important to remember that jobs are charged by the number of nodes allocated, not the number of nodes used. For instance, a job requesting 14 nodes, below the minimum of 128, will be allocated 128 nodes and charged for 128 even though only 14 nodes are used.

We strongly encourage you to bundle smaller jobs into ensembles, either using [Cobalt ensemble scripting](#ensemble-jobs) or using a workflow system such as [Balsam](https://balsam.alcf.anl.gov/).

### Theta (KNL) Mode Selection and Charging
Time spent booting or rebooting nodes to obtain requested modes will not be charged to projects and neither will it count against requested walltime, although it will be included in the total runtime. Please allow up to thirty (30) minutes for rebooting of nodes when submitting jobs.

Failure to specify a mode will result in the selection of cache-quad, the equivalent of listing `#!--attrs mcdram=cache:numa=quad` in your qsub or job script.

## Submit a Job
The queuing system used at ALCF is [Cobalt](https://xgitlab.cels.anl.gov/aig-public/cobalt). On Theta, Cobalt jobs may run either as script jobs or interactive mode jobs.

### Script Method
In the script method, Cobalt will execute a user-supplied script when running a user’s job. All scripts on Theta must have their execute bit set, and must be able to run on a standard x86_64 architecture. Following are the required flags to qsub, as well as some of the more common options. A complete list of options may be found as a part of the qsub manpage, available on any login node.

#### Required Flags
```
-n NN - number of nodes (-n 64 for 64 nodes) 
-t time - running time (-t 5 for 5 minutes, -t 01:10:20 for 1 hr 10 min 20 sec) 
-A Project - project (-A YourProject)
```

### Common Options
```
--attrs - you may specify additional attributes for your job. 
          Multiple attribute key-value pairs are colon-delimited. 
          The following are common on the KNL nodes: 
          - location: a comma-separated list of node ids. Ranges may be hyphenated. 
          - mcdram: The desired MCDRAM mode of a job (default: cache) 
          - numa: The desired NUMA mode of a job (default: quad)

          The following are common on the GPU nodes:          
          - location: a comma-separated list of node names (not IDs). Ranges may NOT be hyphenated on the GPU nodes. 
          - mig-mode: Should the GPUs be put in Multi Instance GPU (MIG) mode (default: False) 
        
See: [https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html)


--env VAR1=1:VAR2=2:… - specify required environment variables
-i file - give a file name to be used for stdin 
-O Name - name your job and stdout/stderr (-O Job1) 
-q queue - queue (only for special cases, such as debug queues (debug-cache-quad, debug-flat-quad))
```

**Note:** Remember to give all options before the executable name.

*See Submitting to Specific Nodes below to see the difference between KNL and GPU node location specifications. Users will need an allocation on ThetaGPU to utilize the GPU nodes. Request for an allocation by filling out this form: [Allocation request](https://accounts.alcf.anl.gov/#!/allocationRequest). ThetaGPU is listed under Theta on the form.*

**Example**
```
qsub -A YourProject -n 256  -t 30  \
--env MYVAR=value1 -i inputdata -O Project1_out \
–attrs mcdram=flat:numa=quad program.exe progarg1
```

The syntax for Cobalt scripting is slightly different than that of a PBS script. For more information, see [Cobalt scripting](https://xgitlab.cels.anl.gov/aig-public/cobalt/-/wikis/cmdref/CommandReference).

### Requesting Ability to SSH into the Compute Nodes on KNL Nodes

To be able to ssh from the MOM/launch nodes on Theta to the compute nodes, pass enable_ssh=1 as part of the --attrs argument (see the example below). 
Once the job begins on the MOM/launch node, you can ssh (or scp, etc.) from the MOM node to the compute nodes. 
The compute node name can be found by reading the $COBALT_PARTNAME number, and prepending "nid" with the appropriate number of 0s to reach 5 digits.

For example, for an interactive job:

```
n@thetalogin4:~/> qsub -I -n 1 -t 20 --attrs enable_ssh=1 -A project -q debug-cache-quad 
Connecting to thetamom1 for interactive qsub... 
Job routed to queue "debug-cache-quad". 
Memory mode set to cache quad for queue debug-cache-quad Wait for job 266815 to start... 
Opening interactive session to 3835 n@thetamom1:/gpfs/mira-home/user> echo $COBALT_PARTNAME 
3835 
n@thetamom1:/gpfs/mira-home/user> 
ssh nid03835 n@nid03835:~> hostname 
nid03835
```

### Specifying Filesystems

On Theta and other systems running Cobalt at the ALCF, your job submission should specify which filesystems your job will be using.  In the event that a filesystem becomes unavailable, this information is used to preserve jobs that would use that filesystem while allowing other jobs that are not using an affected filesystem to proceed to run normally. 

You may specify your filesystem by adding filesystems=<list of filesystems> to the --attrs argument of qsub in Cobalt. Valid filesystems are home, eagle, grand, and theta-fs0. The list is comma-delimited. For example, to request the home and eagle filesystems for your job you would add filesystems=home,eagle to your qsub command. If this is not specified, a warning will be printed and then **the job will be tagged as requesting all filesystems and may be held unnecessarily** if a filesystem is not currently available. The warnings are written to stderr of qsub and qalter commands that change the value of the --attrs flag.  Scripts that are parsing stderr from these utilities may encounter errors from the additional warnings if filesystems are not specified in these commands.

If a job is submitted while a filesystem it requested is marked down, the job will automatically be placed into a user_hold and a warning message will be printed, but the job will be otherwise queued. The job is also placed into admin_hold by a sysadmin script. Once the affected filesystem has been returned to normal operation, the admin_hold is released. **You are responsible for releasing the user_hold** once you receive the message that the affected filesystem has been returned to normal operation. The job cannot run until both the holds are released.

If a job requesting a filesystem that is marked down is already in the queue, it will be placed on admin_hold and will be released once the filesystem is operational.

An example of a job requesting filesystems:
```
qsub -n 128 -t 30 -q default --attrs filesystems=home,grand -A Project ./my_job.sh
```
          
To update the filesystems list for your job, use qalter. Note that qalter --attrs is a replace and not an update operation. This means that you should once again specify all the attributes that you had in the original qsub command.
          
```
qalter --attr filesystems=home,eagle:mcdram=cache:numa=quad <jobid>
```

To release user hold:
```
qrls <jobid>
```

### Interactive Method
To run an “interactive mode” job on ALCF Cray resources, add the “-I” flag or “--mode interactive” to your qsub line and omit any executable. Your qsub submission will then wait until nodes are allocated to your job and Cobalt will start a shell on a job-launch node on your behalf. You may aprun against your assigned resources and run other interactive commands from this node. It is important to note that your shell is executed from a launch node and not from your compute head-node. Once your allocation ends, all apruns will be terminated, but your shell will remain for any cleanup actions that you choose to take.

#### Ensemble Jobs
Users may run an “ensemble job” and combine runs into a single script. This can provide major enhancements to throughput, especially for large ensemble jobs. Users may run multiple jobs in sequence or may use multiple backgrounded apruns to subset their resources among multiple backend executables. There is a system limitation of 1000 simultaneous apruns per Cobalt script job.

### Project Names
You can find active project names that your account is associated with by running the command:
```
sbank allocations
```
If an account is associated with more than one project, a job must be submitted by using a specific project name using -A or by setting the environment variable COBALT_PROJ.

### Submitted Job with the Wrong Arguments
If you submit a job with the wrong arguments, you can modify without deleting it and resubmitting it. Most settings can be changed using qalter.

**For Example:**
```
 Usage: qalter [-d] [-v] -A <project name> -t <time in minutes>
             -e <error file path> -o <output file path>
             --dependencies <jobid1>:<jobid2>
             -n <number nodes of> -h --proccount <processor count>
             -M <email address> <jobid1> <jobid2>
```
**Note:** To change the queue, use qmove.
```
Usage: qmove <queue name> <jobid> <jobid>
```

### Changing Executable after Job Submission
When a job is submitted via qsub, Cobalt records the path to the executable or script, but it does not make a copy. As a result, if the executable or script is modified when there is a deletion or modification, it will affect any jobs already submitted that use that executable. To avoid confusion, it is generally best to avoid making changes after job submission.

### Holding and Releasing Jobs
#### User Holds
To hold a job (prevent from running), use qhold. This will put the job in the "user_hold" state.
```
qhold <jobid>
```
To release a job in a user hold (user_hold) state, use qrls.
```
qrls <jobid>
```

A job may also be put into a user hold immediately upon submission by passing qsub the -h flag.
```
qsub -n 512 -t 120 -A MyProject -h myExe
```
#### Dependency Holds
For jobs in the dep_hold or dep_fail state, see the section on job dependencies.

#### Admin Holds
Jobs in the state admin_hold may be released only by a system administrator.

#### MaxRun Holds
Jobs may temporarily enter the state maxrun_hold if the user has reached the limit of per-user running jobs in a particular queue. No action is required; as running jobs complete, jobs in the maxrun_hold state will be automatically changed back to queued and eligible to run.

### Job Dependencies
To submit a job that waits until another job or jobs have completed, use the dependencies argument to qsub. For example, to submit a job that depends on job 12345:
```
qsub -n 512 -t 10 -A yourproject --dependencies 12345 a.out
```
For multiple dependencies, list and separate with colons.
```
qsub -n 512 -t 10 -A yourproject --dependencies 12345:12346 a.out
```
Jobs submitted with dependencies will remain in the state dep_hold until all the dependencies are fulfilled, then will proceed to the state queued.

**Note:** In the event any of the dependencies do not complete successfully (nonzero exit status), the job will instead go into the state dep_fail. To manually release a job that is in either dep_hold or dep_fail:
```
qrls --dependencies <jobid>
```
or alternatively change the job's dependencies setting to “none”:
```
qalter --dependencies none <jobid>
```
### Customizing the Output of Qstat
Default fields displayed by the qstat command may be changed by setting the QSTAT_HEADER environment variable.
```
export QSTAT_HEADER="JobID:JobName:User:WallTime:RunTime:Nodes:State:attrs:Queue""
qstat

​JobID   JobName                           User      WallTime  RunTime   Nodes  State      attrs             Queue
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
One may specify column headers via the --header flag to qstat.

Available field names can be seen by entering "qstat -fl <jobid>" for any current jobid.

### Redirecting Standard Input
To redirect the standard input to a job, do not use the '<' redirection operator on the qsub command line. This simply redirects standard input to qsub, not the job itself. Instead, use the qsub option "-i".
```
# WRONG
qsub -t 10 -n 64 a.out < my_input_file.dat

# RIGHT
qsub -t 10 -n 64 -i my_input_file.dat a.out
```

### Sbank
The sbank database is updated hourly. This means transactions against your account can take up to an hour before they show up.

### Submitting into Backfill Nodes
Sometimes the scheduler will try to clear up room for a large job. During these times, although not many jobs may be running, new jobs are not being scheduled as expected.

At such times, backfill nodes may be available. While nodes are being drained for a larger job, other user jobs may be backfilled onto these resources, provided that their requested wall time is less than the remaining drain time of the set of resources. For instance, suppose that 16 nodes are being drained to allow a 16-node job to run. Of the 16 nodes, perhaps eight are empty and the other eight are running an eight-node job that has 2 hours of wall time left. This allows the opportunity to run a 2-hour, eight-node job in the backfill here.

To discover available backfill, run the nodelist command.

**For example:**
```
nodelist
Node_id  Name         Queues     Status                 MCDRAM  NUMA    Backfill
================================================================================
{...]
20       c0-0c0s5n0   default     cleanup-pending       flat    quad    4:59:44
21       c0-0c0s5n1   default     cleanup-pending       flat    quad    4:59:44
22       c0-0c0s5n2   default     busy                  flat    quad    4:59:44
24       c0-0c0s6n0   default     busy                  flat    quad    4:59:44
25       c0-0c0s6n1   default     busy                  flat    quad    4:59:44
26       c0-0c0s6n2   default     busy                  flat    quad    4:59:44
27       c0-0c0s6n3   default     busy                  flat    quad    4:59:44
28       c0-0c0s7n0   default     idle                  flat    quad    4:59:44
29       c0-0c0s7n1   default     idle                  flat    quad    4:59:44
30       c0-0c0s7n2   default     idle                  flat    quad    4:59:44
31       c0-0c0s7n3   default     idle                  flat    quad    4:59:44
32       c0-0c0s8n0   default     idle                  flat    quad    4:59:44
33       c0-0c0s8n1   default     idle                  flat    quad    4:59:44
34       c0-0c0s8n2   default     idle                  flat    quad    4:59:44

[...]
```
In this example, a four-node job with a maximum wall time of 4 hours and 59 minutes can be run during this backfill. The backfill times will not always be identical and will depend on the mix of jobs on the partitions that are being drained.

### Submitting to Specific Nodes
In rare cases, there may be a need to target specific hardware. This may be accomplished using "--attrs location=".

**For example:**

```
qsub -t 10 -n 16 --attrs location=0-7,100-107 myprogram.exe
```
This will force the job to run on those specific nodes. Should that location become unschedulable, for instance, due to a failed node, the job will not be allowed to run anywhere else, without resetting the location attribute. If more nodes are specified in the location field than are required to fill a job’s requested node count, then the first n nodes available in the location set will be used.

### Running with a Group of Users
Sometimes it is useful to allow other users to run Cobalt commands on a given job such as qhold, qrls, or qdel. A list of users can be allowed to run commands on your job by submitting a list of users to qsub, cqsub, or qalter using the flag --run_users. Specified users need not be in the same project under which the job was submitted.

**For example:**
```
qsub -A FellowShipOTR -n 512 -t 1:00 --run_users frodo:sam:pippin ./council (KNL)
```
As a convenience, all users belonging to the project under which a job was submitted can be added to a list of users that may control a job by using the --run_project flag.

Users who have been added to the list can run any command that the job-submitter could run on a job. This includes qhold, qrls, qalter, and qdel.

### Group Running and File System Groups
While setting this list of users allows any of the listed users to run Cobalt commands on a job, it does not do anything about the permissions of any files involved with the job. Those must be handled by the user(s) setting appropriate permissions on their directories to allow users in their group to read and write files as appropriate. If your project needs a group on the file system to share files or a user needs to be added, email [User Support](mailto:support@alcf.anl.gov).

#### More Information
For more information on Cobalt commands, their options, consult the manpages on the system. The same information may be found online in the [Cobalt Command Reference](http://trac.mcs.anl.gov/projects/cobalt/wiki/CommandReference).

### Using the Job Resource Manager: Commands, Options, and Examples
This document provides examples of how to submit jobs on our systems. It also provides examples of commands that can be used to query the status of jobs, what partitions are available, etc. 

#### Submit a Job Request
Use qsub to submit a job. (Unlike jobs on the ALCF BlueGene systems, all jobs on Theta are either script or interactive.)

Run the script jobscript.sh with 10 nodes for a maximum of 15 minutes:
```
qsub -n 10 -t 15 jobscript.sh
```
To submit jobs to a particular queue, use qsub -q <queue_name>.

To run jobscript.sh with 10 nodes for a maximum of 30 minutes in the debug queue for flat memory mode and quad numa mode:
```
qsub -q debug-flat-quad -n 10 -t 30 jobscript.sh
```
#### Charge a Job to a Project
Use qsub -A <project_name> to charge a job to a particular project.

To run jobscript.sh with 10 nodes for a maximum of 15 minutes and charge the job to MyProject:
```
qsub -n 10 -t 15 -A MyProject jobscript.sh
```
To see which projects you are a member of:

```
projects
```

You can use the environment variable “COBALT_PROJ” to set your default project. qsub -A takes precedence over COBALT_PROJ.

#### Delete a Job from the Queue
To delete a job from the queue, use the qdel command. For example for job with ID of 34586
```
qdel 34586
```

Depending on the stage of a job’s lifetime, qdel may not complete immediately, especially if the delete is issued during startup on a job that is changing memory modes and rebooting a node. If the job does not ultimately terminate, contact support@alcf.anl.gov with the jobid so that an administrator can take appropriate cleanup actions and administratively terminate the job.

#### Query Partition Availability
To determine which partitions are currently available to the scheduler, use the nodelist command. This command provides a list of node ids, names, queue, and state as well as any backfill windows.

**For example:**
```
% nodelist Node_id  Name         Queues Status           MCDRAM NUMA Backfill
 ===============================================================================
[...] 
20       c0-0c0s5n0 default cleanup-pending flat quad 4:59:44 
21       c0-0c0s5n1 default cleanup-pending flat quad 4:59:44 
22       c0-0c0s5n2 default busy flat quad 4:59:44 
24       c0-0c0s6n0 default busy flat quad 4:59:44 
25       c0-0c0s6n1 default busy flat quad 4:59:44 
26       c0-0c0s6n2 default busy flat quad 4:59:44 
27       c0-0c0s6n3 default busy flat quad 4:59:44 
28       c0-0c0s7n0 default idle flat quad 4:59:44 
29       c0-0c0s7n1 default idle flat quad 4:59:44 
30       c0-0c0s7n2 default idle flat quad 4:59:44 
31       c0-0c0s7n3 default idle flat quad 4:59:44 
32       c0-0c0s8n0 default idle flat quad 4:59:44 
33       c0-0c0s8n1 default idle flat quad 4:59:44 
34       c0-0c0s8n2 default idle flat quad 4:59:44 
[...]
```
