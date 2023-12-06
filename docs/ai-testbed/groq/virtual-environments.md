# Virtual Environments

## Install conda 
If conda is not already installed:
```bash
rm Miniconda3-latest-Linux-x86_64.sh*
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
# answer y/yes to all prompts
# exit ssh session, then start a new ssh session
exit
```
## GroqFlow conda environment setup
### Create and activate a groqflow conda environment
Create a groqflow conda environment and activate it
```bash
export PYTHON_VERSION=3.10.12
conda create -n groqflow python=$PYTHON_VERSION
conda activate groqflow
```

### Install groqflow into the groqflow conda environment
Execute the following commands to install groqflow into the activated groqflow conda environment
```bash
# Alter this if you have cloned groqflow to some other location.
cd ~/groqflow
pip install --upgrade pip
pip install -e .
pushd . 
cd demo_helpers
pip install -e .
popd
pip install soundfile
```

To use groqfloq,
```bash
conda activate groqflow
```
Note: Always use a personal conda environment when installing packages on groq nodes; otherwise they can get installed into `~/.local` and can cause problems when your shared home directory is used on other systems. If you encounter mysterious package dependency/version issues, check your `~/.local/lib` and `~/.local/bin` for mistakenly installed packages.

Note: The conda enviroment should be reinstalled whenever new groqflow code is pulled from the groqflow github; with a groqflow conda environment activated, redo just the pip install steps.
