---
tags:
  - Known Issues
last_verified: 2026-11-13
---

# Known Issues

Please check back here often for updates (indicated by the "Last Updated" timestamp at the bottom of this page). As users encounter new issues and find solutions or workarounds, we will update these notes.

## Working Reliably on Aurora

### Outages and Downtime – Expectations

On average, users should expect weekly preventative maintenance (PM) on the system (downtime most often scheduled for Mondays), although PMs will be deferred where possible. ALCF continues to pursue a number of stability and performance improvements in cooperation with HPE and Intel in order to improve the user experience. Users should continue to be proactive about verifying the correctness of their workloads, watching for hangs in their jobs, and otherwise adopting work methods that are mindful of and resilient to instability.

Maintenance and outage notices go to the [aurora-notify@alcf.anl.gov](mailto:aurora-notify@alcf.anl.gov) mailing list. All users with access to Aurora are added to the list.

### Scheduling

Aurora's queue policy is set up to help projects make productive use of the machine. See [Running Jobs on Aurora](running-jobs-aurora.md) for queues and limits.

- Start testing at small scales and ensure correct results (and performance) before ramping up to production campaigns.
- Validate code and runtime behavior with jobs of ≤2048 nodes before generating science results.

### Storage

#### Flare (Lustre File System)

Flare is the primary file system for project data on Aurora. Very heavy I/O can cause lags and performance degradation; increase I/O volume and job size gradually, and report problems to [support@alcf.anl.gov](mailto:support@alcf.anl.gov). See [Flare](data-management/lustre/flare.md) for striping and other I/O guidance.

#### DAOS (Object Store)

DAOS is the high-performance file system on Aurora which is currently in a stability testing pre-production period. Although now very rare, data loss events are possible so important data should be backed up periodically to a more reliable file system such as [Flare](data-management/lustre/flare.md). There also are relatively infrequent periods of unavailability or extremely slow performance usually due to network events or loss of SSDs.

DAOS is currently in its production configuration with 800 out of 1024 servers in use with approximately 200 PB of storage available to users. Please email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) if you are hitting limits and need your pool allocation size to be increased.

See the [DAOS](data-management/daos/daos-overview.md) page for how to use DAOS on Aurora, including its [known issues and workarounds](data-management/daos/daos-overview.md#known-issues-and-workarounds).

#### Grand/Eagle

These Lustre file systems are mounted on Polaris but not on Aurora, and Flare is not mounted on Polaris. DTNs and [Globus](../data-management/data-transfer/using-globus.md) are the best means to transfer data between Polaris and Aurora.

### Scaling out of Flare (Lustre) and `/soft` (NFS)

Applications which dynamically load libraries out of shared file systems such as Flare or `/soft` may experience performance impacts when scaling to large numbers of nodes. These guidelines may mitigate some of these scaling impacts:

- Use software in the Aurora PE (`/opt/aurora`) whenever possible, as this avoids dependence on shared file systems entirely.
- Statically link application binaries, as this reduces the number of dynamically loaded files.
- If loading many small (≲100MB) shared libraries or Python modules, use [Copper](data-management/copper/copper.md).

### Checkpointing

Checkpointing is absolutely essential. The mean time between application interrupts caused by system instability may be as short as an hour for larger jobs. The frequency of checkpointing is something that needs to be decided for each individual application based on the scale of runs:

- If checkpointing has minimal overhead, consider checkpointing once every 15 minutes.
- If checkpointing has substantial overhead, then consider checkpointing every 30-60 minutes.
- For the highest throughput, consider creating job dependency chains where scripts 1) automatically restart from the latest available checkpoint file and 2) confirm that the prior run generated reasonable/correct results.

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

#### Hangs

There are multiple failure modes that can lead to jobs hanging. For known hardware or low-level software issues such as ping failures as discussed above, just restart the job.

To avoid a hung job running out all the requested wallclock time on all its nodes, we suggest devising ways to monitor job progress. For example, if your application regularly writes small output to a logfile, then you could launch a “watcher” script that looks for that expected output and collects a stack trace and kills the job if it's been too long since progress was made. Please engage your Catalyst POC if you are interested in evaluating this for your application.

#### GPU Segfaults (a.k.a. "Page Faults")

Memory errors on the GPUs are caught when illegal accesses exceed a page boundary, and are reported with an error message like `Unexpected page fault from GPU at <address>`.

The best tools for debugging these are `gdb-oneapi` and `DDT`, both of which allow debugging into GPU kernel threads and looking at GPU data structures. You may also dump and step through the PVC assembly code using the debuggers if helpful. It is possible that there remain bugs in the IGC compiler that produce invalid assembly code, though as always the most likely cause of segfaults is memory errors in application code. To use the debuggers effectively in GPU kernels, you should compile and link your application with `-g -O0`. Keep in mind that the IGC compilation of GPU kernels takes place during the link phase if you're using AoT compilation.

## Issues and Workarounds

This is a collection of known issues that have been encountered on Aurora. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the CELS Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hardware/software resource account for access.

### Runtime and Compiler Issues

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

Other libfabric environment settings may also help, such as the defaults that Cray MPI sets: see [Cray MPI libfabric Settings](https://cpe.ext.hpe.com/docs/24.03/mpt/mpich/intro_mpi.html#libfabric-environment-variables-for-hpe-slingshot-nic-slingshot-11).

#### 2. `failed to convert GOTPCREL relocation`

If you see

```output
_libm_template.c:(.text+0x7): failed to convert GOTPCREL relocation against '__libm_acos_chosen_core_func_x'; relink with --no-relax
```

try linking with `-flink-huge-device-code`.

#### 3. SYCL Device Free Memory Query Error

Note that if you are querying the free memory on a device with the Intel SYCL extension `get_info<sycl::ext::intel::info::device::free_memory>();`, you will need to set `export ZES_ENABLE_SYSMAN=1`. Otherwise, you may see an error like:

```output
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: terminate called after throwing an instance of 'sycl::_V1::invalid_object_error'
  what():  The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
```

Applications are recommended to improve the error handling by checking `aspect::ext_intel_free_memory` SYCL device property before making a memory query.

#### 4. `No VNIs available in internal allocator.`

If you see an error like `start failed on x4102c5s2b0n0: No VNIs available in internal allocator`, pass `--no-vni` to `mpiexec`.

#### 5. `PMIX ERROR: PMIX_ERR_NOT_FOUND` and `PMIX ERROR: PMIX_ERROR`

When running on a single node, you may observe this error message:

```output
PMIX ERROR: PMIX_ERR_NOT_FOUND in file dstore_base.c at line 1567
PMIX ERROR: PMIX_ERROR in file dstore_base.c at line 2334
```

These errors can be safely ignored.

#### 6. File too large for Clang to process

If you see a compile error similar to:

```console
error: file '/var/tmp/icpx-85f5cf3735/wrapper-a0fbdc.bc' is too large for Clang to process
1 error generated.
```

when you are compiling with `-g`, you can decrease the size of the files by compiling with `-fno-system-debug -g`. See [Intel documentation](https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2025-2/fsystem-debug.html) for more details. If `-fno-system-debug -g` does not help, adding additionally `--offload-compress` may help as well.

#### 7. Set `TMPDIR` to avoid `AF_UNIX path too long` error

Software that relies on the setting of `TMPDIR` to create socket files may encounter the Linux error `AF_UNIX path too long` when running in processes launched with `mpiexec` on a single node. This issue has arisen in software using the `python` `multiprocessing` library for this purpose, including some use cases of `pytorch` and `parsl`.

The solution for this error is to manually set `TMPDIR` before launching the application, e.g.

```bash
export TMPDIR=/tmp
```

Alternatively, it can be set with the `mpiexec` command, e.g.

```bash
mpiexec --env TMPDIR=/tmp -n 1 --ppn 1 ...
```

#### 8. Slow performance with ≥96 processes per node

If you are running with 96 processes per node (i.e., `-ppn 96`) or more, you may see slowdowns or hanging (see [pmodels/mpich#7593](https://github.com/pmodels/mpich/issues/7593) for an example). To be clear, this is not total number of processes, but processes per node. A workaround for this is to set `export MPIR_CVAR_CH4_PROGRESS_THROTTLE=1`.

### Submitting Jobs

Jobs may fail to successfully start at times (particularly at higher node counts). If no error message is apparent, then one thing to check is the `comment` field in the full job information for the job using the command `qstat -xfw <jobid> | grep comment`. Some example comments follow.

```output
comment = Job held by <username> on Tue Feb 6 05:20:00 2024 and terminated
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

There are not enough nodes online and free for the job to start.

In the event of a node going down during a job, users may encounter messages such as `ping failed on x4616c0s4b0n0: Application 047a3c9f-fb41-4595-a2ad-4a4d0ec1b6c1 not found`. The node will likely have started a reboot and won't be included in jobs again until checks pass.

Use of the `qsub -V` flag (note: upper-case) is discouraged, as it can lead to startup failures. The following message (found via `pbsnodes -l`):

```output
failed to acquire job resources; job startup aborted (jobid: <jobid>)
```

indicates such a failure. It is recommended to instead use `-v` (note: lower-case) and explicitly export any environment variables that your job may require.

To increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. See this page on [Working Around Node Failures](./running-jobs-aurora.md#working-around-node-failures) for more information.

### Other Issues

* A large number of Machine Check Events from the PVC, which causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.
* Application failures at the single-node level are tracked in the [JLSE Wiki/Confluence page](https://apps.cels.anl.gov/confluence/pages/viewpage.action?pageId=4784336)

### Known Issues on Other Pages

Some pages keep known issues for a specific tool or library in their own section:

- [The `frameworks` module](data-science/frameworks/index.md#known-issues)
- [DAOS](data-management/daos/daos-overview.md#known-issues-and-workarounds)
- [Intel Sanitizer](debugging/Intel_sanitizer.md#known-issues-and-workarounds)
- [Parsl](workflows/parsl.md#known-issues)
- [VASP](applications-and-libraries/applications/vasp.md#known-issues)
- [VTune](performance-tools/vtune.md#known-issues-and-workarounds)
- [JupyterHub](../services/jupyter-hub.md#known-issues)
- [Visual Studio Code with Remote SSH](../dev-environment/vscode.md#known-issues)

## Aurora Bug Tracking repository and table

The repository [argonne-lcf/AuroraBugTracking](https://github.com/argonne-lcf/AuroraBugTracking) is a public bug tracking system for known issues (and recently resolved bugs) that affect production science on ALCF Aurora. To report an issue, please reach out to [ALCF Support](../support/ticket.md).

For convenience, nightly (sortable) copies of the summary tables are included here. For the latest versions, see [`bugs.md`](https://github.com/argonne-lcf/AuroraBugTracking/blob/main/bugs.md).

### [Wider View of Tables](./bugs-table.md)

---8<--- "AuroraBugTracking/bugs.md:3"
