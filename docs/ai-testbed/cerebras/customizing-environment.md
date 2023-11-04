# Customizing Environments

## Using virtual Python environments

#### To make a PyTorch virtual environment for Cerebras

```console
#Make your home directory navigable
chmod a+xr ~/
mkdir ~/R_1.9.2
chmod a+x ~/R_1.9.2/
cd ~/R_1.9.2
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_pt
/software/cerebras/python3.8/bin/python3.8 -m venv venv_pt
source venv_pt/bin/activate
pip3 install /opt/cerebras/wheels/cerebras_pytorch-1.9.2+92b4fad15b-cp38-cp38-linux_x86_64.whl --find-links=/opt/cerebras/wheels
pip install numpy==1.23.4
pip install datasets transformers
```

#### To make a TensorFlow virtual environment for Cerebras

```console
chmod a+xr ~/
mkdir ~/R_1.9.2
chmod a+x ~/R_1.9.2/
cd ~/R_1.9.2
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_tf
/software/cerebras/python3.8/bin/python3.8 -m venv venv_tf
source venv_tf/bin/activate
#pip install tensorflow_datasets
#pip install spacy
pip3 install /opt/cerebras/wheels/cerebras_tensorflow-1.9.2+92b4fad15b-cp38-cp38-linux_x86_64.whl --find-links=/opt/cerebras/wheels/
pip install numpy==1.23.4
```

#### Activation and deactivation

To activate one of these virtual environments,

```console
source ~/R_1.9.2/venv_pt/bin/activate
```

or

```console
source ~/R_1.9.2/venv_tf/bin/activate
```

To deactivate a virtual environment,

```console
deactivate
```
