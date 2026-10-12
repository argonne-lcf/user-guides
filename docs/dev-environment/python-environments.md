---
tags:
  - Python
---

# Python Environments

This page covers best practices for creating [Python environments](#creating-environments) and for [running Python command-line tools](#running-and-installing-tools) without managing an environment for them. You can use whichever package manager you prefer.

Installing everything with `pip install --user` puts every project's packages into one shared location, so projects eventually collide: upgrading a library for one code breaks another, and a tool installed months ago can impact behavior in subtle, hard-to-trace ways. Packages in `~/.local` are also visible from Conda environments and from virtual environments created with `--system-site-packages`, so a `--user` install can leak into environments that you meant to keep separate.

A virtual environment solves this by giving each project its own lightweight, isolated Python environment. The packages you install live in one self-contained folder, separate from the system libraries and from your other work. That isolation enhances reproducibility (you can `pip freeze` library versions from one environment and rebuild it later). It also keeps experiments from interfering with each other, and makes mistakes cheaper to undo.

ALCF provides curated `conda` environments with useful packages installed out of the box on [Aurora](../aurora/data-science/python.md), [Polaris](../polaris/data-science/python.md), and [Sophia](../sophia/data-science/python.md). [Python on Crux](../crux/data-science/python.md) details how to manage your own environments on Crux.

## Creating Environments

/// tab | uv

Verify that `uv` is installed:

```bash
uv --version
```

If not, install `uv`:

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

Create and activate your environment, using a specific Python version:

```bash
uv venv --python 3.13 .venv
source .venv/bin/activate
```

Install packages with `uv pip install` instead of `pip install`:

```bash
uv pip install <package>
```

Optionally, if you would rather not create an environment at all, `uv run --with <package> python` starts Python with that package in a throwaway environment.
///

/// tab | venv

Verify that `venv` is installed:

```bash
python -m venv --help
```

Create and activate your environment:

```bash
python -m venv .venv
source .venv/bin/activate
```

The environment inherits the Python version of your `python` command:

```bash
python --version
```

Install packages into the activated environment with `pip install`:

```bash
pip install <package>
```
///

/// tab | conda

Verify that `conda` is installed:

```bash
conda --version
```

Check which conda installation you are using:

```bash
conda info | grep "base environment"
```

If you need to install conda, use [Miniforge](https://github.com/conda-forge/miniforge). ALCF systems also provide conda through module files.

//// warning | Package channels

ALCF's shared environments use [Miniforge](https://github.com/conda-forge/miniforge) and the `conda-forge` channel.
Use `conda-forge` for your own environments, and avoid re-adding the `defaults` channel, which is subject to commercial licensing terms.
////

Create and activate your environment, using a specific Python version:

```bash
conda create -n my-env python=3.13 -y
conda activate my-env
```

Install packages into the activated environment with `pip install`:

```bash
pip install <package>
```
///

All of these install into your own directories. None of them needs `sudo`, and none writes to the system Python. Prefer an activated environment over `pip install --user`, since it keeps each project's packages separate and avoids conflicts with the system and Spack Python installations. Activate the environment, or use `uv run`, in every new shell, including inside job scripts.

## Comparing Package Managers

| Manager | Install a CLI tool | Install a library | Notes |
| ------- | ------------------ | ----------------- | ----- |
| [`uv`](https://docs.astral.sh/uv/) | `uv tool install <pkg>` | `uv venv`, then `uv pip install <pkg>` | Fast. Creates [environments](https://docs.astral.sh/uv/pip/environments/) with pip interface. Can run [tools](https://docs.astral.sh/uv/guides/tools/) or [scripts](https://docs.astral.sh/uv/guides/scripts/#declaring-script-dependencies) without activating anything.  Not tied to a Python version. [Installs Python](https://docs.astral.sh/uv/guides/install-python/) itself. [Manages projects](https://docs.astral.sh/uv/guides/projects/) via `pyproject.toml`.  |
| [`pipx`](https://pipx.pypa.io/) | `pipx install <pkg>` | n/a | One isolated environment per tool, linked into `~/.local/bin`. |
| `pip` + `venv` | `pip install <pkg>` in an activated virtual environment | `python -m venv .venv`, then `pip install <pkg>` | Available everywhere. Requires activating the environment first. Tied to the base installation of a specific Python version. |
| `conda` | `pip install <pkg>` in an activated environment | `conda create -n <env> python`, then `pip install <pkg>` | Use Conda for the environment and `pip` for packages that are only on PyPI. |

## Running and Installing Tools

`uvx` (short for `uv tool run`) and `pipx run` download a package into a cached, disposable environment and run its command. Nothing is added to your `PATH`, and the tool doesn't impact any of your existing environments:

```bash
uvx <package> --help          # same as: uv tool run <package> --help

# Alternative:
pipx run <package> --help
```

These are convenient for one-off commands, and they default to running the latest release. If `uv` uses a stale cached version, `uvx <pkg>@latest` bypasses the cache.

If you wish to install the tool persistently, so that its command is always available, while keeping the benefits of automatic tool-scoped environment isolation, use:

```bash
uv tool install <pkg>

# Alternative:
pipx install <pkg>
```

Both link the command into `~/.local/bin`. If that directory is not on your `PATH`, run `uv tool update-shell` or `pipx ensurepath` and start a new shell.

## Upgrading

| Manager | Upgrade a tool | Upgrade a library |
| ------- | --------------- | ------------------ |
| `uv` | `uv tool upgrade <pkg>` | `uv pip install --upgrade <pkg>` |
| `pipx` | `pipx upgrade <pkg>` | n/a |
| `pip` / `conda` | `pip install --upgrade <pkg>` in the activated environment | `pip install --upgrade <pkg>` |
