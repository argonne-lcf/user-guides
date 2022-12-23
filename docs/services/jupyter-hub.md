# Jupyter Hub
## Using Jupyter Hub

Jupyter provides an interactive computing environment for different languages. The most popular use case is Python; Project Jupyter is a spin-off of the interactive Python (iPython) project.

With Jupyter Hub, you can run Python and R notebooks, or open a terminal directly on your browser.

You can log in to Theta, ThetaGPU, or Cooley instances of Jupyter Hub at [https://jupyter.alcf.anl.gov](https://jupyter.alcf.anl.gov) using your ALCF credentials.
Theta Jupyter Hub instances runs on an external server ([jupyter02.mcp.alcf.anl.gov](http://jupyter02.mcp.alcf.anl.gov/)), but not on Theta login, mom, or compute nodes.
Theta Jupyter Hub has access to your home folder (```/home/$USER```) and projects folders on theta-lustre, grand, and eagle file systems (```/projects, /grand, /eagle respectively```), but does not have access to ```/opt/cray, /opt/intel, etc.—that``` is, you cannot use any Theta modules or any Cray libraries.

Cooley Jupyter Hub instances have access to the user's home folder (```/home/$USER```), and theta-lustre, grand, and eagle file systems (```/projects, /grand, /eagle respectively```).
You can submit jobs to run on Theta/Cooley with ```!qsub myjob.sh```.

You can customize your environment and add new kernels. To do so, you could run shell commands on a notebook cell by prepending them with ! (```i.e., !pwd```). However, it would be much easier to use the Jupyter terminal. You can open a terminal on the Jupyter Hub home page under Files/new menu. Using the terminal, you can create a new conda environment and a new Jupyter kernel with the commands below.

For more information on how to manage conda environments, refer to this [page](https://conda.io/docs/user-guide/tasks/manage-environments.html).

```
From terminal:
conda create -n jhub_myenv
source activate jhub_myenv
conda install jupyter nb_conda ipykernel
conda install any_module_you_need
source deactivate
 
From notebook:
!conda create -y -n jhub_myenv
!source activate jhub_myenv; conda install -y jupyter nb_conda ipykernel
!source activate jhub_myenv; conda install -y any_module_you_need
```

This will complete the installation of a new environment. To create a Jupyter kernel, additional work is necessary:
```
From terminal:
source activate jhub_myenv
python -m ipykernel install --user --name jhub_myenv
source deactivate
 
From notebook:
!source activate jhub_myenv; python -m ipykernel install --user --name jhub_myenv
```

After completing these steps, you will see jhub_myenv kernel when you click new on the Jupyter Hub home page or when you use Kernel menu on a Jupyter notebook.

- Use a name identifier (such as jhub_ prefix used above) on your Jupyter Hub conda environment to make sure you don’t mix them with conda environments you create on Cooley/Theta nodes.

## Accessing Project Folders:
Jupyterhub limits to a user home directory the user's ability to browse the file system. To access a given project directory, the user must create a symbolic link in their home directory to the project directory:

If a user is connected to the Cooley instances and wants to access Mira project ABC and Theta project EFG, the user should execute the following commands to create the links:

```
From terminal:
ln -s /project/ABC ABC_project
ln -s /lus/theta-fs0/projects/EFG EFG_project
 
From notebook:
!ln -s /project/ABC ABC_project
!ln -s /lus/theta-fs0/projects/EFG EFG_project
```

## Running a Notebook on a compute node
The following guide gives direction for running a Jupyter Notebook on a compute node.  The default behavior for Jupyter in the ALCF environment is for Cobalt job submission to happen as part of user scripting.  The ability to run a Notebook on a compute node current exists on ThetaGPU, as of October 1, 2021.  Other ALCF resources will be added in the following months.

To access the capability, the user should log into the Jupyter instances from [https://jupyter.alcf.anl.gov](https://jupyter.alcf.anl.gov/).  After authenticating for any Jupyter service supporting compute node execution, the user will be presented with a drop down selection for "Select a job profile", with the options  “Local Host Process” and “{X} Compute Node”, where {X} is the resource in question, such as "ThetaGPU Compute Node" for ThetaGPU.

<figure markdown>
  ![Select a job profile](files/Jupyter-1-job-profile.png){ width="700" }
  <figcaption>Select a job profile</figcaption>
</figure>

Local Host Process” will start the Jupyter Notebook on the JupyterHub server (external to the compute resource) following the instructions given above in Using Jupyter Hub

"{X} Compute Node" will allow a user to start a Jupyter Notebook instance on an available compute node by requesting a node via the job scheduler, Cobalt.  When a user selects "{X} Compute Node" additional options will appear and must be selected.

- "{X} Queue (MinTime/MaxTime)". This field provide a list of available queues on the system.  In most cases a user should use “single-gpu” or “full-node” depending on their job requirements.
- Project List.  This field displays the active projects associated with the user on the given system (ThetaGPU).
- Runtime (minutes).  This field allows the user to set the runtime of the job in minutes.  The user should refer to the ThetaGPU Queue (MinTime/MaxTime) for the minimum and maximum runtime allowed for a selected queue.

<figure markdown>
  ![Add options](files/Jupyter-2-job-options.png){ width="700" }
  <figcaption>Add options</figcaption>
</figure>

Once the appropriate information is provided the user will click the “Start” button and wait for the job to spawn.  In cases where the job queue is long the interface will time out and the job will be removed from the queue.

<figure markdown>
  ![Job queued](files/Jupyter-3-job-queued.png){ width="700" }
  <figcaption>Job queued</figcaption>
</figure>

### End a Jupyter Notebook running on a compute node ###
When a user has completed their task in Jupyter the user should stop the Jupyter instance running on the compute node before logging out.  Failing to correctly end a running Jupyter Notebook will continue to consume the selected Project's allocation on the resource in question.  To stop the Notebook, click the “Control Panel” button in the top right, then click “Stop My Server”.

<figure markdown>
  ![Stop panel](files/Jupyter-4-stop-panel.png){ width="700" }
  <figcaption>Stop panel</figcaption>
</figure>

<figure markdown>
  ![Stop server](files/Jupyter-5-stop-server.png){ width="700" }
  <figcaption>Stop server</figcaption>
</figure>

Note: To specify a custom Jupyter kernel, users may configure them in the files located at ```/home/<userid>/.local.share/jupyter/kernels/```. It is the user's responsibility to ensure that the Python environment specified in that custom kernel is present and functional.

## Hardware 
- CPU : Intel(R) Xeon(R) CPU E5-2683
- RAM : 512GB
- GPU: None 
