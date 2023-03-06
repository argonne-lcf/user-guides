# Virtual Environments

## Poplar SDK Setup

The **Graphcore** system has a bash shell script to set up the required software environment **Poplar** and **PopArt**.

The following command is automatically executed when one logs into a node.

```bash
source /software/graphcore/poplar_sdk/3.1.0/enable
```

Check if **Poplar** is setup correctly:

```bash
popc --version
```

One should see:

```bash
POPLAR version 3.1.0 (e12d5f9f01)
clang version 15.0.0 (bab932b4fc4cdb58bb009370384b2c41579bd9d9)
```

## PopTorch

### PopTorch Environment Setup

Run these commands:

```bash
mkdir -p ~/venvs/graphcore
virtualenv ~/venvs/graphcore/poptorch31_env
source ~/venvs/graphcore/poptorch31_env/bin/activate
```

Use the following commands to install PopTorch.

```bash
POPLAR_SDK_ROOT=/software/graphcore/poplar_sdk/3.1.0
export POPLAR_SDK_ROOT=$POPLAR_SDK_ROOT
pip install $POPLAR_SDK_ROOT/poptorch-3.1.0+98660_0a383de63f_ubuntu_20_04-cp38-cp38-linux_x86_64.whl
```

### Miscellaneous Environment Variables

```bash
mkdir ~/tmp
export TF_POPLAR_FLAGS=--executable_cache_path=~/tmp
export POPTORCH_CACHE_DIR=~/tmp

export POPART_LOG_LEVEL=WARN
export POPLAR_LOG_LEVEL=WARN
export POPLIBS_LOG_LEVEL=WARN

export PYTHONPATH=/software/graphcore/poplar_sdk/3.1.0/poplar-ubuntu_20_04-3.1.0+6824-9c103dc348/python:$PYTHONPATH
```

## TensorFlow 2 Environment Setup

Create virtual environment.

```bash
virtualenv ~/venvs/graphcore/tensorflow2_31_env
source ~/venvs/graphcore/tensorflow2_31_env/bin/activate
```

Install **TensorFlow** and **Keras**.

```bash
POPLAR_SDK_ROOT=/software/graphcore/poplar_sdk/3.1.0
export POPLAR_SDK_ROOT=$POPLAR_SDK_ROOT
pip install $POPLAR_SDK_ROOT/tensorflow-2.6.3+gc3.1.0+246224+2b7af067dae+amd_znver1-cp38-cp38-linux_x86_64.whl
pip install $POPLAR_SDK_ROOT/keras-2.6.0+gc3.1.0+246230+88e2debf-py2.py3-none-any.whl
```

### Verify Installation

```bash
python -c "from tensorflow.python import ipu"
```

You should see:

```bash
2023-01-04 17:17:12.609331: I tensorflow/compiler/plugin/poplar/driver/poplar_platform.cc:43] Poplar version: 3.1.0 (e12d5f9f01) Poplar package: 9c103dc348
```

## Installing Packages

Install packages in the normal manner such as:

```bash
python3 -m pip install "some_package"
```

For more details see [Use pip for installing](https://packaging.python.org/en/latest/tutorials/installing-packages/#use-pip-for-installing).

To install a different version of a package that is already installed in one's environment, one can use:

```bash
pip install --ignore-installed  ... # or -I
```

**NOTE: Conda is not supported on the Graphcore system.**
