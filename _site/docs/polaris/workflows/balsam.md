# Balsam

[Balsam](https://balsam.readthedocs.io/en/latest/) is a Python-based workflow manager that helps users execute large numbers of jobs, potentially with inter-job dependencies, track job outcomes, and manage post-processing analysis. A Balsam Site runs on a node with access to the job scheduler, where it can submit and monitor jobs. Overall job state is aggregated on the Balsam Server, making job data from all Sites accessible from any individual site (or the user's laptop) via the command-line interface or the Python API. To get information on how to use the command-line tool, you can type `balsam --help` in your shell.

Full documentation for Balsam is available [online](https://balsam.readthedocs.io/en/latest/).

Balsam requires Python 3.7+. To install Balsam on Polaris, first set up a [virtual Python environment](../data-science/python.md):

```shell
module use /soft/modulefiles
module load conda
conda activate base
python -m venv env
source env/bin/activate
pip install --upgrade pip
pip install --pre balsam
```

To use Balsam, users need an account on the Balsam server. Users can get an account by contacting the [ALCF Help Desk](mailto:support@alcf.anl.gov). Once a user has an account, they can log in and create a new site. A Balsam site is a project space for your workflow. You will be prompted to select which machine (Polaris) you are working on when creating a new site:

```shell
balsam login
balsam site init -n new-site new-site
cd new-site
balsam site start
```

See the Balsam documentation for full details.