# Parsl on Polaris

> Parsl is a flexible and scalable parallel programming library for Python.
> 
> -- <cite>Parsl Documentation</cite>

For many applications, managing an ensemble of jobs into a workflow is a critical step that can easily become a performance bottleneck. Many tools exist to address this, of which `parsl` is just one. On this page, we'll highlight some of the key pieces of information about `parsl` that are relevant to Polaris. `Parsl` is also [extensively documented](https://parsl.readthedocs.io/en/stable/), has a dedicated Slack channel, and a large community of users and developers beyond ALCF. We encourage you to engage with the `parsl` community for support with `parsl`-specific questions, and for Polaris-specific questions or problems, please contact support@alcf.anl.gov.

## Getting Parsl on Polaris

You can install parsl by building off of the ``conda`` modules. You have some flexibility in how you want to extend the ``conda`` module to include parsl, but here is an example way to do it:

```bash linenums="1"
# Load the Conda Module (needed every time you use parsl)
module use /soft/modulefiles
module load conda
conda activate

# Create a virtual env that uses the conda env as the system packages.
# Only do the next line on initial setup:
python -m venv --system-site-packages /path/to/your/virtualenv

# Load the virtual env (every time):
source /path/to/your/virtualenv/bin/activate

# Install parsl (only once)
pip install parsl

```

## Using Parsl on Polaris

Parsl has a variety of possible configuration settings. As an example, we provide the configuration below that will run one task per GPU:

```python linenums="1"
from parsl.config import Config

# PBSPro is the right provider for Polaris:
from parsl.providers import PBSProProvider
# The high throughput executor is for scaling to HPC systems:
from parsl.executors import HighThroughputExecutor
# Use the MPI launcher to create one worker per GPU
from parsl.launchers import MpiExecLauncher
# For checkpointing:
from parsl.utils import get_all_checkpoints

# Adjust your user-specific options here:
run_dir="/lus/eagle/projects/yourproject/yourrundir/"

user_opts = {
    "worker_init":      f"source /path/to/your/virtualenv/bin/activate; cd {run_dir}", # load the environment where parsl is installed
    "scheduler_options":"#PBS -l filesystems=home:eagle" , # specify any PBS options here, like filesystems
    "account":          "YOURPROJECT",
    "queue":            "debug-scaling",
    "walltime":         "1:00:00",
    "nodes_per_block":  3, # think of a block as one job on polaris, so to run on the main queues, set this >= 10
    "cpus_per_node":    32, # Up to 64 with multithreading
    "available_accelerators": 4, # Each Polaris node has 4 GPUs, setting this ensures one worker per GPU
}

checkpoints = get_all_checkpoints(run_dir)
print("Found the following checkpoints: ", checkpoints)

config = Config(
        executors=[
            HighThroughputExecutor(
                label="htex",
                heartbeat_period=15,
                heartbeat_threshold=120,
                worker_debug=True,
                available_accelerators=user_opts["available_accelerators"],
                max_workers_per_node=user_opts["available_accelerators"],
                # This give optimal binding of threads to GPUs on a Polaris node
                cpu_affinity="list:24-31,56-63:16-23,48-55:8-15,40-47:0-7,32-39",
                prefetch_capacity=0,
                provider=PBSProProvider(
                    launcher=MpiExecLauncher(bind_cmd="--cpu-bind", overrides="--depth=64 --ppn 1"),
                    account=user_opts["account"],
                    queue=user_opts["queue"],
                    select_options="ngpus=4",
                    # PBS directives (header lines)
                    scheduler_options=user_opts["scheduler_options"],
                    # Command to be run before starting a worker, such as:
                    worker_init=user_opts["worker_init"],
                    # number of compute nodes allocated for each block
                    nodes_per_block=user_opts["nodes_per_block"],
                    init_blocks=1,
                    min_blocks=0,
                    max_blocks=1, # Can increase more to have more parallel jobs
                    cpus_per_node=user_opts["cpus_per_node"],
                    walltime=user_opts["walltime"]
                ),
            ),
        ],
        checkpoint_files = checkpoints,
        run_dir=run_dir,
        checkpoint_mode = 'task_exit',
        retries=2,
        app_cache=True,
)
```

