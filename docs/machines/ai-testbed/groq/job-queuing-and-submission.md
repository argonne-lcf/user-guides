# Job Queueing and Submission

Groq jobs in the AI Testbed's groqrack are managed by the PBS job scheduler.<br>
Overview: [PBS](https://en.wikipedia.org/wiki/Portable_Batch_System)<br>
For additional information, see 
[https://docs.alcf.anl.gov/running-jobs/job-and-queue-scheduling/](https://docs.alcf.anl.gov/running-jobs/job-and-queue-scheduling/)<br>
Man pages are available. These are the key commands:
```console
# qsub - to submit a batch job using a script
man qsub
# qstat - to display queue information
man qstat
# qdel - to delete (cancel) a job:
man qdel
# qhold - to hold a job
man qhold
```

