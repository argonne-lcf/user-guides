# Machine Reservations

To get a reservation, you must first demonstrate a need to run outside of the normal queueing policies. Reservations are available only to projects with a positive allocation.

A **5-business-day** lead time is recommended to ensure timely approval. Scheduling is contingent on machine availability.

!!! info "Disclaimer"

    Approval for reservation requests is subject to their appropriateness and machine availability. Not all requests will be approved. It is particularly difficult to accommodate reservation requests during busy times of the year, e.g., Supercomputing, end of the ALCC and INCITE allocation cycles.

!!! info "Kindness Policy"
    
	We do monitor reservation utilization. The Scheduling Committee reserves the right to cancel reservations without notice if we decide a reservation is underutilized, not being properly utilized, or otherwise wasting resources. For instance, requesting a 12-hour reservation for interactive work, but then going to lunch leaving the reservation empty with no work.

    !!! tip "Early Completion Policy"
    
    	If you have finished running your jobs before your reservation has ended, please reach out to the support team ([support@alcf.anl.gov](mailto:support@alcf.anl.gov)) to release it for other users. At this time, there is no way for a user to release a reservation early.

### Relevant terms
/// define
Reservation

- A number of nodes on a system set aside from the general pool of resources for a limited time. Only certain users or projects can submit to the queues assigned to the reservation.

Score boost

- A job with a boosted score helps it move ahead in the queue, but it still allows the scheduler to more efficiently fill the machine.

///

----

### [-> Fill out the Form <-](https://forms.office.com/Pages/ResponsePage.aspx?id=haH8DPcl40mK53BNUybihXhEiVpWIVZNp8Ow6W1CJnVUNTZLQ1c2N1lSOVNFQkg4RkJTSTAyMUJCNi4u)

----

# Querying Reservations via Command Line

You can see reservations using the `pbs_rstat` command:

### Submitting to a reservation

Use the `pbs_rstat` command on the login node to view the list of all reservations.

```bash
> pbs_rstat
Resv ID      Queue     User     State               Start / Duration / End             
---------------------------------------------------------------------------
A123456.po   A123456   smith@   CO       Mon Aug 18 09:00 / 43200 / Tue Aug 19 11:00
```

For recurring reservations, the `reserve_start` and `reserve_end` are always the first instance. `reserve_index` and `reserve_count` tell you where you are in the recurrence.

# Using a Reservation

Once the reservation is set up, jobs can be submitted to the reservation queue prior to the reservation start time. In the example above, the queue name is shown in the `Queue` column.

```bash
qsub -q A123456 walltime=60:00 -l select=1024:system=polaris -l filesystems=eagle myprog.exe
```

For jobs using 33 percent or more of a system, place your job in the queue at least 12 hours prior to the start of the reservation or your reservation may be canceled. The machine will start to drain for your reservation, and it is important that your job is ready to run.

You can also move jobs from the regular queue to the reservation queue at any time using the `qmove` command. Keep in mind that a job won't start unless enough time is left in the reservation.

!!! danger

    There is absolutely no time padding at the end of the reservation. When the reservation ends, all jobs are terminated, deleted, and the reservation queue is deleted. If a routing queue is used for the reservation, then jobs may be preserved, but any running job(s) are still terminated.

