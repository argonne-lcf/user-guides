# Rapids on Polaris


[Rapids](https://rapids.ai/) is a suite of software libraries by NVIDIA for "building end-to-end data science and analytics pipelines on GPUs". 
For example, Rapids' `cuDF`, `cuPY`, `cuML` libraries implement common Pandas, Numpy and Scikit-learn APIs, respectively, allowing to run them at scale on a GPU cluster, using [Dask](http://mpi.dask.org/en/latest/).

- [Install](#install)
- [Start a Rapids cluster](#start)
- [Access the cluster from JupyterLab](#jupyter)


<a id="install"></a>
## Install


1. Access one of Polaris' login nodes: `ssh username@polaris.alcf.anl.gov`. 

1. Run the following bash script `install_rapids_polaris.sh` to create a conda environment with Rapids, Jupyter, and dashboard libraries to visualize the cluster and GPU activities. It will also create a Jupyter kernel for the environment and the script `activate_rapids_env_polaris.sh` to activate the environment.    
**Before running the script**:

    - check [Rapids' official website](https://rapids.ai/start.html) for the latest versions of the library and its dependencies, and edit the script's variables `RAPIDS_VERSION`, `CUDATOOLKIT_VERSION`, `PYTHON_VERSION` accordingly

    - choose a directory where you want your conda environment to be created and store it in the `ENV_PATH` variable: for example, the conda environment in the example below will be created in `/path/to/conda/dir/rapids-23.02_polaris`

    - if you wish or need to use a different version of the base conda module to load, edit the `BASE_CONDA` variable accordingly

    - add any other Python library that you want to install in your envoironment at the end of the `conda create` command, after `dask-labextension`

    ```bash
    #!/bin/bash

    # install_rapids_polaris.sh
    
    # Install Rapids on Polaris
    # [check here for the latest version](https://rapids.ai/start.html)
    
    SYSTEM="polaris"
    RAPIDS_VERSION=23.04
    CUDATOOLKIT_VERSION=11.8
    PYTHON_VERSION=3.10
    ENV_PATH="/path/to/conda/dir"
    BASE_CONDA=2022-09-08
    
    module load conda/${BASE_CONDA} && \
    conda create -y -p ${ENV_PATH}/rapids-${RAPIDS_VERSION}_${SYSTEM} \
    -c rapidsai -c nvidia -c conda-forge rapids=${RAPIDS_VERSION} \
    python=${PYTHON_VERSION} cudatoolkit=${CUDATOOLKIT_VERSION} \
    ipykernel jupyterlab-nvdashboard dask-labextension && \
    conda activate ${ENV_PATH}/rapids-${RAPIDS_VERSION}_${SYSTEM} && \
    jupyter serverextension enable --py --sys-prefix dask_labextension && \
    env=$(basename `echo $CONDA_PREFIX`) && \
    python -m ipykernel install --user --name "$env" --display-name "Python [conda env:"$env"]"

    cat > activate_rapids_env_polaris.sh << EOF
    #!/bin/bash
    module load conda/${BASE_CONDA} && \
    conda activate ${ENV_PATH}/rapids-${RAPIDS_VERSION}_${SYSTEM} && \
    \$@
    EOF
    
    chmod 755 activate_rapids_env_polaris.sh
    ```



<a id="start"></a>
## Start a Rapids cluster

1. `ssh` into one of Polaris' login nodes

    ```bash
    ssh username@polaris.alcf.anl.gov
    ```

1. Create the following script `start_rapids_cluster_rank.sh` and change its permissions with `chmod 755 start_rapids_cluster_rank.sh`.

    ```bash
    #start_rapids_cluster_rank.sh

    ROLE=$1
    HOSTNAME=$HOSTNAME
    
    # NVLINK, IB, or TCP (default TCP)
    CLUSTER_MODE="TCP"
    
    MAX_SYSTEM_MEMORY=$(free -m | awk '/^Mem:/{print $2}')M
    DEVICE_MEMORY_LIMIT="29GB"
    POOL_SIZE="31GB"
    # A100 big mem
    # DEVICE_MEMORY_LIMIT="70GB"
    # POOL_SIZE="78GB"
    
    # Used for writing scheduler file to shared storage
    LOCAL_DIRECTORY=$HOME/dask-local-directory
    SCHEDULER_FILE=$LOCAL_DIRECTORY/scheduler.json
    LOGDIR="$LOCAL_DIRECTORY/logs"
    WORKER_DIR="/tmp/dask-workers/"
    DASHBOARD_PORT=8787
    
    # Purge Dask worker and log directories
    if [ "$ROLE" = "SCHEDULER" ]; then
        rm -rf $LOGDIR/*
        mkdir -p $LOGDIR
        rm -rf $WORKER_DIR/*
        mkdir -p $WORKER_DIR
    fi
    
    # Purge Dask config directories
    rm -rf ~/.config/dask
    
    
    # Dask/distributed configuration
    export DASK_DISTRIBUTED__COMM__TIMEOUTS__CONNECT="100s"
    export DASK_DISTRIBUTED__COMM__TIMEOUTS__TCP="600s"
    export DASK_DISTRIBUTED__COMM__RETRY__DELAY__MIN="1s"
    export DASK_DISTRIBUTED__COMM__RETRY__DELAY__MAX="60s"
    export DASK_DISTRIBUTED__WORKER__MEMORY__Terminate="False"
    
    
    # Setup scheduler
    if [ "$ROLE" = "SCHEDULER" ]; then
    
      if [ "$CLUSTER_MODE" = "NVLINK" ]; then
         CUDA_VISIBLE_DEVICES='0' DASK_UCX__CUDA_COPY=True DASK_UCX__TCP=True DASK_UCX__NVLINK=True DASK_UCX__INFINIBAND=False DASK_UCX__RDMACM=False nohup dask-scheduler --dashboard-address $DASHBOARD_PORT --protocol ucx --scheduler-file $SCHEDULER_FILE > $LOGDIR/$HOSTNAME-scheduler.log 2>&1 &
      fi
    
      if [ "$CLUSTER_MODE" = "IB" ]; then
         DASK_RMM__POOL_SIZE=1GB CUDA_VISIBLE_DEVICES='0' DASK_UCX__CUDA_COPY=True DASK_UCX__TCP=True DASK_UCX__NVLINK=True DASK_UCX__INFINIBAND=True DASK_UCX__RDMACM=False UCX_NET_DEVICES=mlx5_0:1 nohup dask-scheduler --dashboard-address $DASHBOARD_PORT --protocol ucx --interface ib0 --scheduler-file $SCHEDULER_FILE > $LOGDIR/$HOSTNAME-scheduler.log 2>&1 &
      fi
    
      if [ "$CLUSTER_MODE" = "TCP" ]; then
         CUDA_VISIBLE_DEVICES='0' nohup dask-scheduler --dashboard-address $DASHBOARD_PORT --protocol tcp --scheduler-file $SCHEDULER_FILE > $LOGDIR/$HOSTNAME-scheduler.log 2>&1 &
      fi
    fi
    
    
    # Setup workers
    if [ "$CLUSTER_MODE" = "NVLINK" ]; then
        dask-cuda-worker --device-memory-limit $DEVICE_MEMORY_LIMIT --local-directory $LOCAL_DIRECTORY --rmm-pool-size=$POOL_SIZE --memory-limit=$MAX_SYSTEM_MEMORY --enable-tcp-over-ucx --enable-nvlink  --disable-infiniband --scheduler-file $SCHEDULER_FILE >> $LOGDIR/$HOSTNAME-worker.log 2>&1
    fi
    
    if [ "$CLUSTER_MODE" = "IB" ]; then
        dask-cuda-worker --device-memory-limit $DEVICE_MEMORY_LIMIT --local-directory $LOCAL_DIRECTORY --rmm-pool-size=$POOL_SIZE --memory-limit=$MAX_SYSTEM_MEMORY --enable-tcp-over-ucx --enable-nvlink  --enable-infiniband --disable-rdmacm --scheduler-file $SCHEDULER_FILE >> $LOGDIR/$HOSTNAME-worker.log 2>&1
    fi
    
    if [ "$CLUSTER_MODE" = "TCP" ]; then
        dask-cuda-worker --device-memory-limit $DEVICE_MEMORY_LIMIT --local-directory $LOCAL_DIRECTORY --rmm-pool-size=$POOL_SIZE --memory-limit=$MAX_SYSTEM_MEMORY --scheduler-file $SCHEDULER_FILE >> $LOGDIR/$HOSTNAME-worker.log 2>&1
    fi
    ```


1. [Submit an interactive job](https://docs.alcf.anl.gov/polaris/running-jobs/) on `n` (here 2) nodes

    ```bash
    qsub -l select=2:system=polaris -l filesystems=home:eagle:grand -l walltime=00:30:00 -I -A YourProject -q YourQueue
    ```

    A shell opens up on one of the compute nodes


1. Run the following script, `~/start_rapids_cluster_polaris.sh`. This will start the scheduler on one node and one worker per GPU on all nodes.  
    
    ```bash
    #!/bin/bash

    # start_rapids_cluster_polaris.sh
    
    NUM_NODES=$(cat $PBS_NODEFILE | wc -l)
    TMP_EXE=tmp_rpds.sh
    
    cat > ${TMP_EXE} << EOF
    #!/bin/bash
    if [ \$PMI_RANK == 0 ]; then
        ~/activate_rapids_env_polaris.sh ~/start_rapids_cluster_rank.sh SCHEDULER
    else
        ~/activate_rapids_env_polaris.sh ~/start_rapids_cluster_rank.sh
    fi
    EOF
    
    chmod 755 ${TMP_EXE}
    sleep 5
    
    mpiexec -n $NUM_NODES --ppn 1 ~/${TMP_EXE}
    
    rm ~/${TMP_EXE}
    ```


1. In case of errors and if the cluster does not start, check the file `~/dask-local-directory/scheduler.json` and the log files of scheduler and workers in `~/dask-local-directory/logs/`. 


1. Example script to access the cluster from python and print information on scheduler and number of connected workers, then terminate the cluster:

    ```bash
    source ~/activate_rapids_env_polaris.sh
    python -c "from dask.distributed import Client; \
    client = Client(scheduler_file='~/dask-local-directory/scheduler.json'); \
    print(client); client.shutdown()"
    ```




<a id="jupyter"></a>
## Access the cluster from JupyterLab

1. [Start a Rapids cluster](#start)


1. Establish a [Multiplexed SSH Connection](https://en.wikibooks.org/wiki/OpenSSH/Cookbook/Multiplexing) to Polaris. Run the code below in your local machine after having edited `YourUsername`

    ```bash
    SSH_MULTIPLEX="-S ~/.ssh/multiplex:polaris.rapids YourUsername@polaris.alcf.anl.gov"  && \
    ssh -M ${SSH_MULTIPLEX}
    ```


1. On a different terminal window in your local machine, run the following script `open_jupyterlab_polaris.sh` to start a JupyterLab session on Polaris 

    ```bash
    #!/bin/bash

    # open_jupyterlab_polaris.sh

    SSH_MULTIPLEX="-S ~/.ssh/multiplex:polaris.rapids YourUsername@polaris.alcf.anl.gov"
    PORT=8675 && \
    ssh ${SSH_MULTIPLEX} "echo \$(hostname) | tee ~/jupyter_pol.log && \
    source ~/activate_rapids_env_polaris.sh 2> /dev/null  && \
    nohup jupyter lab --no-browser --port=${PORT} &>> ~/jupyter_pol.log & \
    JPYURL=''; while [ -z \${JPYURL} ]; do sleep 2; JPYURL=\$(sed -n '/[ ] .*localhost/p' ~/jupyter_pol.log | sed 's/^  *//g'); done; echo \${JPYURL}" > ~/jupyter_pol.log & \
    PORT=''; while [ -z ${PORT} ]; do sleep 2; PORT=$(sed -n 's/.*:\([0-9][0-9]*\)\/.*/\1/p' ~/jupyter_pol.log); done && \
    ssh -O forward -L $PORT:localhost:$PORT ${SSH_MULTIPLEX} && \
    echo "Open this url $(grep token ~/jupyter_pol.log)"
    ```

    Copy the url that is returned by the script and paste it in a browser window to access JupyterLab and view the dashboards.


1. On JupyterLab
    
    - Select the kernel with your Rapids' eniroment name (something like `Python [conda env:rapids-23.02_polaris]`) from the Kernel menu in the top right corner 
    
    - Select and access the dashboards from the bar on the left side

    - Connect to the cluster with 
        ```python
        from dask.distributed import Client
        client = Client(scheduler_file='~/dask-local-directory/scheduler.json')
        client
        ```

    - Shutdown the cluster with 
        ```python
        client.shutdown()
        ```


1. Run the following script `close_jupyterlab_polaris.sh` on your local machine to end the JupyterLab session and close the multiplexed connection

    ```bash
    #!/bin/bash

    # close_jupyterlab_polaris.sh

    SSH_MULTIPLEX="-S ~/.ssh/multiplex:polaris.rapids YourUsername@polaris.alcf.anl.gov"  && \
    PORT=$(sed -n 's/.*:\([0-9][0-9]*\)\/.*/\1/p' ~/jupyter_pol.log)  && \
    RUNNING_ON=$(head -1 ~/jupyter_pol.log)  && \
    ssh -O cancel -L $PORT:localhost:$PORT ${SSH_MULTIPLEX}  && \
    ssh ${SSH_MULTIPLEX} "ssh ${RUNNING_ON} \"ps -ef | grep jupyter | grep \\\`whoami\\\` | grep -v grep | awk -F ' ' '{print \\\$2}' | xargs kill -9  &&  rm ~/jupyter_pol.log\"" && \
    rm ~/jupyter_pol.log
    ```

