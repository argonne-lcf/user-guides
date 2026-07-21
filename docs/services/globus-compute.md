# Globus Compute

[Globus Compute platform](https://www.globus.org/compute) allows users to execute workloads remotely by submiting functions to endpoints on ALCF systems.

There are two options for using Globus Compute on ALCF systems:

1. Facility supported Globus Comute Endpoints (currently supported on Polaris and Crux).  These endpoints offer a set of options suitable for many common workloads.

2. Users may run their own Globus Compute Endpoints on login nodes or edge service nodes on ALCF systems.  This approach allows users to run Globus Compute on systems that currently do not have facility supported endpoints.  It also allows users to create endpoints with features not currently supported by facility endpoints.

## Facility Supported Multiuser Endpoints (MEPs)

Facility supported, multiuser endpoints are currently offered on Polaris and Crux.

| System  | UUID |
| ------  | ---- |
| Polaris | [9a947ba5-f537-4681-acf3-cc66485aadec](https://app.globus.org/compute/endpoints/9a947ba5-f537-4681-acf3-cc66485aadec) |
| Crux | [d01d0c83-e570-4977-9170-1b8f2316e7c6](https://app.globus.org/compute/endpoints/d01d0c83-e570-4977-9170-1b8f2316e7c6) |

The globus pages for these endpoints will give up-to-date details on their configuration templates, schemas, and status.

To submit a simple function to these endpoints from a remote system install `globus_compute_sdk`:
```
pip install globus_compute_sdk
```
And then execute one of these example python scripts:

=== "Polaris"

    ```
    from globus_compute_sdk import Executor

    def hello():
        return "hello from polaris"

    gce = Executor(endpoint_id="9a947ba5-f537-4681-acf3-cc66485aadec",
                   user_endpoint_config={"account": "<your account name>", 
                                         "queue": "debug",})
    future = gce.submit(hello)
    print(future.result())
    ```

=== "Crux"

    ```
    from globus_compute_sdk import Executor

    def hello():
        return "hello from crux"

    gce = Executor(endpoint_id="d01d0c83-e570-4977-9170-1b8f2316e7c6",
                   user_endpoint_config={"account": "<your account name>", 
                                         "queue": "debug",})
    future = gce.submit(hello)
    print(future.result())
    ```

These scripts create a Globus Compute `Executor`.  The `Executor` requires the endpoint id and the user's configuration options passed to `user_endpoint_config`.  The user's configuration options will be passed to the MEP to configure and create the user's endpoint or UEP that will submit jobs and execute work.

### Configuration Options

The following configuration options are available on the MEPs.  When users create globus compute executors, they can include any of the following options. Note that `queue` and `account` are required options and must always be specified:

=== "Polaris"

    | Key | Default | Description |
    |---|---|---|
    | `queue` | required option; no default | queue to submit PBS jobs to |
    | `account` | required option; no default | project account to charge PBS jobs to |
    | `walltime` | `"1:00:00"` | walltime limit for PBS jobs submitted by the endpoint in the form of a string `"HH:MM:SS"` |
    | `nodes_per_block` | 1 | number of nodes per PBS job |
    | `max_workers_per_node` | 100 | concurrent function executions per node |
    | `max_idletime` | 240 | seconds before an idle PBS job shuts down |
    | `init_blocks` | 0 | initial number of PBS jobs queued at the start of the workload |
    | `min_blocks` |  0 | minimum number of PBS jobs queued/running during the workload  |
    | `max_blocks` | 1 | maximum number of PBS jobs queued/running during the workload |
    | `launcher_type` | `"SimpleLauncher"` | Parsl launcher used to create workers; swap to `"MpiExecLauncher"` for multi-node PBS jobs |
    | `worker_init` | `"export TMPDIR=/tmp; export PATH=$PATH:/opt/globus-compute-agent/venv-py313/bin/"` | activation commands at start of PBS jobs |
    | `scheduler_options` | `"#PBS -l filesystems=home"` | PBS options, full override REPLACES default — re-include `filesystems=` |
    | `select_options` | `"system=polaris"` | PBS select line options |

=== "Crux"

    | Key | Default | Description |
    |---|---|---|
    | `queue` | required option; no default | queue to submit PBS jobs to |
    | `account` | required option; no default | project account to charge PBS jobs to |
    | `walltime` | `"1:00:00"` | walltime limit for PBS jobs submitted by the endpoint in the form of a string `"HH:MM:SS"` |
    | `nodes_per_block` | 1 | number of nodes per PBS job |
    | `max_workers_per_node` | 100 | concurrent function executions per node |
    | `max_idletime` | 240 | seconds before an idle PBS job shuts down |
    | `init_blocks` | 0 | initial number of PBS jobs queued at the start of the workload |
    | `min_blocks` |  0 | minimum number of PBS jobs queued/running during the workload  |
    | `max_blocks` | 1 | maximum number of PBS jobs queued/running during the workload |
    | `launcher_type` | `"SimpleLauncher"` | Parsl launcher used to create workers; swap to `"MpiExecLauncher"` for multi-node PBS jobs |
    | `worker_init` | `"export TMPDIR=/tmp; export PATH=$PATH:/opt/globus-compute-agent/venv-py313/bin/"` | activation commands at start of PBS jobs |
    | `scheduler_options` | `"#PBS -l filesystems=home"` | PBS options, full override REPLACES default — re-include `filesystems=` |
    | `select_options` | `"system=crux"` | PBS select line options |

### Setting your own environment with `worker_init`

The default environment activated by the endpoint includes all necessary dependencies to execute simple python functions.  The environment is set by the configuration option `worker_init`.  The default setting for `worker_init` is:
```
"export TMPDIR=/tmp; export PATH=$PATH:/opt/globus-compute-agent/venv-py313/bin/"
```
If you wish to replace the default `worker_init` for your own `worker_init`, you must either activate a python environment with `globus-compute-endpoint` installed or append the default `work_init` commands to your custom `worker_init` commands.

!!! warning
    If you replace `worker_init` with commands that don't activate an environment with `globus-compute-endpoint` installed or include a path to `globus-compute-endpoint` in `PATH`, your PBS compute jobs will fail because `globus-compute-endpoint` will not be found by the jobs.  However, the endpoint will continue to submit jobs in a failure loop and your client process that submitted the requests to the endpoint will continue to wait.  [To stop this, delete the globus compute `pid` file.](#runaway-job-submission)

The setting of `TMPDIR` is to fix a known issue with Parsl running single node jobs with the `MpiExecLauncher` on ALCF systems.  It should be included if you expect running jobs that match this use case.

## Single User Endpoints

Users may, with caution, create their own single-user compute endpoints on login nodes.  This is appropriate for machines that do not yet suppor MEPs like Aurora or for workloads that require options not accomodated in MEP config templates.

The [ALCF globus compute repository](https://github.com/argonne-lcf/alcf-globus-compute) gives example config templates and instructions on how to use them.

## Examples

### Hello world

### Register Function

### Multinode example

## Troubleshooting

### Runaway job submission

The most common pitfall users will encounter is that the endpoint gets into a loop of queuing and running jobs that immediately fail.

To stop this behavior, delete the pid file(s) referenced by the endpoint.  This applies both to use of MEPs and single user endpoints.  To do this, login to the target machine and executue this command:
```
rm ~/.globus_compute/*/daemon.pid
```

This will stop all PBS job submissions under your user account, by the multiuser or single user endpoints.

To diagnose why this happened, there are several things to check:

### Serialization errors
