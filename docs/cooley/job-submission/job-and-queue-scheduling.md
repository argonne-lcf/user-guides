# Queueing and Job Submission on Cooley
## Overview
Like our other computing resources, Cooley uses the Cobalt job scheduler. 

Use the "qstat" command to see what jobs are in the queue, and the ```nodelist``` command to see which of the compute nodes (cc001-cc126) are free.  The ```showres``` command will display any special reservations in place.

Use the "qsub" command to submit jobs; what you submit should be a script which will be executed on the rank0 node allocated to you by the scheduler. This script will have access to an environment variable named ```COBALT_NODEFILE```, which is the name of a file suitable for use with mpirun's ```-f``` option.  One important thing to note is that the ```--proccount``` option to qsub has no effect on Cooley – the number of MPI processes run by your job is entirely dependent on the arguments you supply to mpirun in your script.

At a minimum, qsub must be supplied with the number of nodes desired (-n), the walltime of the job (-t), and the path to the job script.  If you are associated with more than one project, you will also need to supply the project name using the -A option (or, alternately, set the PROJECT environment variable to that project name).

**Note:** If you modify or replace your .bashrc, you will need to retain the following lines from the default .bashrc in order to ensure that your Cobalt batch jobs receive a complete software environment:

```
#  Source global definitions
if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi
```

Some important notes regarding the job script:
  - The job script runs only on one node of your job (designated as the head node); it is up to your script to distribute processes to the other nodes in a multi-node job.  For a normal MPI job, this is accomplished by including ```-f $COBALT_NODEFILE``` in the arguments to mpirun/mpiexec. The ```COBALT_NODEFILE``` environment variable expands to the location of the nodefile (on the head node), which is a file containing the list of nodes Cobalt has assigned to your job.
  - Your job script must have an interpreter at the top (for example, #!/bin/bash) in order for Cobalt to recognize it as a valid script.
  - The job script will run using your default environment as set up by .bash_profile, .bashrc, and .soft.cooley at the time the job runs.  It does not inherit the environment the qsub command was run in.  If you need environment changes specific to this job, they must be set explicitly within the job script.

For example, you might have a script named test.sh, which runs mpirun with 12 processes per node (one process per core):
```
#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))
mpirun -f $COBALT_NODEFILE -n $PROCS /home/lueningh/cpi/cpi-x86_64
```
**Note:** The specific mpirun used depends on your software environment; you'll need to use a softenv key to specify which MPICH version you want to use.  By default, we give all new users +mvapich2 to use the most recent version of MVAPICH2, but if desired, a different MPI may be selected from several available.  For more information on softenv, see the softenv-intro man page.

To request 5 nodes from cobalt with 10 minutes of walltime, charging to the MyProject project, you would use the command:
```
qsub -n 5 -t 10 -A MyProject ./test.sh
```
Cobalt will produce some files in the same directory where you ran qsub (unless of course you tell it to use a different working directory). By default, they are named <jobid>.error, <jobid>.output, and <jobid>.cobaltlog. The .error output is stderr from your script, .output is stdout from your script, and the .cobaltlog file is some cobalt specific information like what your qsub submission contained, and how cobalt tried to invoke your executable.

There is also an "interactive" mode. "qsub -I" will submit an interactive job to cobalt.  
  
Use it like this:
```
qsub -I -n 1 -t 30
```
  
It will block until your job runs, print out the list of nodes allocated to you, and then ssh to your rank0 node. When you log out (or your requested walltime expires), your job will be removed from the queue and the nodes will be released.  
  
**Note:** The old 'qsubi' method of running interactive jobs is deprecated and no longer available on Cooley -- please use 'qsub -I' instead.
 
## Specifying Filesystems
On systems running Cobalt at the ALCF your job submission should specify which filesystems your will be using.  In the event that a filesystem becomes unavailable, this information is used to preserve jobs that would use that filesystem while allowing other jobs that are not using an affected filesystem to proceed to run normally.  

You may specify your filesystem by adding ```filesystems=<list of filesystems>``` to the ```--attrs``` argument of qsub in Cobalt. Valid filesystems are home, eagle, grand, and theta-fs0. The list is comma-delimited. 

For example, to request the home and eagle filesystems for your job you would add filesystems=home,eagle to your qsub command. If this is not specified a warning will be printed and then **the job will be tagged as requesting all filesystems and may be held unnecessarily if a filesystem is not currently available**. The warnings are written to stderr of qsub and qalter commands that change the value of the --attrs flag.  Scripts that are parsing stderr from these utilities may encounter errors from the additional warnings if filesystems are not specified in these commands.

If a job is submitted while a filesystem it requested is marked down, the job will automatically be placed into a user_hold and a warning message will be printed, but the job will be otherwise queued. The job is also placed into admin_hold by a sysadmin script. Once the affected filesystem has been returned to normal operation, the admin_hold is released. You are responsible for releasing the user_hold once you receive the message that the affected filesystem has been returned to normal operation. The job cannot run until both the holds are released.

If a job requesting a filesystem that is marked down is already in the queue, it will be placed on admin_hold and will be released once the filesystem is operational.
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

## Job Scheduling on Cooley
There are two primary queues on Cooley, default and debug

### Default Queue
The default queue is for production use, and is the default queue for jobs that are submitted without a queue specified. It has the following characteristics:
  
  - Max. runtime: 12 hours
  - Max. job size: 110 nodes (the other sixteen nodes are dedicated to debugging)
  - Max. running jobs per user: 10
  - Max. running and queued jobs per user: 30
  - Max. node-hours (queued and running): 1320
  - Priority: FIFO -- (jobs are run in order, with small, short jobs run on any otherwise-free nodes)

### Debug Queue
In addition, there are sixteen nodes set aside for dedicated debugging in the **debug** queue. This is intended for short debugging and interactive visualization runs only. It has the following scheduling policy:

  - Max. runtime: 2 hour
  - Max. job size: 16 nodes
  - Max. running jobs per user: 1
  - Priority: FIFO -- (jobs are run in order, with small, short jobs run on any otherwise-free nodes)

#### Public Network Connectivity
For jobs that require public network connectivity on the compute nodes (i.e. connectivity to non-ALCF resources), you may include the argument --```attrs=pubnet``` in your qsub command . 

#### GPUs Directly for Computation
For jobs that use the GPUs directly for computation (e.g. CUDA) and don't require an X sever, you may wish to include the argument ```--attrs=nox11``` in your qsub command.  This will stop the X server that normally runs on the nodes in order to prevent any performance impact on your GPU jobs.

If required, the above job attributes may be combined as a colon-separated list, e.g. ```--attrs=pubnet:nox11```

While we currently continue to maintain special-purpose queues for the above functions (the queues named **pubnet**, **pubnet-debug**, **nox11**, and **pubnet-nox11**) in order to maintain compatibility for submission scripts that use them, these queues have been deprecated in favor of using the above job attributes, which provide more flexbility and can be used within reservations.

If you have needs not addressed by the standard queues, please send mail to [support@alcf.anl.gov](https://mailto:support@alcf.anl.gov) requesting a reservation.

We will monitor Cooley's queues and evaluate the above policies as needed. Your [feedback](mailto:support@alcf.anl.gov) is appreciated.
  
  



