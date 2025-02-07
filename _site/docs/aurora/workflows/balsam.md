# Balsam on Aurora

[Balsam](https://github.com/argonne-lcf/balsam) is a toolkit for managing large computational campaigns on HPC systems. Balsam helps users execute large numbers of jobs with inter-job dependencies, track job outcomes, and manage post-processing analysis. The command line interface and Python API make it easy for users to adopt: after wrapping the command line for an application in a few lines of Python code, users can describe jobs with accompanying options. These jobs are stored persistently in the Balsam database. Balsam is especially well-suited for executing large ensembles of MPI tasks with a variety of sizes.

A user's Balsam service consists of a Balsam Site process that runs on a login node that orchestrates the execution of work, and a Balsam Site directory space where job and workflow results are stored. When the user submits a batch job to PBS through Balsam, the Site process pulls Balsam jobs from the database and executes them within the PBS batch job, achieving high throughput while incurring only a single wait-time in the queue.

The full [Balsam documentation](https://balsam.readthedocs.io/en/latest) covers all functionality for users, including additional examples, and describes the Balsam architecture for potential developers.

## Setup and installation

Balsam requires Python 3.7+. To install Balsam on Aurora, first set up a virtual Python environment:

```bash linenums="1"
module load frameworks
python -m venv _env
source _env/bin/activate
pip install --upgrade pip
pip install --pre balsam
```

Alternatively, Balsam can be installed in a conda environment also with pip.

The Balsam command line tool will now be in your path. To get information on how to use the command line tool, you can type `balsam --help` in your shell.

To use Balsam, users need an account on the Balsam server. Users can get an account by contacting the [ALCF Help Desk](mailto:support@alcf.anl.gov). Once a user has an account, they can log in and make a new site. A Balsam site is a project space for your workflow. You will be prompted to select what machine (Aurora) you are working on when creating a new site:

```bash linenums="1"
balsam login
balsam site init -n new-site new-site
cd new-site
balsam site start
```

## Aurora specific notes

In the Balsam configuration for Aurora, a Balsam `gpu` refers to an Aurora node GPU tile. Setting the Balsam job option `gpus_per_rank = 1` will place one rank per GPU tile. Setting `gpus_per_rank = 2` will place one rank per GPU.

## Simple MPI ensemble on Aurora with Balsam

Here is an example that runs an application `hello_affinity` from our getting started guide in `mpi-mode`, which will execute the application with `mpiexec`. We also show an example of executing an echo command that takes an argument and runs on a single GPU tile.

!!! warning

    Ensembles of tasks launched with `mpiexec` on multiple nodes are currently limited to 1000 total tasks run per batch job. This means when `mpiexec` calls return, the nodes they used can refill only a limited number of times, rather than an arbitrary number of times like on Polaris. This is due to a known issue with Slingshot and will be fixed in the future. Users running MPI application ensembles on Aurora with Balsam should take this into account when configuring their workflows.

```python linenums="1" title="balsam_job_ensemble.py"
from balsam.api import ApplicationDefinition, Job

# Define an application that runs an echo command and takes an input
class EchoHello(ApplicationDefinition):
    site = "test_aurora"
    command_template = "echo Hello, {{ say_hello_to }}! ZE_AFFINITY_MASK=$ZE_AFFINITY_MASK OMP_NUM_THREADS=$OMP_NUM_THREADS"
EchoHello.sync()

# Define an application that runs a compiled executable and is wrapped by the gpu affinity script
class HelloAffinity(ApplicationDefinition):
    site = "test_aurora"
    command_template = "$HOME/GettingStarted/HelperScripts/Aurora/set_affinity_gpu_aurora.sh $HOME/GettingStarted/Examples/Aurora/affinity_gpu/sycl/hello_affinity"
HelloAffinity.sync()

# Create a Balsam job that runs hello_affinity on one node with twelve ranks per node
affinity_job = Job(site_name="test_aurora",
                   app_id="HelloAffinity",
                   workdir=f"demo/hello_affinity",
                   tags={"workflow":"demo"},
                   num_nodes=1,
                   node_packing_count=1,
                   ranks_per_node=12,
                   gpus_per_rank=1)

# This call saves a single Balsam job to the database
affinity_job.save()

# Create many Balsam jobs that run the EchoHello app each on a single tile
echo_jobs = [Job( site_name="test_aurora", 
         app_id="EchoHello", 
         workdir=f"demo/echo_hello{n}", 
         parameters={"say_hello_to": f"world {n}!"},
         tags={"workflow":"demo"}, 
         node_packing_count=12, # this allows for 12 of these jobs to run concurrently on a node
         gpus_per_rank=1,
         ranks_per_node=1,)       
    for n in range(24)
]

# This call saves a list of jobs to the database
echo_jobs = Job.objects.bulk_create(echo_jobs)
```

After execution of this script, your site will have two registered apps and several Balsam jobs. Use the Balsam CLI tool to query them:

To check apps registered in a site:
```bash linenums="1"
balsam app ls
```

To check the status of jobs in the site:
```bash linenums="1"
balsam job ls
```

To submit a batch job to PBS to execute the Balsam jobs in your site, you can do so at the command line from within your site directory:
```bash linenums="1"
balsam queue submit -n 2 -t 10 -q debug-scaling -A <project_name> -j mpi
```
This will submit a 2-node job (`-n` option) to the `debug-scaling` queue in mpi mode (`-j` option). MPI-mode batch jobs like this one will execute applications with `mpiexec`.  The time limit for the batch job is set to 10 minutes (`-t` option).

You can also submit jobs with the Python API:
```python linenums="1" title="submit_mpi_mode.py"
from balsam.api import BatchJob, Site

# Get the site id
site = Site.objects.get("test_aurora")
site_id = site.id

# Submit batch job to PBS
BatchJob.objects.create(
   site_id=site_id,
   num_nodes=2,
   wall_time_min=10,
   job_mode="mpi", # This mode will execute the application with mpiexec
   queue="debug-scaling",
   project="Aurora_deployment", # put your <project_name> here
)
```

To check the status of batch jobs that Balsam is tracking:
```bash linenums="1"
balsam queue ls
```
and batch jobs it has completed:
```bash linenums="1"
balsam queue ls --history
```

The standard output (stdout) will be written to each job's workdir in the data directory to a file called `job.out` and can be accessed like this:
```bash linenums="1"
cat data/demo/*/job.out
```

Batch jobs created by Balsam will have a name beginning with `qlaunch` when queried with the `PBS` command `qstat`.

Balsam has additional features that will submit work to PBS elastically, a special app type for native Python code, and a `serial` job mode for executing tasks that are single core/GPU that do not require MPI launching. More information can be found in the [Balsam documentation](https://balsam.readthedocs.io/en/latest).

## Troubleshooting

If Balsam is failing to submit batch jobs to PBS, check the `settings.yml` file in the Balsam site directory and look for the section `allowed_queues`.  The queue you are submitting to must appear in this section of the settings.  If it does not, add it and restart the site process with:
```bash linenums="1"
balsam site stop
balsam site start
```

If the queue does appear, get more information about the batch jobs Balsam is submitting to PBS with:
```bash linenums="1"
balsam queue ls -v --history
```
