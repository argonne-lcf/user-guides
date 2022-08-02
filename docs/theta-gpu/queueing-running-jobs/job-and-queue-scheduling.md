# Job and Queue Scheduling on ThetaGPU
## Queues and Job Scheduling
[Information on Theta Queue](/docs/theta/queueing-and-running-jobs/job-and-queue-scheduling.md)

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

### ThetaGPU Node Queues
**Note:** Users will need an allocation on ThetaGPU to utilize the GPU nodes. Request for an allocation by filling out this form: Allocation request. ThetaGPU is listed under Theta on the form.

The GPU nodes are new and we expect the workload to be significantly different than it is on the KNL nodes.  This document describes the current state of affairs, but we will monitor usage and adjust the policies as necessary.

### Nodes vs Queue vs MIG mode
The GPU nodes are NVidia DGX A100 nodes and each node contains eight (8) A100 GPUs.  You may request either entire nodes, or a single GPU based on your job needs.  What you will get is determined by the queue you submit to (See Queues section below).  If it has node in the name, you will get nodes.  If it has GPU in the name, you will get a single GPU. Note that if you need more than a single GPU, you should submit to the full-node queue.

Additionally, the Nvidia A100 GPUs support a feature called “Multi-Instance GPU” (MIG) mode.  This allows a single GPU to be shared by up to 7 different processes.  We do not schedule at this level, but to use MIG capabilities, you may pass –attrs mig-mode=True in with your qsub and use the `nvidia-smi_mig` command (note the UNDERSCORE) just as you would calling `nvidia-smi mig ...` directly. Attempts to call `nvidia-smi mig ...`(no underscore) directly will result in an error message. The single-gpu host will, by default, not create an MIG instance and users will have direct access to the gpu. If you are not using MIG mode, your session will appear as if it were a normal full-node system with only one gpu. Note that as of 12/13/21, MIG mode is unavailable for full-node jobs.

### Queues
There are three primary queues:
  - full-node: This is the general production queue for jobs that require full nodes. The -n parameter in your qsub will match the resource type in this queue i.e. -n 2 in node queue will get you two full nodes.
  - bigmem -  2 of the nodes have 640 GB of memory compared to the other 22 nodes with 320 GB. Use this queue to access these 2 nodes by specifying ```-q bigmem``` in your script. A max of 2 nodes (-n 2) can be requested in this queue.
  - single-gpu: This is the general production queue for jobs that operate best on a single GPUs. The -n parameter in your qsub should always be 1 as you can only submit to a single gpu. If you need more than 1 gpu, use the full-node queue.

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

#### Debug queues (currently unavailable)
  - debug-node: submit to this queue if you need an entire node for your testing (for instance you are utilizing the NVLink)
  - debug-gpu: submit to this queue if you need GPUs.

Initially, we are relaxing our node restrictions to encourage early users.  Please be courteous to your fellow users and do not monopolize the machine.  We will tighten restrictions as required to manage the demonstrated workload. 

## Running Jobs On ThetaGPU
**Note:** Users will need an allocation on ThetaGPU to utilize the GPU nodes. Request for an allocation by filling out this form: Allocation request. ThetaGPU is listed under Theta on the form.

### Running on multiple GPU nodes
Until there is tighter integration of Cobalt and mpirun on GPU nodes, the user will have to identify the nodes Cobalt assigned to their job and pass them as options to mpirun along with some other mpirun options.  The following shows 2 different code snippets on how to get the hosts allocated to the job and pass them to mpirun.

- option 1 - simplest
- mpirun -hostfile $COBALT_NODEFILE -n 16 -npernode 8 mpi-example-code
- where $COBALT_NODEFILE is a file that the -hostfile option can use.
- option 2 - little more complicated
- HOSTS=$(cat $COBALT_NODEFILE | sed ':a;N;$!ba;s/\n/,/g')
- mpirun  --np 16 --host $HOSTS --oversubscribe ./mpi-example-code

To specifically see how the MPI ranks were assigned, one could add --display-map --display-allocation to the mpirun options.

### Controlling which Cobalt instance (GPU) for my commands
IF YOU ARE ONLY USING KNL NODES NOTHING CHANGES AND YOU CAN IGNORE THIS

Because of the difference in architectures and limitations in Cobalt V1, we are running two Cobalt instances, the existing one for the KNL nodes, which remains as is, and a second one for the GPU nodes.  You need to be able to control which instance you are interacting with and there are several ways to do so.
  - As was true in the past, if you do nothing, the commands will default to the architecture associated with the host you are on when you issue it
  - If you are on the Theta login nodes, commands will default to the KNL instance.
  - If you are on a GPU node, for instance the build nodes, then commands will default to the GPU instance.
  - You can set an environment variable to control which instance the default commands (qsub, qstat, etc) will interact with. The primary use case here will be users who only use GPU nodes, but are working from the Theta login nodes.  To do so, you may:
    - `module load cobalt/cobalt-knl` which would make cobalt commands interact with the original Cobalt instance and launch jobs on the KNL nodes
    - `module load cobalt/cobalt-gpu` which would make Cobalt commands interact with the new Cobalt instance and launch jobs on the GPU nodes
    - you can also set COBALT_CONFIG_FILES=<path to cobalt config>
      - knl config: /etc/cobalt.knl.conf
      - gpu config: /etc/cobalt.gpu.conf

You can use suffixed commands to explicitly control which instance you are interacting with. If you regularly use both types of nodes, this is the recommended path to avoid confusion and to prevent launching jobs on the wrong architecture.

All the commands you are used to are there, they take the same command line parameters, etc., they just have either -knl or a -gpu suffix on them. For instance:
  - qsub-knl <parameters> would submit a job to the KNL nodes
  - qstat-gpu would check the queue status for the GPU nodes.

Requesting DGX nodes or individual GPUs
The DGX nodes, which contain (8) A100 GPUs, are extremely powerful and it can be very difficult for a single job to efficiently use an entire node.  For this reason, you may request either full nodes (all 8 GPUS) or individual GPUs.  What you are assigned (a node or a GPU) is dependent on the queue you submit to:
  - If the queue name ends in -node, you will get full nodes (8 A100 GPUs) 
  - If the queue name ends in -gpu, you will get an individual GPU
  - The -n parameter on the qsub is the number of resources of the type in that queue. So, for example:
    - `qsub -n 2 -q full-node <rest of command line>` would get you two full DGX nodes, which would be a total of (16) A100 GPUs
    - `qsub -n 2 -q single-gpu <rest of command line>` would get you two A100 GPUs

For reservations, you can only have one queue, and the resources in the queue need to be consistent, so your entire reservation must be in nodes or GPUs. If you need both, you will need two reservations, one for each type of resource.

    - Node names are of the form thetagpu## where ## ranges from 01 to 24.  This is an entire node (8 GPUs)
    - GPU names are of the form thetagpu##-gpu# where the GPU numbers range from 0-7.
 
### Multi-Instance GPU (MIG) mode
The A100 GPUs have a capability known as [Multi-Instance GPU (MIG)](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html). This allows you a single A100 GPU to be reconfigured at a hardware level down to a maximum of 7 instances. The valid configuration are shown in a table on the MIG page referenced above. These instances appear as a GPU to the application. In order to use this feature, the GPU must be put into MIG mode and this requires a reset of the GPU.  At the current time, we are not supporting scheduling at the MIG level.  However, a user can request that their GPU be put in MIG mode and then they can reconfigure the GPU into a supported configuration from their job script. 

If you wish to have the resources you have requested put into MIG mode you can add either of these to your qsub command line:
```--attrs mig-mode=True```
  
### Details of a job submission
Details of the job submission are recorded in the <jobid>.cobaltlog. This file contains the qsub command and environment variables. The location of this file can be controlled with the ‘qsub --debuglog <path>’ that defaults to the same place as the .output and .error files.

### Jobs stuck in "starting" state
If you submit a job and qstat shows it in "starting" state for 5 minutes or more, most likely your memory/numa mode selection requires rebooting some or all of the nodes your job was assigned. This process takes about 15 minutes, during which your job appears to be in the "starting" phase. When no reboots are required, the "starting" phase only lasts a matter of seconds.

### "utime" and "stime"
At the bottom of a <jobid>.ouput file, there is usually a line like:
```
Application 3373484 resources: utime ~6s, stime ~6s, Rss ~5036, inblocks ~0, outblocks ~8
```
The "utime" and "stime" values are user CPU time and system CPU time from the aprun and getrusage commands. They are rounded aggregate numbers scaled by the number of resources used, and are approximate. The aprun man page has more information about them.
  
### #COBALT directives on the second line of job script
If #COBALT directives are used inside a job submission script, then they must appear at the topmost lines of the script. #COBALT directives following a blank line will be ignored. Attempting to qsub the following example script will lead to the error message below.
  
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

