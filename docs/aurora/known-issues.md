# Known Issues

This is a collection of known issues that have been encountered during Aurora's early user phase. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the CELS Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hw/sw resource account for access.

## Runtime Errors

### 1. `Cassini Event Queue overflow detected.`
  
`Cassini Event Queue overflow detected.` errors may occur for certain MPI communications and may happen for a variety of reasons - software and hardware, job placement, job routing, and the sate of the machine. Simply speaking, it means one of the network interfaces is getting messages too fast and cannot keep up to process them

```
libfabric:16642:1701636928::cxi:core:cxip_cq_eq_progress():531<warn> x4204c1s3b0n0: Cassini Event Queue overflow detected.
```

As a workaround, the following environment variables can be set to try alleviating the problem.

```bash
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
```

The value of `FI_CXI_DEFAULT_CQ_SIZE` can be set to something larger if issues persist. This is directly impacted by the number of unexpected messages sent and so may need to be increased as the scale of the job increases. 

It maybe be useful to use other libfabric environment settings.
In particular, the setting below may be useful to try. These are what what Cray MPI sets by default
[Cray MPI libfabric Settings](https://cpe.ext.hpe.com/docs/24.03/mpt/mpich/intro_mpi.html#libfabric-environment-variables-for-hpe-slingshot-nic-slingshot-11).

### 2. `failed to convert GOTPCREL relocation `

If you see
```
_libm_template.c:(.text+0x7): failed to convert GOTPCREL relocation against '__libm_acos_chosen_core_func_x'; relink with --no-relax
```
try linking with `-flink-huge-device-code`

### 3. SYCL Device Free Memory Query Error

Note that if you are querying the free memory on a device with the Intel SYCL extension "get_info<sycl::ext::intel::info::device::free_memory>();", you will need to set `export ZES_ENABLE_SYSMAN=1`. Otherwise you may see an error like:

```
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: terminate called after throwing an instance of 'sycl::_V1::invalid_object_error'
  what():  The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
```

### 4. `No VNIs available in internal allocator.` 

If you see an error like `start failed on x4102c5s2b0n0: No VNIs available in internal allocator`, pass ` --no-vni` to `mpiexec`

### 5. `PMIX ERROR: PMIX_ERR_NOT_FOUND` and `PMIX ERROR: PMIX_ERROR `

When running on single node, you may observe this error message:
```
PMIX ERROR: PMIX_ERR_NOT_FOUND in file dstore_base.c at line 1567 
PMIX ERROR: PMIX_ERROR in file dstore_base.c at line 2334
```
These errors can be safely ignored. 


## Submitting Jobs

Jobs may fail to successfully start at times (particularly at higher node counts). If no error message is apparent, then one thing to check is the `comment` field in the full job information for the job using the command `qstat -xfw [JOBID] | grep comment`. Some example comments follow.

```
comment = Job held by [USER] on Tue Feb 6 05:20:00 2024 and terminated
```
The user has placed the job on hold; user can `qrls` the job when ready for it to be queued again.


```
comment = Not Running: Queue not started. and terminated
```

User has submitted to a queue that is not currently running; user should `qmove` the job to an appropriate queue.

```
comment = job held, too many failed attempts to run
```

The job tried and failed to start. In this scenario, the user should find that their job was placed on hold. This does not indicate a problem the users' job script, but indicates PBS made several attempts to find a set of nodes to run the job and was not able too. Users can `qdel` the job and resubmit or `qrls` the job to try running it again.

```
comment = Not Running: Node is in an ineligible state: down and terminated
```

There are an insufficient number of nodes are online and free for the job to start

In the event of a node going down during a job, users may encounter messages such as `ping failed on x4616c0s4b0n0: Application 047a3c9f-fb41-4595-a2ad-4a4d0ec1b6c1 not found`. The node will likely have started a reboot and won't be included in jobs again until checks pass.

Use of the qsub `-V` flag(note: upper-case) is discouraged, as it can lead to startup failures.  The following message(found via pbsnodes -l):
```
failed to acquire job resources; job startup aborted (jobid: [YOUR JOBID])
```
indicates such a failure.  It is recommended to instead use `-v`(note: lower-case) and explicitly export any environment variables that your job may require.

To increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. See this page on [Working Around Node Failures](https://docs.alcf.anl.gov/aurora/running-jobs-aurora/#working-around-node-failures) for more information.



## Other issues

* Interim Filesystem: The early access filesystem is not highly performant. Intermittent hangs or pauses should be expected - waiting for IO to complete is recommended and IO completions should pass without failure. Jobs requiring significant filesystem performance must be avoided at this time.
* Large number of Machine Check Events from the PVC, that causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking  at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.
* Application failures at single-node are tracked in the JLSE wiki/confluence [page](https://apps.cels.anl.gov/confluence/pages/viewpage.action?pageId=4784336)
