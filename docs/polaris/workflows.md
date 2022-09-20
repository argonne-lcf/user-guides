# Workflows

## Balsam
[Balsam](https://balsam.readthedocs.io/en/latest/) is a python-based workflow manager that helps users execute large numbers of jobs, track job outcomes, and manage postprocessing analysis.

Balsam requires Python 3.7+.  To install Balsam, first set up a [virtual python environment](data-science-workflows/python.md):

```shell
module load conda
conda activate base
python -m venv env
source env/bin/activate
pip install --upgrade pip
pip install --pre balsam
```

To use Balsam, users need an account on the Balsam server.  Users can get an account by contacting the [ALCF Help Desk](mailto:support@alcf.anl.gov).  Once a user has an account, they can login and make a new site.  A Balsam site is a project space for your workflow. You will be prompted to select what machine (Polaris) you are working on when creating a new site:

```shell
balsam login
balsam site init -n new-site new-site
cd new-site
balsam site start
```

The [Balsam docs](https://balsam.readthedocs.io/en/latest/) give information on how to set up a workflow in your new site.  One helpful feature is Balsam's command line tool.  To get information on how to use the command line tool, you can type `balsam --help` in your shell.

## Parsl on Polaris

> Parsl is a flexible and scalable parallel programming library for Python.
> 
> -- <cite>Parsl Documentation</cite>

For many applications, managing an ensemble of jobs into a workflow is a critical step that can easily become a performance bottleneck.  Many tools exist to address this, of which `parsl` is just one.  On this page, we'll highlight some of the key pieces of information about `parsl` that are relevant to Polaris.  `Parsl` is also [extensively documented](https://parsl.readthedocs.io/en/stable/), has a dedicated Slack Channel, and a large community of users and developers beyond ALCF.  We encourage you to engage with the `parsl` community for support with `parsl` specific questions, and for Polaris-specific questions or problems, please contact support@alcf.anl.gov.

### Getting Parsl on Polaris

Polaris is newer than ``parsl``, and some changes to the source code were required to correctly use ``parsl`` on Polaris.  For that reason, on Polaris, a minimum parsl version of ``1.3.0-dev`` or higher is required on Polaris.

You can install parsl building off of the ``conda`` modules.  You have some flexibility in how you want to extend the ``conda`` module to include parsl, but here is an example way to do it:

```python
# Load the Conda Module (needed everytime you use parsl)
module load conda
conda activate

# Create a virtual env that uses the conda env as the system packages.
# Only do the next line on initial set up:
python -m venv --system-site-packages /path/to/your/virtualenv

# Load the virtual env (every time):
source /path/to/your/virtualenv/bin/activate

# Install parsl (only once)
pip install parsl==1.3.0.dev0 

```

### Using Parsl on Polaris

Parsl has a variety of system configurations available already, though as a new system, Polaris has not been included as of Fall 2022.  As an example, we provide the configuration below:

```python
from parsl.config import Config

# PBSPro is the right provider for Polaris:
from parsl.providers import PBSProProvider
# The high throughput executor is for scaling to HPC systems:
from parsl.executors import HighThroughputExecutor
# You can use the MPI launcher, but may want the Gnu Parallel launcher, see below
from parsl.launchers import MpiExecLauncher, GnuParallelLauncher
# address_by_hostname is best on polaris:
from parsl.addresses import address_by_hostname
# For checkpointing:
from parsl.utils import get_all_checkpoints

# Adjust your user-specific options here:
user_opts = {
    "worker_init":      "module load conda; conda activate; module load cray-hdf5; source /path/to/your/virtualenv/bin/activate",
    "scheduler_options":"" ,
    "account":          "YOURACCOUNT",
    "queue":            "debug-scaling",
    "walltime":         "1:00:00",
    "run_dir":          "/lus/grand/projects/yourproject/yourrundir/"
    "nodes_per_block":  3, # think of a block as one job on polaris, so to run on the main queues, set this >= 10
    "cpus_per_node":    32, # Up to 64 with multithreading
    "strategy":         simple,
}

checkpoints = get_all_checkpoints(user_opts["run_dir"])
print("Found the following checkpoints: ", checkpoints)

config = Config(
        executors=[
            HighThroughputExecutor(
                label="htex",
                heartbeat_period=15,
                heartbeat_threshold=120,
                worker_debug=True,
                max_workers=user_opts["cpus_per_node"],
                cores_per_worker=1,
                address=address_by_hostname(),
                cpu_affinity="alternating",
                prefetch_capacity=0,
                provider=PBSProProvider(
                    launcher=MpiExecLauncher(),
                    # launcher=GnuParallelLauncher(),
                    account=user_opts["account"],
                    queue=user_opts["queue"],
                    select_options="ngpus=4",
                    # PBS directives (header lines): for array jobs pass '-J' option
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
        run_dir=user_opts["run_dir"],
        checkpoint_mode = 'task_exit',
        strategy=user_opts["strategy"],
        retries=2,
        app_cache=True,
)

```

### Special notes for Polaris

On Polaris, we are currently investigating a hang that occurs in some parallel applications.  The hang occurs under these circumstances, as far as we can tell at this time:
- The application is launched on multiple nodes using `mpi`.
- The application uses ``fork`` on each node to spawn processes
- (Likely, but unconfirmed) the application has some aspect of locking involved, perhaps not visible to the user.

Under these circumstances, we have observed that rank 0 will proceed as far as possible, but ranks 1 to N will hang.  For ``parsl``, which uses ``python.multiprocessing`` (which calls ``fork``), hangs can occur for some workloads on remote ranks.  In this case, you can install ``GNU Parallel`` instead and use that as a launcher in ``parsl`` - you can install it into the virtual env as parsl is involved above.  This will circumvent the hang issue if your ``parsl`` workloads require ``fork`` calls.
