# Machine Reservations on Polaris

To get a reservation, you must first demonstrate a need to run outside of the normal queueing policies. Reservations are available only to projects with a positive allocation. Lead time for approval is 5 business days. If approved, scheduling is contingent on machine availability.

**Disclaimer:** Approval for reservation requests are subject to their appropriateness and machine availability. Not all requests will be approved. It is particularly difficult to accommodate reservation requests during busy times of the year, e.g. Supercomputing, end of the ALCC and INCITE allocation cycles.

To request a reservation, e-mail [support@alcf.anl.gov](mailto:support@alcf.anl.gov) with the requested information below.

1. RESERVATION REQUEST FOR ALL SYSTEMS (including vis clusters) AT ALCF Machine name:
2. Project for reservation:
3. ALCF account username(s) (NOT the user's legal name) for reservation:
4. Length of reservation:
5. Earliest date you could start:
6. Deadline for the run(s),
7. Details on the Run: Can it run anytime, day or night?
8. Your local time zone (e.g., US/Central):
9. Total number of jobs to be run:
10. Total amount of data generated during reservation:
11. For each job, indicate: Node count (Note: not processor count)
    1. Run time whether this job depends on any other jobs to finish before it can start
    2. Briefly describe the goals for this run: (Example: We are doing a scaling run of code XXXX to determine YYYY)
    3. Please provide a detailed explanation of why this workload cannot be accomplished with the existing queues: (Requests omitting this response will be not be processed)
    4. After a reservation is granted, you will receive a reservation name by e-mail. Use the command `pbs_rstat` to verify the reservation attributes.

**For example:**

```
pbs_rstat
Resv ID      Queue     User     State               Start / Duration / End             
---------------------------------------------------------------------------
A123456.po   A123456   smith@   CO       Mon Aug 18 09:00 / 43200 / Tue Aug 19 11:00

qsub -q A123456 walltime=60:00 -l select=1024:system=polaris -l filesystems=eagle myprog.exe
```

Once the reservation is set up, jobs can be submitted to the reservation queue prior to the reservation start time.

For jobs using 33 percent or more of a system, place your job in the queue at least 12 hours prior to the start of the reservation or your reservation may be canceled. The machine will start to drain for your reservation, and it is important that your job is ready to run.

You can also move jobs from the regular queue to the reservation queue at any time using the “qmove” command. 
Keep in mind that a job won't start unless enough time is left in the reservation. 

***NOTE: There is NOT a 10-minute pad at the end of the reservation. 
When the reservation ends all jobs are terminated, deleted, and the reservation queue is deleted. 
If a routing queue is used for the reservation, then jobs may be preserved, but any running job(s) are still terminated.***

If you have finished running your jobs before your reservation has ended, please reach out to the support team to have to release it for other users.
At this time, there is no way for a user to release a reservation early.