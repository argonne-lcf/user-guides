# Customizing Environments

## Using virtual Python environments

#### To make a PyTorch virtual environment for Cerebras

```console
mkdir ~/R_2.2.1
cd ~/R_2.2.1
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_cerebras_pt
/software/cerebras/python3.8/bin/python3.8 -m venv venv_cerebras_pt
source venv_cerebras_pt/bin/activate
pip install --upgrade pip
pip install cerebras_pytorch==2.2.1
```

<!--- No longer any TensorFlow wheel
#### To make a TensorFlow virtual environment for Cerebras
--->

#### Activation and deactivation

To activate a virtual environments

```console
source ~/R_2.2.1/venv_cerebras_pt/bin/activate
```

To deactivate a virtual environment,

```console
deactivate
```
