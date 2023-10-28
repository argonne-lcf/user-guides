# JupyterHub

JupyterHub is an open-source service application that enables users to launch
separate Jupyter instances on a remote server. [ALCF
JupyterHub](https://jupyter.alcf.anl.gov) provides access to Polaris, ThetaGPU,
Theta, and Cooley with the same [authentication
protocol](../account-project-management/accounts-and-access/alcf-passcode-tokens.md)
that is used to access these systems, but through a web interface rather than a
terminal. On the [ALCF JupyterHub home page](https://jupyter.alcf.anl.gov),
users can choose their desired system. Upon selection, they'll be directed to
the sign-in page to enter their ALCF username and [passcode
token](../account-project-management/accounts-and-access/alcf-passcode-tokens.md).

<figure markdown>
  ![JupyterHub](files/Jupyter-0-login.png){ width="700" }
  <figcaption>ALCF JupyterHub home page and sign-in screen</figcaption>
</figure>

We describe below how to use JupyterHub on Polaris, ThetaGPU, Theta, and Cooley
in more detail.

## Polaris

The Polaris JupyterHub server runs on a Polaris login node and launches individual 
users' environments on the compute nodes through the PBS job scheduler. 
After the authentication step, the user will be presented with the
menu of the available job options to start the Jupyter instance.

- Select a job profile:  This field lists the available profiles, which is
  limited to “Polaris Compute Node” at this time.
- Queue Name: This field provide a list of available queues on the system.
- Project List: This field displays the active projects associated with the user
  on Polaris.
- Number of Nodes: This field allows the user to select the number of compute
  nodes to be allocated.
- Runtime (minutes:seconds): This field allows the user to set the runtime of
  the job in minutes and seconds. The user should refer to the [Polaris queue
  scheduling
  policy](../theta-gpu/queueing-and-running-jobs/job-and-queue-scheduling.md)
  for minimum and maximum runtime allowed for the selected queue.
- File Systems: This field allows the user to select the file systems to be
  mounted. By default all the file systems are selected.

<figure markdown>
  ![Add options](files/Jupyter-6-job-options.png){ width="700" }
  <figcaption>Polaris Job options</figcaption>
</figure>

Once the appropriate information is provided the user will click the “Start”
button and wait for the job to spawn. If there's an extended wait time due to a
lengthy job queue, the interface might time out, leading to the job's removal
from the queue. If not, the job kicks off and it begins to use up the user's
allocation based on the chosen job options. It's crucial for users to shut down
the server when resources are no longer required. Failing to do so will result
in continued consumption of the allocated time until the predetermined runtime
concludes. 

## ThetaGPU

The ThetaGPU JupyterHub instance can run either on an external server or
directly on ThetaGPU compute nodes. After the authentication step, the user will
be presented with a drop-down menu to "Select a job profile", with the options
“Local Host Process” and “ThetaGPU Compute Node” as shown below.

<figure markdown>
  ![Select a job profile](files/Jupyter-1-job-profile.png){ width="700" }
  <figcaption>Select a job profile</figcaption>
</figure>

"Local Host Process” will start the Jupyter Notebook on the JupyterHub server
(external to the compute resource).

"ThetaGPU Compute Node" will allow a user to start a Jupyter Notebook instance
on an available compute node by requesting a node via the job scheduler, Cobalt.
When a user selects this option additional options will appear as shown below.

- ThetaGPU Queue (MinTime/MaxTime): This field provide a list of available
  queues on the system with the minimum and maximum times allowed for each
  queue.
- Project List:  This field displays the active projects associated with the
  user on the given system (ThetaGPU).
- Runtime (minutes):  This field allows the user to set the runtime of the job
  in minutes. Please note that minimum and maximum times are shown on the menu.
  You may refer to the [ThetaGPU queue scheduling
  policy](../theta-gpu/queueing-and-running-jobs/job-and-queue-scheduling.md)
  for more details.

<figure markdown>
  ![Add options](files/Jupyter-2-job-options.png){ width="700" }
  <figcaption>ThetaGPU Job options</figcaption>
</figure>

Once the appropriate information is provided the user will click the “Start”
button and wait for the job to spawn. In cases where the job queue is long the
interface will time out and the job will be removed from the queue.

<figure markdown>
  ![Job queued](files/Jupyter-3-job-queued.png){ width="700" }
  <figcaption>Job queued</figcaption>
</figure>

> **_NOTE:_** If you would like to change your selection about where to run the
> Jupyter instance after the Notebook started, you need to stop the server to be
> able to see the drop-down menu again.

## Theta and Cooley
JupyterHub for Cooley and Theta deploy notebooks on an external server that
mounts the same home directory
([swift-home](../data-management/filesystem-and-storage/file-systems.md)) as
used in these systems. However, users cannot directly access the compute nodes
for these systems. We would like to note that both Cooley and Theta will be
retired by the end of 2023, therefore we recommend our users to switch to
Polaris and ThetaGPU instead.


## Additional Notes
 
### Custom IPython Kernels

ALCF JupyterHub provides a set of pre-configured IPython kernels for the users
to select. However, users may need custom kernels with additional packages
installed. This can be achieved by first creating custom Python environments
either through [venv](https://docs.python.org/3/library/venv.html) or
[conda](https://conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html).
More information on creating custom Python environments can be found in our
documentation for [Polaris](../polaris/data-science-workflows/python.md) and
[ThetaGPU](../theta-gpu/data-science-workflows/building-python-packages.md).
After activating the custom environment, `ipykernel` package needs to be
installed with the following command:
```bash
pip install ipykernel
```
Once `ipykernel` is installed, the custom kernel can be added to the list of
available kernels with the following command:
```bash
python -m ipykernel install --user --name custom_kernel_name 
```
where `custom_kernel_name` is the name of the kernel that will appear in the
kernel list. This name does not have to match the name of the environment, but
should not contain spaces. If you want more flexibility in naming, you can add
the `--display-name` argument as shown below.
```bash
python -m ipykernel install --user --name custom_kernel_name --display-name "Polaris Python 3.11 Tensorflow 2.4.1" 
```
Note that, you still need to provide `--name` with a simple name that does not
contain spaces. Additionally, you can also set environment variables for the
kernel with the `--env` argument, i.e:
```bash
python -m ipykernel install --user --name custom_kernel_name --env http_proxy http://proxy.alcf.anl.gov:3128 --env https_proxy http://proxy.alcf.anl.gov:3128
```
You can see the list of available kernels with the following command:
```bash
jupyter kernelspec list
```
By default, the kernels are installed in the user's home directory under
`~/.local/share/jupyter/kernels/`. All the configuration is specified in the
`kernel.json` file under the kernel directory. For the example above, the path
for the json file will be
`~/.local/share/jupyter/kernels/custom_kernel_name/kernel.json`. You can edit
this file to add additional environment variables or change the display name.

Once you've followed the steps above, your new kernel will be visible on
JupyterHub. It's recommended to perform these steps in a terminal, ideally on
the login node of the system you're using. After setting up a custom kernel, you
can easily add more packages directly within JupyterHub. Simply create a new
notebook using your custom kernel and use the %pip or %conda magic commands to
install packages. If you're on a compute node, remember to enable internet
access by configuring the `http_proxy` and `https_proxy` environment variables as
previously mentioned.

### Accessing Project Folders

Jupyter file browser limits the user to view files and directories within their
home directory. To access directories located outside of the user home directory
a symbolic link to the directory must be created within the user home directory.
An example of this is:

```bash
ln -s /project/ABC ~/ABC_project_link
```
Please note that one can run any shell command directly on a Jupyter notebook by
simply adding an exclamation mark, `!`, to the beginning of the command. For
example, the above command can be run from a notebook cell as follows:

```bash
!ln -s /project/ABC ~/ABC_project_link
```

### Ending a Jupyter Notebook running on a compute node
Failing to correctly end a running Jupyter Notebook will continue to consume the
selected project's allocation on the resource in question. When a user has
completed their task in Jupyter the user should stop the Jupyter instance
running on the compute node before logging out.  To stop the Notebook, click the
“Control Panel” button in the top right, then click “Stop My Server”.

<figure markdown>
  ![Stop panel](files/Jupyter-4-stop-panel.png){ width="700" }
  <figcaption>Stop panel</figcaption>
</figure>

<figure markdown>
  ![Stop server](files/Jupyter-5-stop-server.png){ width="700" }
  <figcaption>Stop server</figcaption>
</figure>


## Resources
* Jupyter Lab [documentation](https://jupyterlab.readthedocs.io/en/stable/).
* ALCF Hands-on HPC Workshop presentation on Python and Jupyter on Polaris: [slides](https://www.alcf.anl.gov/support-center/training-assets/python-jupyter-notebook-and-containers) and [video](https://youtu.be/fhCe5eO1RSM).
* ALCF webinar on JupyterHub: [slides](https://github.com/keceli/ezHPC/blob/main/webinar/jupyterhub_webinar.pdf) and [video](https://youtu.be/X9g9eQcYseI?feature=shared).
