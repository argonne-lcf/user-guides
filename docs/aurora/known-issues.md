# Early User Notes and Known Issues

_Last Updated: 2025-05-05_

## Early User Notes

Please check back here often for updates (indicated by the "Last Updated" timestamp above). As early production users encounter new issues and find solutions or workarounds, we will update these notes. As Aurora matures and becomes more stable over the first year of production, this section should become obsolete.

### Outages and Downtime – Expectations

Users should expect weekly preventative maintenance (PM) on the system, although  PMs will be deferred where possible. The stability of the Aurora system has improved significantly in the recent past, but there are still a number of improvement efforts ongoing in cooperation with HPE and Intel, to improve the user experience. Users need to be proactive about verifying correctness, watching for hangs, and otherwise adopting work methods that are mindful of and resilient to instability.

### Scheduling

The current queue policy for Aurora is set up based on experiences to date to help maximize productive use of the machine by projects.

- The initial goal for teams is to start testing at small scales, ensure correct results (and performance), and ramp up to generating scientific results in production campaigns.
- Focus initially on making good use of the system with <=2048 nodes per job; the key is to validate code and runtime behavior, then start generating science results. Initially, the `prod` routing queue will only allow 2048 nodes max. 
- Project teams are required to email support to request running on more than 2048 nodes (with evidence that they are likely to succeed at larger scales). Once a project has permission for larger jobs, any >2048-node job must be submitted to a separate routing queue, `prod-large`. See [Aurora Queues](./running-jobs-aurora.md#Aurora-Queues) for more details. 

### Storage

#### Flare (Lustre File System)

This is the primary and most stable storage filesystem for now. It is still possible that heavy use may trigger significant lags and performance degradations, and possibly lead to compute nodes crashing. We will continue to monitor filesystem stability as production use ramps up. We encourage teams to start out easy on I/O (both amount and job size), if possible, and report issues.

#### DAOS (Object Store)

DAOS is a scratch file system. Please note that data may be removed or unavailable at any time.

The initial configuration of DAOS has a smaller number of nodes, resulting in smaller project allocations. We expect DAOS to grow over the year, and when that happens, changes will be announced/posted in user docs. Please email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) if you are hitting limits and need the allocation size to be increased.

The performance of DAOS has been impressive, but we continue to experience crashes with large jobs, including loss of data. Projects may use it, but should not consider it stable or safe for long-term storage.

#### Grand/Eagle

These won’t be mounted on Aurora initially, but they might be mounted around May 2025, depending on feasibility. Similarly, Flare will not initially be mounted on Polaris. DTNs and [Globus](../data-management/data-transfer/using-globus.md) are the best means to transfer data between Polaris and Aurora.

### Scaling out of Flare (Lustre) and `/soft` (NFS)

Applications which dynamically load libraries out of shared filesystems such as Flare or `/soft` may experience performance impacts when scaling to large numbers of nodes. These guidelines may mitigate some of these scaling impacts:

- Use software in the Aurora PE (`/opt/aurora`) whenever possible, as this avoids dependence on shared filesystems entirely.
- Statically link application binaries, as this reduces the number of dynamically loaded files.
- If loading many small (≲100MB) shared libraries or Python modules, use [Copper](data-management/copper/copper.md).

### Checkpointing

Checkpointing is absolutely essential. The mean time between application interrupts caused by system instability may be as short as an hour for larger jobs. The frequency of checkpointing is something that needs to be decided for each individual application based on the scale of runs:

- If checkpointing has minimal overhead, consider checkpointing once every 15 minutes.
- If checkpointing has substantial overhead, then consider checkpointing every 30-60 minutes.
- It may be the case that the highest throughput initially will be with creating job dependency chains where scripts are able to 1) automatically restart from the latest available checkpoint file and 2) confirm that the prior run generated reasonable/correct results.

### Troubleshooting Common Issues

As always, INCITE and ALCC projects should report all issues to the Catalyst point of contact.

#### Ping Failures

Network and compute node instabilities may lead to inaccessible compute nodes, which will cause MPI ranks on those nodes to become unreachable. If your job output shows an error message like these:

```output
ping failed on x4707c6s4b0n0: Couldn't forward RPC ping(24c93b8c-3434-4fb5-a8f0-53cff4cbbe42) to child x4707c7s6b0n0.hostmgmt2707.cm.aurora.alcf.anl.gov: Resource temporarily unavailable
```

```output
ping RPC timeout from x4212c7s0b0n0.hostmgmt2212.cm.aurora.alcf.anl.gov after 120s
```

```output
ping failed on x4304c1s6b0n0: No reply from x4307c2s6b0n0.hostmgmt2307.cm.aurora.alcf.anl.gov after 87s
```

then most likely your application will crash or hang. If you see these, the best action is to kill the job and re-run it (from the last checkpoint, if there has been one). Potential issues can be discovered when the non-responsive node gets brought back online. Users may query PBS for more info.
```console
$ pbsnodes x4307c2s6b0n0 |grep comment
     comment = StabilityDB 2025-02-24T04:38:14: hbm_controller_errors
```

Email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to open a ticket if your application experiences ping failures, especially if these failures are frequent and/or involve the same problematic nodes. ALCF Operations may take such nodes offline.

<!-- #### PBS failures -->

<!-- Need to fill this in with examples and advice to set retry_count to something sane -->

#### Hangs

There are multiple failure modes that can lead to jobs hanging. For known hardware or low-level software issues such as ping failures as discussed above, just restart the job.

To avoid a hung job running out all the requested wallclock time on all its nodes, we suggest devising ways to monitor job progress. For example, if your application regularly writes small output to a logfile, then you could launch a “watcher” script that looks for that expected output and collects a stack trace and kills the job if it's been too long since progress was made. Please engage your Catalyst POC if you are interested in evaluating this for your application.

#### GPU Segfaults (a.k.a. "Page Faults")

Memory errors on the GPUs are caught when illegal accesses exceed a page boundary. When you see an error message indicating `Unexpected page fault from GPU at <address>`

The best tools for debugging these are `gdb-oneapi` and `DDT`, both of which allow debugging into GPU kernel threads and looking at GPU data structures. You may also dump and step through the PVC assembly code using the debuggers if helpful. It is possible that there remain bugs in the IGC compiler that produces invalid assembly code, though as always the most likely cause of segfaults is memory errors in application code. To use the debuggers effectively in GPU kernels, you should compile and link your application with `-g -O0`. Keep in mind that the IGC compilation of GPU kernels takes place during the link phase if you're using AoT compilation.

## Known Issues

This is a collection of known issues that have been encountered during Aurora's early user phase. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the CELS Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hardware/softare resource account for access.

### Runtime Errors

#### 1. `Cassini Event Queue overflow detected`

`Cassini Event Queue overflow detected` errors may occur for certain MPI communications and may happen for a variety of reasons - software and hardware, job placement, job routing, and the state of the machine. Simply speaking, it means one of the network interfaces is getting messages too fast and cannot keep up with processing them.

```output
libfabric:16642:1701636928::cxi:core:cxip_cq_eq_progress():531<warn> x4204c1s3b0n0: Cassini Event Queue overflow detected.
```

As a workaround, the following environment variables can be set to try alleviating the problem.

```bash
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_OFLOW_BUF_SIZE=8388608
export FI_CXI_CQ_FILL_PERCENT=20
```

The value of `FI_CXI_DEFAULT_CQ_SIZE` can be set to something larger if issues persist. This is directly impacted by the number of unexpected messages sent and so may need to be increased as the scale of the job increases.

It may be useful to use other libfabric environment settings. In particular, the setting below may be useful to try. These are what Cray MPI sets by default [Cray MPI libfabric Settings](https://cpe.ext.hpe.com/docs/24.03/mpt/mpich/intro_mpi.html#libfabric-environment-variables-for-hpe-slingshot-nic-slingshot-11).

#### 2. `failed to convert GOTPCREL relocation`

If you see

```output
_libm_template.c:(.text+0x7): failed to convert GOTPCREL relocation against '__libm_acos_chosen_core_func_x'; relink with --no-relax
```

try linking with `-flink-huge-device-code`

#### 3. SYCL Device Free Memory Query Error

Note that if you are querying the free memory on a device with the Intel SYCL extension `get_info<sycl::ext::intel::info::device::free_memory>();`, you will need to set `export ZES_ENABLE_SYSMAN=1`. Otherwise, you may see an error like:

```output
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: terminate called after throwing an instance of 'sycl::_V1::invalid_object_error'
  what():  The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
```
Applications are recommended to improve the error handling by checking `aspect::ext_intel_free_memory` SYCL device property before making a memory query.

#### 4. `No VNIs available in internal allocator.`

If you see an error like `start failed on x4102c5s2b0n0: No VNIs available in internal allocator`, pass `--no-vni` to `mpiexec`

#### 5. `PMIX ERROR: PMIX_ERR_NOT_FOUND` and `PMIX ERROR: PMIX_ERROR`

When running on a single node, you may observe this error message:

```output
PMIX ERROR: PMIX_ERR_NOT_FOUND in file dstore_base.c at line 1567 
PMIX ERROR: PMIX_ERROR in file dstore_base.c at line 2334
```

These errors can be safely ignored.

#### 6. Incorrect results in receive buffer in GPU memory 

In the default MPICH module on Aurora, it is possible to get incorrect results in GPU buffers passed through MPI calls. More detail are: [Issue#7302](https://github.com/pmodels/mpich/issues/7302). This will be fixed in the next MPICH module upgrade. For now, be careful of using GPU buffers in MPI communications as you may get incorrect results. 

### Submitting Jobs

Jobs may fail to successfully start at times (particularly at higher node counts). If no error message is apparent, then one thing to check is the `comment` field in the full job information for the job using the command `qstat -xfw <JOBID> | grep comment`. Some example comments follow.

```output
comment = Job held by <USER> on Tue Feb 6 05:20:00 2024 and terminated
```

The user has placed the job on hold; the user can `qrls` the job when ready for it to be queued again.

```output
comment = Not Running: Queue not started. and terminated
```

The user has submitted to a queue that is not currently running; the user should `qmove` the job to an appropriate queue.

```output
comment = job held, too many failed attempts to run
```

The job tried and failed to start. In this scenario, the user should find that their job was placed on hold. This does not indicate a problem with the user's job script, but indicates PBS made several attempts to find a set of nodes to run the job and was not able to. Users can `qdel` the job and resubmit or `qrls` the job to try running it again.

```output
comment = Not Running: Node is in an ineligible state: down and terminated
```

There are an insufficient number of nodes online and free for the job to start.

In the event of a node going down during a job, users may encounter messages such as `ping failed on x4616c0s4b0n0: Application 047a3c9f-fb41-4595-a2ad-4a4d0ec1b6c1 not found`. The node will likely have started a reboot and won't be included in jobs again until checks pass.

Use of the `qsub -V` flag (note: upper-case) is discouraged, as it can lead to startup failures. The following message (found via `pbsnodes -l`):

```output
failed to acquire job resources; job startup aborted (jobid: <YOUR JOBID>)
```

indicates such a failure. It is recommended to instead use `-v` (note: lower-case) and explicitly export any environment variables that your job may require.

To increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. See this page on [Working Around Node Failures](./running-jobs-aurora.md#working-around-node-failures) for more information.

### Other Issues

* A large number of Machine Check Events from the PVC, which causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.
* Application failures at the single-node level are tracked in the [JLSE Wiki/Confluence page](https://apps.cels.anl.gov/confluence/pages/viewpage.action?pageId=4784336)


## Aurora Bug Tracking repository and table

The repository [argonne-lcf/AuroraBugTracking](https://github.com/argonne-lcf/AuroraBugTracking) is a public bug tracking system for known issues (and recently resolved bugs) that affect production science on ALCF Aurora. To report an issue, please reach out to [ALCF Support](../support/technical-support.md).

For convenience, nightly (sortable) copies of the summary tables are included here. For the latest versions, see [`bugs.md`](https://github.com/argonne-lcf/AuroraBugTracking/blob/main/bugs.md)

### [Wider View of Tables](./bugs-table.md)

---8<--- "AuroraBugTracking/bugs.md:3"
