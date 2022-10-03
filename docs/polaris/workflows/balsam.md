# Balsam

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
