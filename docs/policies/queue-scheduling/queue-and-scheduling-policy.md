# Queue and Scheduling Policy

## General Policy

We ask that all users follow good etiquette and be excellent to one another.

## Priority

As with all Argonne Leadership Computing Facility production systems, job priority in the queue is based on several criteria:

- Positive balance of your project
- Size (in nodes) of the job; larger jobs receive higher priority
- The type of project (e.g., INCITE, ALCC, or discretionary)
- Job duration - shorter duration jobs will accumulate priority more quickly, so it is best to specify the job run time as accurately as possible

## Reservations and Scheduling Policy

Some work will require use of Polaris that necessitates deviation from regular policy. On such occasions, normal reservation policy applies. Please send the regular form no fewer than five (5) business days in advance.

## Big Run Mondays

As part of our regular maintenance procedures on Mondays, we will promote to the highest priority any jobs in the queued state requesting 802 nodes or more. Promotion is subject to operational discretion.

We may also, at our discretion, take the opportunity to promote the priority of capability jobs (20% of the machine) if the system has been drained of jobs for any other reason.

## Monday Maintenance

On Mondays when the ALCF is on a regular business schedule, the system may be expected to undergo maintenance from 9:00 am until 5:00 pm US Central Time. The `showres` command may be used to view pending and active maintenance reservations.

## INCITE/ALCC Overburn Policy

If an INCITE or ALCC project has exhausted its allocation in the first 11 months of its allocation year, it is eligible for overburn running. At this point, capability jobs (20% of the machine) submitted by INCITE and ALCC projects will run in the default queue (instead of backfill) for the first 11 months of the allocation year until 125% of the project allocation has been consumed.

INCITE and ALCC projects needing additional overburn hours should email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with a short description of what they plan to do with the additional hours, highlighting specific goals or milestones and the time expected to accomplish them. This will be reviewed by the scheduling committee, allocations committee, and ALCF management. Requests should be submitted 15 days before the start of the next quarter of the allocation year for full consideration. Non-capability jobs (less than 20% of the machine) from projects that have exhausted their allocation will continue to run in backfill.

To be clear, this policy does not constitute a guarantee of extra time, and we reserve the right to prioritize the scheduling of jobs submitted by projects that have not yet used 100% of their allocations, so the earlier that an INCITE or ALCC project exhausts its allocation, the more likely it is to be able to take full advantage of this policy.