# Python Virtual Environments

This section provides options on how to create and work with python environments.

## Creating Environments

??? "uv"

    Verify that `uv` is installed:
    ```bash
    uv --version
    ```

    If not, install `uv`:
    ```bash
    curl -LsSf https://astral.sh/uv/install.sh | sh
    ```

    Create and activate your environment, using a specific python version:
    ```bash
    uv venv --python 3.13 .venv
    source .venv/bin/activate
    ```

    Install packages with `uv pip install` instead of `pip install`.

??? "venv"

    Verify that `venv` is installed:
    ```bash
    python -m venv --help
    ```

    If not, install `venv`:
    ```bash
    sudo apt update
    sudo apt install python3-venv
    ```

    Create and activate your environment:
    ```bash
    python -m venv venv
    source venv/bin/activate
    ```

    The environment will inherit the python version tied to your `python` CLI:
    ```bash
    python --version
    ```

??? "conda"

    Verify that `conda` is installed:
    ```bash
    conda --version
    ```

    Check which conda installation you are using:
    ```bash
    conda info | grep "base environment"
    ```

    If you need to install conda, use [Miniforge](https://github.com/conda-forge/miniforge).

    Create and activate your environment, using a specific python version:
    ```bash
    conda create -n my-env python=3.13 -y
    conda activate my-env
    ```


### Using Tools