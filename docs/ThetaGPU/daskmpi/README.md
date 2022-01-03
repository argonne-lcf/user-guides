# Dask-mpi on ThetaGPU

How to run [Dask-MPI](http://mpi.dask.org/en/latest/) on **ThetaGPU** at Argonne Leadership Computing Facility.

- [Install](#install)
- [Submit a batch job](#batch)
- [Run a script in an interactive session](#interactive)
- [Start a JupyterLab interactive session](#jlab)


<a id="install"></a>
## Install

1. `ssh` into one of Theta's login nodes

    ```bash
    ssh username@theta.alcf.anl.gov
    ```

1. `ssh` into one of ThetaGPU's login nodes

    ```bash
    ssh thetagpusn1
    ```

1. Load a pre-defined conda environment (either one works fine)
    
    with pytorch
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
    ```

    with tensorflow
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/tf_master/2020-11-11/mconda3/setup.sh
    ```

1. Create a new conda environment

    ```bash
    conda create -n envname
    ```

1. Activate the environment

    ```bash
    conda activate envname
    ```

1. Install required packages

    ```bash
    conda install -c rapidsai -c nvidia -c numba -c conda-forge \
    cudf cupy python=3.7 ipython=5.8 cudatoolkit mpi4py dask dask-mpi \
    bokeh jupyter Jupyterlab ipykernel
    ```

1. Download the files `start_daskmpi.py`, `cupy_test.py` and `jupyterlab_cupy_test.ipynb` from this repository into a directory, e.g. `dirname`. 

1. Optional: create the Jupyter kernel for interactive JupyterLab sessions

    ```bash
    env=$(basename `echo $CONDA_PREFIX`)
    python -m ipykernel install --user --name "$env" --display-name "Python [conda env:"$env"]"
    ```
    


### Local storage

If your dataset is larger than the combined memory of all compute nodes, Dask will spill excess data to disk. 
If you do not have write permission to local storage on the compute nodes, spilling to disk will be disabled by default, as explained [here](https://docs.dask.org/en/latest/setup/hpc.html#local-storage). 

<!-- See these [instructions](https://www.alcf.anl.gov/support-center/theta/theta-file-systems) on how to request access to local storage on Theta's compute nodes. -->



<a id="batch"></a>
## Submit a batch job

This will run `cupy_test.py` using 8 GPUs on one node.

1. `ssh` into one of Theta's login nodes

    ```bash
    ssh username@theta.alcf.anl.gov
    ```

1. `ssh` into one of ThetaGPU's login nodes

    ```bash
    ssh thetagpusn1
    ```

1. Submit a batch job

    - on a **single** node

        ```bash
        qsub -n 1 -q full-node -A datascience -t 00:30:00 daskmpi_job_gpu.sh
        ```
    
        where the script `daskmpi_job_gpu.sh` is
    
        ```bash
        #!/bin/bash
        source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
        conda activate envname
        cd dirname
        export PYTHONPATH='/full/path/to/dirname'
        mpirun -np 9 python start_daskmpi.py cupy_test.py
        ``` 
    
        The number of processors (`-np`) should be (at least) the number of GPUs per node plus one.

    - on **multiple** nodes

        ```bash
        qsub -n 2 -q full-node -A datascience -t 00:30:00 daskmpi_job_gpu.sh
        ```
    
        where the script `daskmpi_job_gpu.sh` is
    
        ```bash
        #!/bin/bash
        source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
        conda activate envname
        cd dirname
        export PYTHONPATH='/full/path/to/dirname'
        mpirun -x LD_LIBRARY_PATH -x PYTHONPATH -x PATH -np 20 -npernode 10 --hostfile $COBALT_NODEFILE python start_daskmpi.py cupy_test.py
        ```
        
        The number of processors per node (`-npernode`) should be (at least) the number of GPUs per node plus one.    
        The number of processors (`-np`) should be `-npernode` times the number of nodes requested. 


<a id="interactive"></a>
## Run a script in an interactive session

1. `ssh` into one of Theta's login nodes

    ```bash
    ssh username@theta.alcf.anl.gov
    ```

1. `ssh` into one of ThetaGPU's login nodes

    ```bash
    ssh thetagpusn1
    ```

1. Submit an interactive job on `n` nodes

    ```bash
    qsub -n 2 -q full-node -A datascience -I -t 00:30:00
    ```

    A shell opens up on one of the compute nodes


1. Load a pre-defined conda environment (either one works fine)
    
    with pytorch
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
    ```

    with tensorflow
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/tf_master/2020-11-11/mconda3/setup.sh
    ```


1. Activate the environment created in the [install](#install) section

    ```bash
    conda activate envname
    ```


1. `cd` into the directory `dirname` where files `start_daskmpi.py` and `cupy_test.py` are 
    
    ```bash
    cd dirname
    ```


1. `export` the directory where `cupy_test.py` is to `$PYTHONPATH`

    ```bash
    export PYTHONPATH='/full/path/to/dirname'
    ```


1. Run the example script `cupy_test.py` (taken from "[GPU Dask Arrays, first steps: throwing Dask and CuPy together](https://blog.dask.org/2019/01/03/dask-array-gpus-first-steps)")
    
    - **single node**
    
        ```bash
        mpirun -np 9 python start_daskmpi.py cupy_test.py
        ```
        
        The number of processors (`-np`) should be (at least) the number of GPUs per node plus one.

        The output should be similar to the following: 

        ```
        Starting the scheduler
        Scheduler address: tcp://172.23.2.199:8786 on node thetagpu11
        Starting the workers
        Client status: <Client: 'tcp://172.23.2.199:8786' processes=8 threads=8, memory=270.50 GB>
        
        
        To connect to the Dask dashboard, execute the following command in a shell on your local machine:
            ssh -t -L 8787:localhost:8787 username@theta.alcf.anl.gov ssh -t -L 8787:localhost:8787 thetagpusn1 ssh -t -L 8787:localhost:8787 thetagpu11
        
        To open the Dask dashboard, go to:
            http://localhost:8787/status
        
        
        Operation on dask.array (from https://blog.dask.org/2019/01/03/dask-array-gpus-first-steps):
        with gpus
        elapsed time: 3.932972
        
        without gpus
        elapsed time: 47.268787

        Code ran successfully.
        Successfully exited
        ```

    - **multiple nodes**
    
        ```bash
        mpirun -x LD_LIBRARY_PATH -x PYTHONPATH -x PATH -np 20 -npernode 10 --hostfile $COBALT_NODEFILE python start_daskmpi.py cupy_test.py
        ```
        
        The number of processors per node (`-npernode`) should be (at least) the number of GPUs per node plus one.    
        The number of processors (`-np`) should be `-npernode` times the number of nodes requested. 

        The output should be similar to the following: 

        ```
        Starting the scheduler
        Scheduler address: tcp://172.23.2.192:8786 on node thetagpu04
        Starting the workers
        Client status: <Client: 'tcp://172.23.2.192:8786' processes=16 threads=16, memory=541.00 GB>
        
        
        To connect to the Dask dashboard, execute the following command in a shell on your local machine:
            ssh -t -L 8787:localhost:8787 username@theta.alcf.anl.gov ssh -t -L 8787:localhost:8787 thetagpusn1 ssh -t -L 8787:localhost:8787 thetagpu04
        
        To open the Dask dashboard, go to:
            http://localhost:8787/status
        
        
        Operation on dask.array (from https://blog.dask.org/2019/01/03/dask-array-gpus-first-steps):
        with gpus
        elapsed time: 4.121156
        
        without gpus
        elapsed time: 23.829560

        Code ran successfully.
        Successfully exited
        ```

        NB: In the `cupy_test.py` example the matrix size is not big enough to see a reduction of execution time between 8 GPUs (1 node) and 16 GPUs (2 nodes). Increase the value of variable `size` in `cupy_test.py` to make full use of the 16 GPUs. 


#### Dask dashboard

You can connect to the Dask dashboard on `http://localhost:8787/status` in you browser after you run the `ssh` command printed in the above output message in a shell on your local machine.




<a id="jlab"></a>
## Start a JupyterLab interactive session

1. `ssh` into one of Theta's login nodes

    ```bash
    ssh username@theta.alcf.anl.gov
    ```

1. `ssh` into one of ThetaGPU's login nodes

    ```bash
    ssh thetagpusn1
    ```

1. Submit an interactive job on `n` nodes

    ```bash
    qsub -n 2 -q full-node -A datascience -I -t 00:30:00
    ```

    A shell opens up on one of the compute nodes


1. Load a pre-defined conda environment (either one works fine)
    
    with pytorch
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/pt_master/2020-11-25/mconda3/setup.sh
    ```

    with tensorflow
    ```bash
    source /lus/theta-fs0/software/thetagpu/conda/tf_master/2020-11-11/mconda3/setup.sh
    ```


1. Activate the environment created in the [install](#install) section

    ```bash
    conda activate envname
    ```


1. `cd` into the directory `dirname` where file `start_daskmpi.py` is
    
    ```bash
    cd dirname
    ```


1. Run the `start_daskmpi.py` script without any argument to start a JupyterLab session
    
    - **single node**
    
        ```bash
        mpirun -np 9 python start_daskmpi.py
        ```
        
        The number of processors (`-np`) should be (at least) the number of GPUs per node plus one.


    - **multiple nodes**
    
        ```bash
        mpirun -x LD_LIBRARY_PATH -x PYTHONPATH -x PATH -np 20 -npernode 10 --hostfile $COBALT_NODEFILE python start_daskmpi.py
        ```
        
        The number of processors per node (`-npernode`) should be (at least) the number of GPUs per node plus one.    
        The number of processors (`-np`) should be `-npernode` times the number of nodes requested. 


    The output should be similar to the following: 

    ```
    Starting the scheduler
    Scheduler address: tcp://172.23.2.203:8786 on node thetagpu15
    Starting the workers
    Client status: <Client: 'tcp://172.23.2.203:8786' processes=8 threads=8, memory=270.50 GB>
    Starting JupyterLab on the scheduler...
    
    To connect to JupyterLab and Dask dashboard, execute the following command in a shell on your local machine:
        ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 username@theta.alcf.anl.gov ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 thetagpusn1 ssh -t -L 7787:localhost:7787 -L 8787:localhost:8787 thetagpu15
    
    To open JupyterLab, go to (see log file /home/username/dask_logs/jupyterlab.log):
        http://localhost:7787/?token=444f71ede62dfe1718f2499baee3483893fc14d6c7e97b88
    
    To open the Dask dashboard, go to:
        http://localhost:8787/status
    
    JupyterLab started. Type 'stop' to stop Dask: 
    ```

1. JupyterLab and Dask dashboard: 
You can connect to JupyterLab on `http://localhost:7787/` in you browser and view the Dask dashboard on `http://localhost:8787/status` after you run the `ssh` command printed in the above output message in a shell on your local machine.

    You can try the example notebook `jupyterlab_cupy_test.ipynb`. 


1. Type `stop` to terminate the Dask session.


