#Job and Queue Scheduling for Theta

##General Policy
We ask that all users follow good etiquette and be kind to one another.

##Priority
As with all Argonne Leadership Computing Facility production systems, job priority in the queue is based on several criteria:
- positive balance of your project
- size (in nodes) of the job, larger jobs receive higher priority
- the type of project (e.g. INCITE, ALCC, or discretionary)
- job duration - shorter duration jobs will accumulate priority more quickly, so it is best to specify the job run time as accurately as possible

##Reservations and Scheduling Policy
Some work will require use of Theta that requires deviation from regular policy. On such occasions, normal reservation policy applies. Please send the regular form no fewer than five (5) business days in advance.

##Big Run Mondays
As part of our regular maintenance procedures on Mondays, we will promote to the highest priority any jobs in the queued state requesting 802 nodes or more (.ie. capability jobs). Promotion is subject to operational discretion.

We may also, at our discretion, take the opportunity to promote the priority of capability jobs if the system has been drained of jobs for any other reason.

##Monday Maintenance
On Mondays where the ALCF is on a regular business schedule the system may be expected to undergo maintenance from 9:00 am until 5:00 pm US Central Time. The showres command may be used to view pending and active maintenance reservations.

##INCITE/ALCC Overburn Policy
If an INCITE or ALCC project has exhausted its allocation in the first 11 months of its allocation year, it is eligible for overburn running. At this point, capability jobs submitted by INCITE and ALCC projects will run in the default queue (instead of backfill) for the first 11 months of the allocation year until 125% of the project allocation has been consumed. Note that non-capability jobs will be routed to backfill queue.

INCITE and ALCC projects needing additional overburn hours should e-mail support@alcf.anl.gov with a short description of what they plan to do with the additional hours, highlighting specific goals or milestones and the time expected to accomplish them. This will be reviewed by the scheduling committee, allocations committee, and ALCF management. Requests should be submitted 15 days before the start of the next quarter of the allocation year for full consideration. Non-capability jobs from projects that have exhausted their allocation will continue to run in backfill. 

To be clear, this policy does not constitute a guarantee of extra time, and we reserve the right to prioritize the scheduling of jobs submitted by projects that have not yet used 100% of their allocations, so the earlier that an INCITE or ALCC project exhausts its allocation, the more likely it is to be able to take full advantage of this policy.

##Queues
Information on ThetaGPU Queues:
###Theta (KNL) Queues
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

* Effective 4/13/20, ALCF has reserved 260 nodes in the production queue for a research project. The maximum job size is 4,100 nodes.

###Running With Less Than 128 Nodes in Default Queue

It is important to remember that jobs are charged by the number of nodes allocated, not the number of nodes used. For instance, a job requesting 14 nodes, below the minimum of 128, will be allocated 128 nodes and charged for 128 even though only 14 nodes are used.

We strongly encourage you to bundle smaller jobs into ensembles, either using Cobalt ensemble scripting or using a workflow system such as Balsam.

##Theta (KNL) Mode Selection and Charging
Time spent booting or rebooting nodes to obtain requested modes will not be charged to projects and neither will it count against requested walltime, although it will be included in the total runtime.

Please allow up to thirty (30) minutes for rebooting of nodes when submitting jobs.

Failure to specify a mode will result in the selection of cache-quad, the equivalent of listing:

```--attrs mcdram=cache:numa=quad```

in your qsub or job script.

**Note:** You are charged for the number of nodes allocated, not the number you use. The minimum possible allocation is 128 nodes.