# Globus Compute

[Globus Compute platform](https://www.globus.org/compute) allows users to execute workloads remotely by submiting functions to endpoints on ALCF systems.

There are two options for using Globus Compute on ALCF systems:

1. Facility supported Globus Comute Endpoints (currently supported on Polaris and Crux).  These endpoints offer a set of options suitable for many common workloads.

2. Users may run their own Globus Compute Endpoints on login nodes or edge service nodes on ALCF systems.  This approach allows users to run Globus Compute on systems that currently do not have facility supported endpoints.  It also allows users to create endpoints with features not currently supported by facility endpoints.

In addition to these docs, the [globus compute docs](https://globus-compute.readthedocs.io/en/stable/index.html) are a useful reference.

## Facility Supported Multiuser Endpoints (MEPs)

Facility supported, multiuser endpoints are currently offered on Polaris and Crux.

| System  | UUID |
| ------  | ---- |
| Polaris | [9a947ba5-f537-4681-acf3-cc66485aadec](https://app.globus.org/compute/endpoints/9a947ba5-f537-4681-acf3-cc66485aadec) |
| Crux | [fd8b54bb-9452-411d-8e3a-09408156a886](https://app.globus.org/compute/endpoints/fd8b54bb-9452-411d-8e3a-09408156a886) |

The globus pages for these endpoints will give up-to-date details on their configuration templates, schemas, and status.

To submit a simple function to these endpoints from a remote system install `globus_compute_sdk` v4+ at the remote site:
```
pip install globus_compute_sdk > 4.0
```
And then execute one of these example python scripts (paste your project name in the account setting before execution):

=== "Polaris"

    ```python
    from globus_compute_sdk import Executor
    from globus_compute_sdk.serialize import ComputeSerializer, AllCodeStrategies

    def hello():
        return "hello from polaris"

    serializer = ComputeSerializer(strategy_code=AllCodeStrategies())
    gce = Executor(endpoint_id="9a947ba5-f537-4681-acf3-cc66485aadec",
                   serializer=serializer,
                   user_endpoint_config={"account": "<your project name>", 
                                         "queue": "debug",})
    future = gce.submit(hello)
    print(future.result())
    ```

=== "Crux"

    ```python
    from globus_compute_sdk import Executor
    from globus_compute_sdk.serialize import ComputeSerializer, AllCodeStrategies

    def hello():
        return "hello from crux"

    serializer = ComputeSerializer(strategy_code=AllCodeStrategies())
    gce = Executor(endpoint_id="fd8b54bb-9452-411d-8e3a-09408156a886",
                   serializer=serializer,
                   user_endpoint_config={"account": "<your project name>", 
                                         "queue": "debug",})
    future = gce.submit(hello)
    print(future.result())
    ```

These scripts create a Globus Compute `Executor`.  The `Executor` requires the endpoint id and the user's configuration options contained in `user_endpoint_config`.  The user's configuration options will be passed to the MEP to configure and create the user's endpoint or UEP that will submit jobs and execute work under the user's account.

The first time this script is executed, a request to authenticate with the globus service will appear a the command line.  Copy the URL given at the command line and paste it into an internet browser.  The URL will take you to the Globus website where you will be asked to authenticate your credentials.  Select "Argonne LCF" from the organizations menu and you will be taken to an ALCF page where you will be asked for your ALCF username and MobilePass+ code.  Once you successfully provide your MobilePass+ code, you will be taken back to the Globus page where you will be given a token of letters and numbers to copy.  Copy this token and paste it in your original command line prompt.  Authentication should be complete and should be valid for 7 days (check this).

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

The default environment activated by the endpoint includes all necessary dependencies to execute simple python functions.  A custom environment can be set by the user the configuration option `worker_init`.  The default setting for `worker_init` is:
```
"export TMPDIR=/tmp; export PATH=$PATH:/opt/globus-compute-agent/venv-py313/bin/"
```
If you wish to replace the default `worker_init` for your own `worker_init`, you must either activate a python environment with `globus-compute-endpoint` installed or append the default `work_init` commands to your custom `worker_init` commands.

!!! warning
    If you replace `worker_init` with your own commands, the environment it creates must include `globus_compute_endpoint`.  The `globus-compute-endpoint` application is required by compute jobs submited by globus compute ednpoints.  If your `worker_init` doesn't activate an environment with `globus-compute-endpoint` installed or include a path to an installation of `globus-compute-endpoint` in `PATH`, your PBS compute jobs will fail.  Moroever, if this happens, the endpoint will continue to submit jobs in a failure loop and your client process that submitted the requests to the endpoint will continue to wait.    [To stop this, delete the globus compute `pid` file](#runaway-job-submission) and revise your `worker_init` before resubmitting functions.

The setting of `TMPDIR` is to fix a known issue with Parsl running single node jobs with the `MpiExecLauncher` on ALCF systems.  It should be included if you expect running jobs that match this use case.

## Single User Endpoints

Users may, with caution, create their own single-user compute endpoints on login nodes.  This is appropriate for machines that do not yet support MEPs, like Aurora, or for workloads that require options not accomodated by the MEP configuration options.

The [ALCF globus compute repository](https://github.com/argonne-lcf/alcf-globus-compute) gives example config templates and instructions on how to use them.

## Examples

### Hello affinity

Here is a simple example that will return information on the endpoint environment.  This can be a useful example when debugging environments or running for the first time.

=== "Polaris"

    ```python
    from globus_compute_sdk import Executor

    def hello_affinity():
        import sys
        import parsl
        import socket
        import os
        import globus_compute_endpoint
        return f""" hostname: {socket.gethostname()}\n \
                    CUDA_VISIBLE_DEVICES: {os.environ.get('CUDA_VISIBLE_DEVICES')}\n \
                    remote environment: {sys.executable}\n \
                    python version: {sys.version}\n \
                    parsl version: {parsl.__version__}\n \
                    GCE version: {globus_compute_endpoint.__version__}
                """

    # Paste your endpoint id here
    endpoint_id = '9a947ba5-f537-4681-acf3-cc66485aadec'

    # ... then create the executor, ...
    gce = Executor(endpoint_id=endpoint_id,
                   user_endpoint_config={"account": "<your account name>", 
                                        "queue": "debug",})
    future = gce.submit(hello_affinity)
    print(future.result())
    ```

### Register Function

Globus compute allows users to register functions with the service that can then be called with a function id.  Registered Globus functions can be used in Globus Flows.

Here is an example of how to register a Globus function with a Globus Client:
```python
from globus_compute_sdk import Client

def adder(a, b):
    return a+b

gcc = Client()
function_id = gcc.register_function(adder)
print(f"Registered adder; id {function_id}")
```

The `register_function` call will return a UUID which is the function id.  This id can be used to run the function on any system that has environments and software capable executing it.  To run this example on the Crux and Polaris MEPs, copy the function id and paste it into this script:
```python
TBD
```

### Wrap Compiled Executable

This is a simple example to show how to wrap a compiled executable with a python function for execution with a Globus Compute endpoint.

```
TBD
```


### Multinode example

Here is an example of running functions across many nodes with `MpiExecLauncher`.  This example will execute on function per node concurrently.  Note that it is important to include `place=scatter` in the `scheduler_options`.

```python
from globus_compute_sdk import Executor
from globus_compute_sdk.serialize import ComputeSerializer, AllCodeStrategies
from concurrent.futures import as_completed

def query_host():
    import socket
    import time
    time.sleep(5)
    return f"Hello from node {socket.gethostname()}"

endpoint_id = "fd8b54bb-9452-411d-8e3a-09408156a886" # Polaris
num_nodes = 2
user_endpoint_config = {"account": "datascience", 
                        "queue": "debug",
                        "launcher_type": "MpiExecLauncher",
                        "scheduler_options": "#PBS -l filesystems=home\n#PBS -l place=scatter",
                        "max_workers_per_node": 1,
                        "nodes_per_block": num_nodes,
                        }
serializer = ComputeSerializer(strategy_code=AllCodeStrategies())
with Executor(endpoint_id=endpoint_id,
                serializer=serializer,
                user_endpoint_config=user_endpoint_config) as gce:
    
    # Submit functions
    futures = []
    for _ in range(2*num_nodes):
        futures.append(gce.submit(query_host))
    
    # Collect results
    for f in as_completed(futures):
        print(f.result())
```

## Troubleshooting

### Runaway job submission

The most common pitfall users will encounter is that the endpoint gets into a loop of queuing and running jobs that immediately fail.

To stop this behavior, delete the pid file(s) referenced by the endpoint.  This applies both to use of MEPs and single user endpoints.  To do this, login to the target machine and executue this command:
```bash
rm ~/.globus_compute/*/daemon.pid
```

This will stop all PBS job submissions under your user account, by the multiuser or single user endpoints.

To diagnose why this happened, there are several things to check:

1. Look at the PBS job logs created by the endpoint.  Look in `~/.globus_compute/<endpoint_name>/submit_scripts`.  The `<endpoint_name>` will begin with `uep` if useing the MEPs.  It may have another name if using a single user endpoint.  This directory will contain the PBS submit scripts and the PBS job stdout and stderr files.

2. A common issue you may find in the PBS job stdout files (found in `~/.globus_compute/<endpoint_name>/submit_scripts`) is that `globus-compute-endpoint` cannot be found.  If that is the case, check the environment commands you have activated in `worker_init`.  Make sure `globus-compute-endpoint` is in the PATH.

### Serialization errors

When submiting or registering functions from a client Executor on a remote machine, globus compute will serialize the function code and send the serialized code through the globus service to the compute endpoint on the target machine.  On the target machine, the environment activated by `worker_init` will deserailze the function for execution.

If there is a major version difference in the python version on the client side and the endpoint side of this exchange, it is possible to get an error due to seraization.  When this happens a message will often be returned on the client side with a `ManagerLost` error and this message:
```
This appears to be an error with serialization. If it is, using a different
serialization strategy from globus_compute_sdk.serialize might resolve the issue. For
example, to use globus_compute_sdk.serialize.AllCodeStrategies:
```

To resolve this issue there are a few options:

1. Match the python versions of the client and endpoint environments.

2. Pass a `serializer` to the `Executor`.  The `AllCodeStrategies` serializer is recommended as a first choice:

    ```python
    from globus_compute_sdk import Executor
    from globus_compute_sdk.serialize import ComputeSerializer, AllCodeStrategies

    serializer = ComputeSerializer(strategy_code=AllCodeStrategies())
    gce = Executor(endpoint_id="...",
                    serializer=serializer,
                    user_endpoint_config=...)
    ```

3. If submitting a registered function, re-register the function with environment on the target machine where the endpoint is located.  This gaurentees the serialization and deserialization of the function will be done in the same environment.