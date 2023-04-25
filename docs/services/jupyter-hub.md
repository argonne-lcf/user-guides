# Jupyter Hub
## Using Jupyter Hub

JupyterHub is an open-source application to allow multiple users to launch Jupyter Notebooks from a central location. At ALCF users can use the JupyterHub instances at [https://jupyter.alcf.anl.gov](https://jupyter.alcf.anl.gov) to run notebooks on servers connect to the compute resource or on the compute resource itself.

The JupyterHub instance assigned to Cooley start notebooks on a login node.  The JupyterHub instance assigned to theta run notebooks a server external to Theta.  This instance does not have access to HPE/Cray programing environment and tools located in ``/opt/cray, opt/intel, etc.``.  These instances provide users access to their home (/home/$USER) and project folders on theta-lustre, grand and eagle file systems (/home/$USER, /project, /grand, /eagle) using symbolic links in the home directory.

The JupyterHub instances assigned to ThetaGPU allows users to start notebooks on a compute node through the job schedule system (Cobalt).  The instance assigned to Polaris will start notebooks on compute nodes through the job scheduler (PBS Pro).  These instances follow the scheduling and accounting policies run the notebook against the project allocation.


## Customize Environment

ALCF provides a simple Python environment to start.  User can customize their environment to meet their needs by creating virtual python environment and defining new kernels.  Below is an example of setting up a simple environment `projectA` with module mpi from within a notebook

For more information on how to manage conda environments, refer to this [page](https://conda.io/docs/user-guide/tasks/manage-environments.html).

From a terminal:
```
# Load a conda module
module load conda
conda activate base

# set shell proxy variables to access external URL
export http_proxy=http://proxy.alcf.anl.gov:3128
export https_proxy=$http_proxy

# create an environment name projectA
conda create -y -n projectA

# Activate conda environment
conda activate projectA

# Install required packages
conda install -y jupyter nb_conda ipykernel mpi

# Add environment to available kernel list
python -m ipykernel install --user --name projectA

# deactivate conda environment
conda deactivate
```

Once the base environment is setup the user must add an `env` section to the `kernel.json` file, located in directory `${USER}/.local/share/jupyter/kernels/projecta`, defining the `CONDA_PREFIX` and `PATH` variables.  Currently, Polaris compute nodes access the internet through a proxy.  To configure the kernel to use the proxy add variables `http_proxy` and `https_proxy` to the `env` section.  This will allow users to install packages form within the notebook using `!conda` magic commands.  The following is a sample configuration:

```
{
 "argv": [
  "/home/<user>/.conda/envs/projectA/bin/python",
  "-m",
  "ipykernel_launcher",
  "-f",
  "{connection_file}"
 ],
 "display_name": "projectA",
 "language": "python",
 "env": {
    "CONDA_PREFIX":"/home/<user>/.conda/envs/projecta",
    "PATH":"/home/<user>/.conda/envs/projecta/bin:${PATH}",
    "http_proxy":"http://proxy.alcf.anl.gov:3128",
    "https_proxy":"http://proxy.alcf.anl.gov:3128"
 },
 "metadata": {
  "debugger": true
 }
}
```

After completing these steps, you will see projectA kernel when you click new on the Jupyter Hub home page or when you use Kernel menu on a Jupyter notebook.


## Accessing Project Folders:

Jupyterhub file browser limits the user view files and directories within their home directory. To access project directory located outside of the user home directory a symbolic link to the directory mist be created within the user home directory. An example of this is:

 If a user is connected to the Cooley instances and wants to access Theta project ABC, the user should execute the following commands to create the links:

```
From terminal:
ln -s /project/ABC ABC_project
ln -s /lus/theta-fs0/projects/EFG EFG_project

From notebook:
!ln -s /project/ABC ABC_project
!ln -s /lus/theta-fs0/projects/EFG EFG_project
```


## Running Notebook on a compute node

The ThetaGPU and Polaris instance of JupyterHub allow users to start Jupyter Notebooks on compute nodes through the given job scheduler.  The job will be executed according to ALCF’s  queue and scheduling policy (Note:  If the queued Job does not start within 2 minutes JupyterHub will timeout and the job will be removed from queue)

### ThetaGPU

After authenticating to the ThetaGPU  JupyterHub instances the user will be presented with a drop down selection for "Select a job profile", with the options  “Local Host Process” and “ThetaGPU Compute Node”.

<figure markdown>
  ![Select a job profile](files/Jupyter-1-job-profile.png){ width="700" }
  <figcaption>Select a job profile</figcaption>
</figure>

Local Host Process” will start the Jupyter Notebook on the JupyterHub server (external to the compute resource).

"ThetaGPU Compute Node" will allow a user to start a Jupyter Notebook instance on an available compute node by requesting a node via the job scheduler, Cobalt.  When a user selects this option additional options will appear and must be selected.

- ThetaGPU Queue (MinTime/MaxTime): This field provide a list of available queues on the system.  In most cases a user should use “single-gpu” or “full-node” depending on their job requirements.
- Project List:  This field displays the active projects associated with the user on the given system (ThetaGPU).
- Runtime (minutes):  This field allows the user to set the runtime of the job in minutes.  The user should refer to the ThetaGPU Queue (MinTime/MaxTime) for the minimum and maximum runtime allowed for a selected queue.

<figure markdown>
  ![Add options](files/Jupyter-2-job-options.png){ width="700" }
  <figcaption>ThetaGPU Job options</figcaption>
</figure>

Once the appropriate information is provided the user will click the “Start” button and wait for the job to spawn.  In cases where the job queue is long the interface will time out and the job will be removed from the queue.

<figure markdown>
  ![Job queued](files/Jupyter-3-job-queued.png){ width="700" }
  <figcaption>Job queued</figcaption>
</figure>


### Polaris

The Polaris JupyterHub instance does not have a “Local Host Process” option.  All jupyter notebooks are run on a compute node through the job scheduler.  When the user authenticates the user will be presented with a “Start My Server” button and after clicking the button the user will be presented the available job options needed to start the notebook.

- Select a job profile:  This field list the current available Profiles “Polaris Compute Node”
- Queue Name: This field provide a list of available queues on the system.
- Project List: This field displays the active projects associated with the user on the given system (ThetaGPU).
- Number Chunks: This field allows the user to select the number of compute nodes to allocated to the job starts.
- Runtime (minutes:seconds) : This field allows the user to set the runtime of the job in minutes and seconds. The user should refer to the Polaris queue scheduling policy for minimum and maximum runtime allowed for a selected queue.
- File Systems: This field allow the user to select which file systems are required.   By default al file systems are checked.

<figure markdown>
  ![Add options](files/Jupyter-6-job-options.png){ width="700" }
  <figcaption>Poalris Job options</figcaption>
</figure>

Once the appropriate information is provided the user will click the “Start” button and wait for the job to spawn.  In cases where the job queue is long the interface will time out and the job will be removed from the queue.


### End a Jupyter Notebook running on a compute node ###
Failing to correctly end a running Jupyter Notebook will continue to consume the selected Project's allocation on the resource in question. When a user has completed their task in Jupyter the user should stop the Jupyter instance running on the compute node before logging out.  To stop the Notebook, click the “Control Panel” button in the top right, then click “Stop My Server”.

<figure markdown>
  ![Stop panel](files/Jupyter-4-stop-panel.png){ width="700" }
  <figcaption>Stop panel</figcaption>
</figure>

<figure markdown>
  ![Stop server](files/Jupyter-5-stop-server.png){ width="700" }
  <figcaption>Stop server</figcaption>
</figure>


### References

- [ThetaGPU Queue Policy](../theta-gpu/queueing-and-running-jobs/job-and-queue-scheduling.md)
- [Polaris Queue Policy](../running-jobs/job-and-queue-scheduling.md#Polaris-Queues)
