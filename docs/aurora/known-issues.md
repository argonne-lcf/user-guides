# Known Issues

This is a collection of known issues that have been encountered during Aurora's early user phase. Documentation will be updated as issues are resolved. Users are encouraged to email [support@alcf.anl.gov](mailto:support@alcf.anl.gov) to report issues.

A known issues [page](https://apps.cels.anl.gov/confluence/display/inteldga/Known+Issues) can be found in the CELS Wiki space used for NDA content. Note that this page requires a JLSE Aurora early hw/sw resource account for access.

## Running Applications

1. `Cassini Event Queue overflow detected.` errors may occur for certain MPI communications and may happen for a variety of reasons - software and hardware, job placement, job routing, and the sate of the machine. Simply speaking, it means one of the network interfaces is getting messages too fast and cannot keep up to process them

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

2. `double free detected` output while running with the mpich/52.2/* modules

A core dump might indicate communicator cleanup e.g. after calling MPI_Comm_split_type. A workaround is to unset a few config-file related variables: 
```
unset MPIR_CVAR_CH4_COLL_SELECTION_TUNING_JSON_FILE
unset MPIR_CVAR_COLL_SELECTION_TUNING_JSON_FILE
unset MPIR_CVAR_CH4_POSIX_COLL_SELECTION_TUNING_JSON_FILE
```
Additional information is here: https://github.com/pmodels/mpich/pull/6730 

3. Slower-than expected GPU-Aware MPI:
You can try one of those 2 set of env:
- RDMA
```
            export MPIR_CVAR_CH4_OFI_ENABLE_HMEM=1
            export MPIR_CVAR_CH4_OFI_ENABLE_MR_HMEM=0
            export MPIR_CVAR_CH4_OFI_ENABLE_MULTI_NIC_STRIPING=0
            export MPIR_CVAR_CH4_OFI_MAX_NICS=8
            export MPIR_CVAR_CH4_OFI_GPU_RDMA_THRESHOLD=0
```

- Pipelining
```
            export MPIR_CVAR_CH4_OFI_ENABLE_GPU_PIPELINE=1
            export MPIR_CVAR_CH4_OFI_GPU_PIPELINE_THRESHOLD=0
            export MPIR_CVAR_CH4_OFI_GPU_PIPELINE_BUFFER_SZ=4194304
            export MPIR_CVAR_CH4_OFI_GPU_PIPELINE_NUM_BUFFERS_PER_CHUNK=256
            export MPIR_CVAR_CH4_OFI_GPU_PIPELINE_MAX_NUM_BUFFERS=256
            export MPIR_CVAR_CH4_OFI_GPU_PIPELINE_D2H_ENGINE_TYPE=0
```

4. Compiler error like
```
_libm_template.c:(.text+0x7): failed to convert GOTPCREL relocation against '__libm_acos_chosen_core_func_x'; relink with --no-relax
```
in SYCL
- Please try linking with `-flink-huge-device-code`

5. General MPI Error

Similar to Issue #1, it maybe be useful to use other libfabric environment settings.
In particular, the setting below may be useful to try. These are what what Cray MPI sets by default
[Cray MPI libfabric Settings](https://cpe.ext.hpe.com/docs/24.03/mpt/mpich/intro_mpi.html#libfabric-environment-variables-for-hpe-slingshot-nic-slingshot-11).
```
export FI_CXI_RDZV_THRESHOLD=16384
export FI_CXI_RDZV_EAGER_SIZE=2048
export FI_CXI_DEFAULT_CQ_SIZE=131072
export FI_CXI_DEFAULT_TX_SIZE=1024
export FI_CXI_OFLOW_BUF_SIZE=12582912
export FI_CXI_OFLOW_BUF_COUNT=3
export FI_CXI_RX_MATCH_MODE=hardware
export FI_CXI_REQ_BUF_MIN_POSTED=6
export FI_CXI_REQ_BUF_SIZE=12582912
export FI_MR_CACHE_MAX_SIZE=-1
export FI_MR_CACHE_MAX_COUNT=524288
export FI_CXI_REQ_BUF_MAX_CACHED=0
export FI_CXI_REQ_BUF_MIN_POSTED=6
```

5. SYCL Device Free Memory Query Error

Note that if you are querying the free memory on a device with the Intel SYCL extension "get_info<sycl::ext::intel::info::device::free_memory>();", you will need to set `export ZES_ENABLE_SYSMAN=1`. Otherwise you may see an error like:

```
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
x1921c1s4b0n0.hostmgmt2000.cm.americas.sgi.com 0: terminate called after throwing an instance of 'sycl::_V1::invalid_object_error'
  what():  The device does not have the ext_intel_free_memory aspect -33 (PI_ERROR_INVALID_DEVICE)
```

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

To increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. See this page on [Working Around Node Failures](https://docs.alcf.anl.gov/aurora/running-jobs-aurora/#working-around-node-failures) for more information.



## Other issues

* Interim Filesystem: The early access filesystem is not highly performant. Intermittent hangs or pauses should be expected - waiting for IO to complete is recommended and IO completions should pass without failure. Jobs requiring significant filesystem performance must be avoided at this time.
* Large number of Machine Check Events from the PVC, that causes nodes to panic and reboot.
* HBM mode is not automatically validated. Jobs requiring flat memory mode should test by looking  at `numactl -H` for 4 NUMA memory nodes instead of 16 on the nodes.
* Application failures at large node-count are being tracked in the CNDA Slack workspace. See this [canvas table](https://alcf-cnda.slack.com/canvas/C05HMK7DD4J?focus_section_id=temp:C:EYXdcf8f1d1b86d44428a9abab5b) for more information and to document your case. ESP and ECP project members with access to Aurora should have access to the CNDA slack workspace. Contact support@alcf.anl.gov if you have have access to Aurora and belong to an ESP or ECP project,  but are not in the CNDA Slack workspace.
* Application failures at single-node are tracked in the JLSE wiki/confluence [page](https://apps.cels.anl.gov/confluence/pages/viewpage.action?pageId=4784336)
