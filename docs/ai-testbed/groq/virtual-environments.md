# Virtual Environments

For general guidance on creating and using Python environments, see [Python Environments](../../dev-environment/python-environments.md).

## Install conda 
If conda is not already installed:
```bash
rm Miniforge3-Linux-x86_64.sh*
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh
# answer y/yes to all prompts
# exit ssh session, then start a new ssh session
exit
```
## GroqFlow conda environment setup
### Create and activate a groqflow conda environment
Create a groqflow conda environment and activate it
```bash
export PYTHON_VERSION=3.10.12
conda create -n groqflow python=$PYTHON_VERSION -y
conda activate groqflow
```

### Install groqflow into the groqflow conda environment
Execute the following commands to install groqflow into the activated groqflow conda environment

```bash
# Alter this if you have cloned groqflow to some other location.
cd ~/groqflow
if [ -d "groqflow.egg-info" ]; then rm -r groqflow.egg-info; fi
pip install --upgrade pip
pip list --format=freeze > frozen.txt
pip install -r frozen.txt -e .
pushd . 
cd demo_helpers
if [ -d "groqflow_demo_helpers.egg-info" ]; then rm -r groqflow_demo_helpers.egg-info; fi
cp ../frozen.txt .
pip install -r frozen.txt -e .
popd
pip install transformers==4.20.0 huggingface-hub==0.1.0 datasets==2.3.2
pip install scikit-learn==1.1.1 numpy==1.21.6 pandas==2.0.0 scipy==1.8.0
pip install pyarrow==18.0.0 onnxconverter-common==1.13.0 skl2onnx==1.17.0
pip install soundfile==0.12.0
pip install opencv-python==4.6.0.66
```

Note: if you encounter problems trying to update an existing groqflow conda environment, consider removing the existing environment with the following command, and recreating it. Make sure you deactivate the environment before removing it. 
    ```bash
      conda remove --name groqflow --all -y
    ```

### Use Groqflow
To use groqfloq,
```bash
conda activate groqflow
```
Note: Always use a personal conda environment when installing packages on groq nodes; otherwise they can get installed into `~/.local` and can cause problems when your shared home directory is used on other systems. If you encounter mysterious package dependency/version issues, check your `~/.local/lib` and `~/.local/bin` for mistakenly installed packages.

Note: The conda environment should be reinstalled whenever new groqflow code is pulled from the groqflow github. With a groqflow conda environment activated, redo just the pip install steps, including the removal of the egg-info directories.
