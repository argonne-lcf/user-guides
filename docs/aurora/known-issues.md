# Known Issues

This is a collection of known issues that have been encountered during Aurora's early user phase. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.


## Running Applications

1. `Cassini Event Queue overflow detected.` errors may occur for certain MPI communications and may happen for a variety of reasons - software and hardware, job placement, job routing, and the sate of the machine. Simply speaking, it means one of the network interfaces is getting messages too fast and cannot keep up to process them

```
libfabric:16642:1701636928::cxi:core:cxip_cq_eq_progress():531<warn> x4204c1s3b0n0: Cassini Event Queue overflow detected.
```

As a workaround, the following environment variables can be set to try alleviating the problem.

```bash
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OVFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
```

The value of `FI_CXI_DEFAULT_CQ_SIZE` can be set to something larger if issues persist. This is directly impacted by the number of unexpected messages sent and so may need to be increased as the scale of the job increases. 

## Submitting Jobs

Jobs may fail to successfully start at times (particularly at higher node counts). If no error message is apparent, then one thing to check is the `comment` field in the full job information for the job using the command `qstat -xf [JOBID]`.

1. In the event that you find your job placed on hold, you may find the message `comment = job held, too many failed attempts to run`. This does not indicate a problem with your script, but indicates PBS made several attempts to find a set of nodes to run your job and was not able too. Users are encouraged to delete the held job and try resubmitting.

2. In the event of a node going down during a job, users may encounter messages such as `ping failed on x4616c0s4b0n0: Application 047a3c9f-fb41-4595-a2ad-4a4d0ec1b6c1 not found`. The node will likely have started a reboot and won't be included in jobs again until checks pass.

