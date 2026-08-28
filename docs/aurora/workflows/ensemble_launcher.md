# Ensemble Launcher

[Ensemble Launcher](https://argonne-lcf.github.io/ensemble_launcher/) is a lightweight Python tool for launching and orchestrating ensembles of tasks across HPC compute nodes.

Instead of submitting one PBS job per task, you request a single allocation and let Ensemble Launcher pack many tasks onto the nodes you already hold. A hierarchical master/worker tree distributes the tasks, which lets it scale from a single node to the whole machine. Tasks in the same ensemble can be heterogeneous (different node counts, process counts, and GPU requirements), can be shell commands, MPI applications, or Python callables, and get their CPU/GPU affinity set for you.

There are two ways to run it:

* **Batch mode** -- describe all your tasks up front, call `run()`, and block until everything finishes. This is the common case for parameter sweeps.
* **Cluster mode** -- start the orchestrator as a long-lived service and submit tasks to it dynamically from a client. Use this when tasks are generated as results come in (e.g. an AI model steering a simulation campaign).

## Installation

```bash linenums="1"
module load frameworks
python -m venv /path/to/venv --system-site-packages
source /path/to/venv/bin/activate
pip install "ensemble_launcher[hpc]"
```

The `hpc` extra pulls in `mpi4py`, which is needed to launch MPI tasks and to spawn sub-masters across nodes. See the ALCF docs for more details on using [Python on Aurora](../data-science/python.md).

For future sessions, load the `frameworks` module and re-run the activate line.

## Aurora System Configuration

Ensemble Launcher needs to know how many CPUs and GPUs each node has so it can pin tasks. A ready-made Aurora configuration is provided:

```python linenums="1"
from ensemble_launcher.config import get_system_config

system_config = get_system_config("aurora")  # 102 CPUs, 12 GPUs (tile-as-device) per node
```

This reserves cores 0 and 52 for the operating system and exposes the 12 Aurora GPU tiles as separate GPUs. Task GPU affinity is applied through `ZE_AFFINITY_MASK`, which is already the default `gpu_selector`.

The list of compute nodes is read automatically from `$PBS_NODEFILE`, so you do not need to pass `Nodes` inside a PBS job.

## Batch Mode

Describe the ensemble as a JSON file. The `cmd_template` is expanded once per parameter set:

**sweep.json:**
```json linenums="1"
{
    "ensembles": {
        "forces_sweep": {
            "nnodes": 1,
            "ppn": 12,
            "ngpus_per_process": 1,
            "cmd_template": "./forces.x {particles}",
            "particles": "linspace(1000, 10000, 24)",
            "relation": "one-to-one"
        }
    }
}
```

This creates 24 tasks, each using one node with 12 MPI ranks and one GPU per rank. Use `"relation": "many-to-many"` for a Cartesian product over multiple parameters instead of element-wise pairing.

**run_sweep.py:**
```python linenums="1"
from ensemble_launcher import EnsembleLauncher, write_results_to_json
from ensemble_launcher.config import LauncherConfig, get_system_config

if __name__ == "__main__":
    el = EnsembleLauncher(
        ensemble_file="sweep.json",
        system_config=get_system_config("aurora"),
        launcher_config=LauncherConfig(
            task_executor_name="async_mpi",
            return_stdout=True,
        ),
    )
    results = el.run()
    write_results_to_json(results, "results.json")
```

**submit.sh:**
```bash linenums="1"
#!/bin/bash -l
#PBS -A <myproject>
#PBS -l select=2
#PBS -l walltime=00:30:00
#PBS -l filesystems=home:flare
#PBS -q debug

cd $PBS_O_WORKDIR
module load frameworks
source /path/to/venv/bin/activate

python run_sweep.py
```

```bash linenums="1"
qsub submit.sh
```

With two nodes, the 24 tasks above run 2 at a time until the ensemble completes.

Instead of a JSON file you can pass a dictionary of `Task` objects, where `executable` is either a shell command string or a Python callable:

```python linenums="1"
from ensemble_launcher.ensemble import Task

tasks = {
    "sim-0": Task(task_id="sim-0", nnodes=1, ppn=12, ngpus_per_process=1,
                  executable="./forces.x 1000"),
    "analysis-0": Task(task_id="analysis-0", nnodes=1, ppn=1,
                       executable=my_python_function, args=(0.1, 0.2)),
}

el = EnsembleLauncher(ensemble_file=tasks, system_config=get_system_config("aurora"))
results = el.run()
```

## Cluster Mode

In cluster mode the orchestrator runs as a separate process and writes its ZMQ address to `checkpoint_dir`. A `ClusterClient` reads that address, submits tasks, and gets back `concurrent.futures.Future` objects. Tasks can be submitted at any time until you call `stop()`.

**run_cluster.py:**
```python linenums="1"
import os

from ensemble_launcher import EnsembleLauncher
from ensemble_launcher.config import LauncherConfig, get_system_config
from ensemble_launcher.ensemble import Task
from ensemble_launcher.orchestrator import ClusterClient

CHECKPOINT_DIR = os.path.join(os.environ["PBS_O_WORKDIR"], "ckpt")

if __name__ == "__main__":
    # 1. Start the orchestrator (non-blocking)
    el = EnsembleLauncher(
        ensemble_file={},
        system_config=get_system_config("aurora"),
        launcher_config=LauncherConfig(
            cluster=True,
            checkpoint_dir=CHECKPOINT_DIR,
            return_stdout=True,
        ),
    )
    el.start()

    # 2. Submit tasks as they are generated
    with ClusterClient(checkpoint_dir=CHECKPOINT_DIR) as client:
        futures = {}
        for i in range(20):
            task = Task(
                task_id=f"sim-{i}",
                nnodes=1,
                ppn=12,
                ngpus_per_process=1,
                executable=f"./forces.x {1000 * (i + 1)}",
            )
            futures[task.task_id] = client.submit(task)

        for task_id, future in futures.items():
            print(task_id, future.result(timeout=600))

    # 3. Shut down gracefully
    el.stop()
```

Submit it with the same PBS script as above, replacing `python run_sweep.py` with `python run_cluster.py`.

`EnsembleLauncher` is also a context manager, so `stop()` can be handled for you:

```python linenums="1"
with EnsembleLauncher(...) as el:
    ...  # orchestrator is running
# stop() is called on exit
```

## Command Line Interface

The `el` command runs an ensemble without a Python driver script. Configurations that would be passed as `SystemConfig` and `LauncherConfig` objects are supplied as JSON files.

In batch mode it blocks until every task finishes and writes `results.json`:

```bash linenums="1"
el start sweep.json \
    --system-config-file system.json \
    --launcher-config-file launcher.json
```

For cluster mode, set `"cluster": true` in the launcher config:

**launcher_cluster.json:**
```json linenums="1"
{
    "task_executor_name": "async_mpi",
    "cluster": true,
    "checkpoint_dir": "/lus/flare/projects/<myproject>/ckpt"
}
```

```bash linenums="1"
el start empty_ensemble.json --launcher-config-file launcher_cluster.json &

# submit tasks from a separate Python client using ClusterClient
python submit_tasks.py

el stop
```

!!! warning "Background the `el start` command"
    `el start` does not return in cluster mode -- the orchestrator is spawned as a child process and the CLI waits on it. Put it in the background with `&` so the rest of your job script can run.

`el stop` sends `SIGTERM` to the orchestrator (its PID is stored in `.el_launcher.pid` in the working directory) and it exits gracefully.

## Debugging

Turn on logging and status reporting through `LauncherConfig`:

```python linenums="1"
launcher_config = LauncherConfig(
    master_logs=True,
    worker_logs=True,
    report_interval=5.0,
)
```

Logs are written to `logs/master-*.log` and `logs/worker-*.log`.

To watch GPU usage, SSH into one of your compute nodes in another window and run:

```bash linenums="1"
module load xpu-smi
watch -n 0.1 xpu-smi dump -d -1 -m 0 -n 1
```

More details:

* [Ensemble Launcher Documentation](https://argonne-lcf.github.io/ensemble_launcher/)
* [Ensemble Launcher GitHub page](https://github.com/argonne-lcf/ensemble_launcher)
* [Examples directory](https://github.com/argonne-lcf/ensemble_launcher/tree/main/examples)
