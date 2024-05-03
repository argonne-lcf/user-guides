# Customizing Environments

## Using virtual Python environments

#### To make a PyTorch virtual environment for Cerebras

```console
mkdir ~/R_2.2.0
cd ~/R_2.2.0
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_cerebras_pt
/software/cerebras/python3.8/bin/python3.8 -m venv venv_cerebras_pt
source venv_cerebras_pt/bin/activate
pip install --upgrade pip
pip install cerebras_pytorch==2.2.0
# Patch to fix a mount problem; a future release will not need this.
cp /software/cerebras/venvs/R_2_2.0_patch/appliance_manager.py ~/R_2.2.0/venv_cerebras_pt/lib/python3.8/site-packages/cerebras/appliance/appliance_manager.py
```

<!--- No longer any TensorFlow wheel
#### To make a TensorFlow virtual environment for Cerebras
--->

#### Activation and deactivation

To activate a virtual environments

```console
source ~/R_2.2.0/venv_cerebras_pt/bin/activate
```

To deactivate a virtual environment,

```console
deactivate
```
