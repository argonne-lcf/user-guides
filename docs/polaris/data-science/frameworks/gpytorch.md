# GPyTorch on Polaris

## 1. Login and queue a job
Login to Polaris
```
ssh alcfusername@polaris.alcf.anl.gov
```
!!! note

    The instructions below should be **run directly from a compute node**.

    Explicitly, to request an interactive job (from `polaris-login`):
    ```bash
    qsub -A <project> -q debug-scaling -l select=2 -l walltime=01:00:00 -I
    ```

    Refer to [job scheduling and execution](../../../running-jobs/index.md) for additional information.

## 2. Load Modules

Load the Anaconda environment module, which contains a PyTorch installation, since GPyTorch has PyTorch as a dependency:
```
module use /soft/modulefiles
module load conda
conda activate
```
* Notice that we can check the available modules with "module avail" and check the loaded modules with "module list."

Create a virtual environment with Python and activate it:
```
python -m venv --system-site-packages path_to_myenv
source path_to_myenv/bin/activate
```
Now the bash prompt should show that we're in the environment we just created, and we're good to use pip install:
```
pip install gpytorch==version
```

### Loading environment in future sessions
After the first time, to run the files, simply activate the Python virtual environment on a compute node with:
```
module use /soft/modulefiles
module load conda
source path_to_myenv/bin/activate
``` 

## 3. Using Jupyter Notebook to Run GPyTorch on Polaris
Here is the guide:

### Approach 1 - Use ALCF JupyterHub
1. Go to [Jupyter Hub of ALCF](https://jupyter.alcf.anl.gov/), click Login Polaris.
2. Queue up on a debug node.

**For the first time only, one needs to set up the environment and kernel by following these extra steps:**

4. Once Jupyter Notebook is launched on a compute node, click "New" and open a **terminal**.
5. Run:
``` 
module use /soft/modulefiles
module load conda
conda activate
source <path_to_previously_created_python_venv>/bin/activate
python -m ipykernel install --user --name python_venv
```
Note: Depending on the system and environment, you might need to install the "ipykernel" package first. The `python_venv` that I just created has the `ipykernel` module.

Go back to your `.ipynb` file, change the kernel to `python_venv` from the dropdown menu, and we'll be good to run GPyTorch!

### Approach 2 - Use SSH Tunnel 
To use an SSH tunnel, we first need to be in an interactive session on a compute node. See Part 1, "Login and queue a job" for more details on this.

On a compute node, follow these steps:
1. On the compute node terminal, do:
```
module use /soft/modulefiles
module load conda
conda activate
jupyter notebook
```
You should see a line like `http://localhost:XXXX/`, where `XXXX` is the port number that Jupyter Notebook is launched on the compute node, usually the default 8888. If it is not 8888, replace 8888 in the following with your port number.

2. Then, on a **new, local terminal**, do:
```
export PORT_NUM=8889
ssh -L $PORT_NUM:localhost:8888 <yourusername@polaris.alcf.anl.gov>
ssh -L 8888:localhost:8888 your_compute_node
navigate to localhost:8889 in your browser
``` 

You should see a Jupyter Notebook. Notice that for the first time doing this, one might need to input some password or key. Just follow the directions on that page.

(Essentially, the above steps, using SSH, set the local port 8889 to listen to the allocated compute node port 8888 where we initiated a Jupyter Notebook.)

**For the first time only, one needs to set up the environment and kernel by following these extra steps:**

Click "New" and open a **terminal**, and run:
``` 
module use /soft/modulefiles
module load conda
conda activate
source <path_to_previously_created_python_venv>/bin/activate
python -m ipykernel install --user --name python_venv
```
Note: Depending on the system and environment, you might need to install the "ipykernel" package first. The `python_venv` that I just created has the `ipykernel` module.

Go back to your `.ipynb` file, change the kernel to `python_venv` from the dropdown menu, and we'll be good to run GPyTorch!