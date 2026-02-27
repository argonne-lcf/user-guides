# Common PBS Issues & Troubleshooting

This page contains common PBS errors and issues that users may find useful in determining why their job is not yet running or has failed. 

## Error: ```Job violates queue and/or server resource limits```

If you receive a ```qsub: Job violates queue and/or server resource limits``` error immediately after submitting your job, then your job does not fit within any of the available queue limits.
If you're submitting to the Aurora or Polaris ```prod``` queues, be aware that the ```prod``` queue is a routing queue that routes the job to the ```small```, ```medium```, or ```large``` queue
(or to the backfill queues, depending on your project's remaining node-hours).

### Job "Disappeared" After Submission

For batch job submissions, if the parameters within your submission script do not meet the parameters of any of the execution queues (```small```, ..., ```backfill-large```), 
you might not receive the ```Job violates queue and/or server resource limits``` error on the command line at all, nor will the job appear in the history ```qstat -xu <username>``` (current bug in PBS).
For example, if a user submits a script to the Aurora ```prod``` routing queue requesting 10 nodes for 24 hours, exceeding the "Time Max" of 6 hours of the small execution queue (which handles jobs with 10-24 nodes), then it may behave as if the job was never submitted.

See [Aurora Queues](../aurora/running-jobs-aurora.md#queues), [Polaris Queues](../polaris/running-jobs/index.md#queues), or [Sophia Queues](../sophia/queueing-and-running-jobs/running-jobs.md#queues) for details on the available queues and their parameters.

## Error: ``` would exceed queue generic's per-user limit```

If you receive a ```qsub: would exceed queue generic's per-user limit``` error when attempting to submit your job, 
then you've reached the limit for the max number of jobs you can have both running and queued for that particular queue. 
Please see [Aurora Queues](../aurora/running-jobs-aurora.md#queues), [Polaris Queues](../polaris/running-jobs/index.md#queues), or [Sophia Queues](../sophia/queueing-and-running-jobs/running-jobs.md#queues) 
for details on the available queues and their individual per-user or per-job running+queued maximums.

!!! warning "If you DO NOT have any running or queued jobs"

    If you are receiving this error message when attempting to submit to one of the debug queues and DO NOT have any 
    actively running or queued jobs in the queue you are attempting to submit to, please email ALCF Support at 
    support@alcf.anl.gov and include your ALCF user ID, what system you are on, and what queue you are attempting to 
    submit to.

## Error: ```No active allocation found for project```...

If you receive a ```No active allocation found for project <your_project_name> and resource <system_name>``` error when attempting to submit your job,
then your project allocation has expired. You can submit an allocation renewal [here](https://my.alcf.anl.gov/accounts/#/allocationRequests).

## Common PBS Comments and their Meanings

!!! tip "Viewing PBS job comments"

    You can view the PBS comment assocated with your job ID with the following command: 
    qstat -was1 <your_job_ID>

### Comment: ```job held, too many failed attempts to run```

If you see a ```job held, too many failed attempts to run``` error when viewing your job's comment via the ```qstat -was1 <your_job_ID>``` command, then your job is attempting to run, failing, then re-trying.
This is usually caused by the following:

  1. One of the nodes you've been assigned is experiencing an issue. 
     If you've successfully run the *exact* same script in the past, then this is the most likely issue. 
     In this case, kill the held job, and resubmit.
  2. You're attempting to direct the ```*.e``` error and ```*.o``` output files to a either a directory that does not exist, 
     or to a directory that you do not have write permissions to. 
     PBS cannot create directories for you.
     So if you are using the ```#PBS -e``` or ```#PBS -o``` flags, make sure that the path listed exists and is writable to you.
  3. There's an issue with your code and PBS is attempting to restart, but it keeps failing. 
     In this case, you'll want to begin debugging, as it is not a PBS issue.

### Comment: ```Not Running: Not enough free nodes available```

The ```Not Running: Not enough free nodes available``` comment is the most common comment to see when a job is still in the queued state. 
This means that the resources you requested for your job are not yet available (i.e, if you requested 400 nodes, then there are not 400 nodes free yet due to other running jobs).

!!! warning "PBS ncpu and ngpu Flag Values"

    On Polaris and Aurora, the PBS 'ncpus' and 'ngpus' values are PER-NODE, not total for the job. 
    If you request an ncpu value that's greater than the total number of CPUs per compute node (i.e., ncpus=256) 
    your job will never run, as the resource(s) you are requesting doesn't exist.

### Comment: ```Not Running: Job is requesting an exclusive node and node is in use```

The ```Not Running: Job is requesting an exclusive node and node is in use``` comment means you have requested a specific node, 
instead of allowing PBS to select the first available node(s) for your job, and the specific compute node you requested in still in use or unavailable. 

## "Job to be deleted at request of Scheduler"

If you are running in the ```preemptable``` queue on Polaris and receive an email informing you that your has been deleted by at the request of the scheduler, 
then your job has been preempted by a job in the ```demand``` queue.

Please be aware that jobs in the ```preemptable``` queue can be killed **at any time** if jobs are submitted to the ```demand``` queue. 
Jobs in the ```demand``` queue take priority over jobs in the ```preemptable``` queue. 
This means jobs in the ```preemptable``` queue may be preempted (killed without any warning) if there are jobs in the ```demand``` queue. 
Unfortunately, there's always an inherent risk of jobs being killed when using the ```preemptable``` queue.

## Comments in Submission Scripts

Since ```#``` is required prior to each PBS directive, comments should be added *after* ALL directives have been listed in your submission script. 
If you try to add comments within the directive list, you could experience submission issues due to PBS attempting to read your comment as an additional directive.
This includes adding comments on the same line as a directive (i.e., ```#PBS -q <queue_name> #comment```).

## Unable to ```ssh``` to Compute Nodes

You should be able to `ssh` freely (without a password) between your assigned compute nodes. If you are running into `ssh` issues, check for the following causes:

   1. Your `/home/<username>` directory permissions should be set to `700` (`chmod 700 /home/<username>`).
   2. Confirm the following files exist in your `.ssh` directory and the permissions are set to the following:
      1. `-rw-------  (600)  authorized_keys`
      2. `-rw-r--r--  (644)  config`
      3. `-rw-------  (600)  id_rsa`
      4. `-rw-r--r--  (644)  id_rsa.pub`
   3. If you do not have the files mentioned above, you will need to create them. You can generate an `id_rsa` file with the following command: `ssh-keygen -t rsa`
   4. Copy the contents of your `.ssh/id_rsa.pub` file to `.ssh/authorized_keys`.

