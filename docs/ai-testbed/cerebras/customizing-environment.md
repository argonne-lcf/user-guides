# Customizing Environments

## Using a virtual python environment to customize an environment

It is considered good practice to create and use a python virtual environment for python projects with dependencies not satisfied by the base environment.
This prevents dependency conflicts between projects.
Cerebras does not support conda, but python virtualenvs can be used instead.

```console
python3 -m venv ~/testp3env
source ~/testp3env/bin/activate
pip install --upgrade pip
# sample pip install
pip install wget
# to exit the virtual env:
deactivate
```

To use this virtual env, e.g. in a script started with csrun_wse or csrun_cpu, or in a singularity shell:

```console
source testp3env/bin/activate
```

## Customizing the Cerebras singularity container

See this section of the Cerebras documentation:
[Adding Custom Packages To cbcore Container](https://docs.cerebras.net/en/latest/software-guides/adding-custom-pkgs-to-cbcore-container.html)
