# Guide of Login to Polaris Load Environments, Run Jobs, and Install GPytorch

#### 1. Login and queue a job
Login to Polaris
```
ssh alcfusername@polaris.alcf.anl.gov
```


Start a sessiom, for example Interactive ssh Session on a Compute Node
```
qsub -I -l nodes=1:ppn=4 -l walltime=1:00:00 -q debug -l filesystems=eagle:home -A datascience
```
  where:
* -A for project name
* -q for single-gpu or full-node. If using full-node, it would be better to add `-M <youremail>` as well, so that you will receive an email when your job is starting.
* -n number of resources (n gpu or n node)
* -t number of minutes you want to have
* -I indicates an interactive session. One can also remove -I and specify a executable bash script for it to run directly on the compute node

If queueing for an interactive session, once it is running, we can use `qstat -u <yourusername>` **on a service node**  to see our job id and allocated node. 


#### 2. Once on a Compute Node, Load Modules

Load the Conda Environment (Module) with Pytorch, since our GPytorch has Pytorch dependency
```
module load conda
conda activate
```
* Notice that we can check the available modules with "module avail" and check the loaded modules with "module list"

Create a virtual environment with python and activate it 
```
python -m venv --system-site-packages path_to_myenv
source path_to_myenv/bin/activate
```
Now the bash prompt should show that we're in the environment we just created, and we're good to use pip install
```
pip install gpytorch==version
```

### After the First Time
After the first time, to run the files, simply activate the python_venv on a compute node with 
```
module load conda
source path_to_myenv/bin/activate
``` 


## Using Jupyter Notebook to Run GPytorch on Polaris
Here is the guide:

### Approach 1 - Use Jupyter Hub
1. Go to [Jupyter Hub of ALCF](https://jupyter.alcf.anl.gov/), click Login Polaris
2. Queue up on a debug node

**For the first time only, one needs to set up the environment and Kernel by follow these extra steps**

4. Once Jupyter Notebook is launched on a compute node, click "New" and open a **terminal**
5. Run 
``` 
module load conda
conda activate
source <path_to_previously_created_python_venv>/bin/activate
python -m ipykernel install --user --name python_venv
```
Note: depending on the system and environment, you might need to install the "ipykernel" package first. The python_venv that I just created has the ipykernel module.

Go back to your .ipynb file, change kernel to python_venv from the dropdown menu, and we'll be good to run GPytorch!

### Approach 2 - Use ssh tunnel 
To use ssh tunnel, we first need to be in an interactive session on a compute node. See Part 1, "Log in and queue a job" for more details on this.

After on a compute node, follow these steps:
1. On the compute node terminal, do
```
module load conda
conda activate
jupyter notebook
```
You should see a line like `http://localhost:XXXX/`, where XXXX is the port number that jupyter notebook is launched on the compute node, usually the default 8888. If it is not 8888, replace 8888 in the following with your port number.

2. Then, on a **new, local terminal**, do
```
export PORT_NUM=8889
ssh -L $PORT_NUM:localhost:8888 <yourusername@polaris.alcf.anl.gov>
ssh -L 8888:localhost:8888 your_compute_node
navigate to localhost:8889 in your browser
``` 

You should see a jupyter notebook
Notice that for the first time doing this, one might need to input some password or weird key. Just follow the direction on that page.

(Essentially, the above steps, using ssh, sets the local port 8889 to listen to the allocated compute node port 8888 which we initiated a jupyter notebook.)

**For the first time only, one needs to set up the environment and Kernel by follow these extra steps**

Click "New" and open a **terminal**, and run
``` 
module load conda
conda activate
source <path_to_previously_created_python_venv>/bin/activate
python -m ipykernel install --user --name python_venv
```
Note: depending on the system and environment, you might need to install the "ipykernel" package first. The python_venv that I just created has the ipykernel module.

Go back to your .ipynb file, change kernel to python_venv from the dropdown menu, and we'll be good to run GPytorch!
