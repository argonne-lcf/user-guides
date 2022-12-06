# Machine Reservations on Theta

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
    2. Briefly describe the goals for this run: (Example: We are doing a scaling run of code XXXX to determine YYY)
    3. Please provide a detailed explanation of why this workload cannot be accomplished with the existing queues: (Requests omitting this response will be not be processed)
    4. After a reservation is granted, you will receive a reservation name by e-mail. Use the command “showres” to verify the reservation attributes.

**For example:**

```
showres
Reservation  Queue     User   Start                                 Duration  Passthrough  Partitions             
==========================================================================================================
smith        R.smith   smith  Mon Aug  18 09:00:00 2013 -0500 (CDT)  24:00     Allowed      MIR-00000-73FF1-16384  

qsub -q R.smith -t 60 -n 1024 myprog.exe
```

Once the reservation is set up, jobs can be submitted to the reservation queue prior to the reservation start time.

For jobs using 33 percent or more of a system, place your job in the queue at least 12 hours prior to the start of the reservation or your reservation may be canceled. The machine will start to drain for your reservation, and it is important that your job is ready to run.

You can also move jobs from the regular queue to the reservation queue at any time using the “qmove” command. Keep in mind that a job won't start unless enough time is left in the reservation. There is a 10-minute pad at the end. (That is, if you have 60 minutes left, do not try to run more than 50.)

If you have finished running your jobs before your reservation has ended, use the command “userres” to release it for other users. The argument to userres is the reservation name shown in the first column of the showres output (in the example above, smith, not R.smith).
```
userres reservation_name
```
