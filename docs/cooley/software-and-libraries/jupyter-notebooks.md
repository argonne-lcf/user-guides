# Jupyter Notebooks on Cooley
## Jupyter Notebooks on Cooley
Frequently, it's very useful for prototyping, analysis, or debugging to have an interactive session on Cooley using jupyter notebooks to debug your python scripts. The single node containers supported by the ALCF datascience group have jupyter and common python packages installed, and so it is possible to use jupyter notebooks with the production ML/DL software in an interactive way.

For more information about the supported software in the containers, or alternative ways to get pytorch/tensorflow, please see the page on Machine Learning Tools: ==update link to Cooley/Machine Learning Tools==.

## Setting up Jupyter on an Interactive node
To use jupyter notebooks on the GPUs, you will need an interactive node. Please refer to the cobalt and job submission documentation for details, but a simple interactive node request could look like this:
```
qsub -I -n 1 -t 60 -A [project] -q debug --attrs nox11.  
```

If you need network access from your interactive node, be sure to use the pubnet queues.

Once your interactive job has started, take note of which node you are on.  On Cooley, it's possible to use ssh from the login node directly to the worker nodes. There are several ways to see which node your job is running on. The easiest is to see in your terminal the `user@ccXXX` where XXX is the node number, ranging from 001 to 0016 for the debug queue, and cc017 to cc126 for the other queues. You can also find the node information from `qstat -u [username]`.

From the interactive node, you can launch one of the datascience containers ("singularity exec --nv /soft/datascience/singularity/pytorch/centos7-cuda9.0-torch1.0.img  bash"). This will give you a shell inside the container, and from there you can start a jupyter notebook with:
```
juypter notebook.
```

You will see output from jupyter, including a link to access the notebook in a browser.  The link will look something like `http: //localhost:8888/?token=[long string of numbers and letters]`.  This should indicate that your notebook is up and running on the interactive node.  The port in the above address (8888) can be configured with the `--port` syntax to `jupyter notebook`, if you want to use a different port.

## Connecting your laptop browser to your juypter notebook
To use your browser on your laptop with jupyter hub, connect to cooley with port forwarding to the login node. The easiest way to do this is to open a new terminal and create an ssh connection to Cooley and connect the port number from your jupyter notebook. 

If it's port 8888 like in the example above, do:
```
ssh -L 8888:localhost:8888 user@cooley.alcf.anl.gov.  
```

From the login node, make a second port-forwarding ssh connection to the interactive node running your jupyter notebook:
```
`ssh -L 8888:localhost:8888 ccXXX`, 
```
where ccXXX is the interactive node with the correct numbers.  This will forward the port running jupyter notebook on the interactive node to the login node, and from the login node to your local computer.  You can copy/paste the link from jupyter into your browser, and a jupyter hub window should appear as normal in your browser.  Since it is running the datascience container on the interactive node, you will have access to tensorflow or pytorch (whichever you picked) and the other available software from your jupyter notebook.

Running your software on the GPUs is then exactly the same as in a normal python script, by calling `x.cuda()` for a pytorch tensor x for example.

For questions or bug reports on jupyter on Cooley, please email [support@alcf.anl.gov](https://mailto:support@alcf.anl.gov)





