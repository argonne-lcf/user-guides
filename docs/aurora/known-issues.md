# Known Issues

This is a collection of known issues that have been encountered during Aurora's early user phase. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the JLSE Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hw/sw resource account for access.

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

To increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. See this page on [Working Around Node Failures](https://docs.alcf.anl.gov/aurora/running-jobs-aurora/#working-around-node-failures) for more information.
## Other issues

* Interim Filesystem: The early access filesystem is not highly performant. Intermittent hangs or pauses should be expected - waiting for IO to complete is recommended and IO completions should pass without failure. Jobs requiring significant filesystem performance must be avoided at this time.
* Large number of Machine Check Events from the PVC, that causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking  at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.
* Application failures at large node-count are being tracked in the CNDA Slack workspace. See this [canvas table](https://alcf-cnda.slack.com/canvas/C05HMK7DD4J?focus_section_id=temp:C:EYXdcf8f1d1b86d44428a9abab5b) for more information and to document your case. ESP and ECP project members with access to Aurora should have access to the CNDA slack workspace. Contact support@alcf.anl.gov if you have have access to Aurora and belong to an ESP or ECP project,  but are not in the CNDA Slack workspace.
* Application failures at single-node are tracked in the JLSE wiki/confluence [page](https://apps.cels.anl.gov/confluence/pages/viewpage.action?pageId=4784336)
