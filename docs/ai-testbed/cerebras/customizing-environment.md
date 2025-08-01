# Customizing Environments

## Using virtual Python environments

#### To make a PyTorch virtual environment for Cerebras

Clone the Cerebras modelzoo, if it is not already cloned. Check out the R 2.4.0 release.

```console
mkdir ~/R_2.4.0
cd ~/R_2.4.0
git clone https://github.com/Cerebras/modelzoo.git
cd modelzoo
git tag
git checkout Release_2.4.0
```
Note: a `git pull` will not update the tags; if `modelzoo/setup.py` does not exist after tag checkout, please re-clone `modelzoo`.


Then build the virtual environment

```console
mkdir ~/R_2.4.0
cd ~/R_2.4.0
# Note: "deactivate" does not actually work in scripts.
deactivate
rm -r venv_cerebras_pt
/software/cerebras/python3.8/bin/python3.8 -m venv venv_cerebras_pt
source venv_cerebras_pt/bin/activate
pip install --upgrade pip
pip install -e modelzoo
```

<!--- No longer any TensorFlow wheel
#### To make a TensorFlow virtual environment for Cerebras
--->

#### Activation and deactivation

To activate a virtual environments

```console
source ~/R_2.4.0/venv_cerebras_pt/bin/activate
```

To deactivate a virtual environment,

```console
deactivate
```
