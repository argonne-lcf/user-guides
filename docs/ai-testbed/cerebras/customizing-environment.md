# Customizing Environments

## Using virtual Python environments

#### To make a PyTorch virtual environment for Cerebras

```console
mkdir ~/R_1.8.0
cd ~/R_1.8.0
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_pt
/srv/software/cerebras/python3.7/bin/python3.7 -m venv venv_pt
source venv_pt/bin/activate
python -m pip -q --disable-pip-version-check install pip
pip install db-sqlite3
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_appliance-1.8.0+fd608ab89c-py3-none-any.whl
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_pytorch-1.8.0+fd608ab89c-py3-none-any.whl --find-links=/opt/cerebras/wheels/
```

#### To make a TensorFlow virtual environment for Cerebras

```console
mkdir ~/R_1.8.0
cd ~/R_1.8.0
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_tf
/srv/software/cerebras/python3.7/bin/python3.7 -m venv venv_tf
source venv_tf/bin/activate
python -m pip -q --disable-pip-version-check install pip
pip install db-sqlite3
pip install tensorflow_datasets
pip install spacy
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_appliance-1.8.0+fd608ab89c-py3-none-any.whl
pip3 install -q --disable-pip-version-check /opt/cerebras/wheels/cerebras_tensorflow-1.8.0+fd608ab89c-py3-none-any.whl --find-links=/opt/cerebras/wheels/
```

#### Activation and deactivation

To activate one of these virtual environments,

```console
source ~/R_1.8.0/venv_pt/bin/activate
```

or

```console
source ~/R_1.8.0/venv_tf/bin/activate
```

To deactivate a virtual environment,

```console
deactivate
```
