# Dask

[Dask](https://www.dask.org/) is a Python library for parallel and distributed computing. A Dask cluster is composed of one scheduler that coordinates the job of many workers, which can have access to CPU or GPU resources. 

[RAPIDS](https://rapids.ai/) is a suite of software libraries by NVIDIA for "building end-to-end data science and analytics pipelines on GPUs". 
For example, RAPIDS' [`cuDF`](https://github.com/rapidsai/cudf), [`cuPY`](https://github.com/cupy/cupy), [`cuML`](https://github.com/rapidsai/cuml) libraries implement common Pandas, Numpy and Scikit-learn APIs, respectively, allowing to run them at scale on a GPU cluster, using Dask.

Here we show how to install RAPIDS and Dask in a conda environment on Sophia and how to start a cluster with GPU workers.


## Install RAPIDS and Dask

1. Login to Sophia
   ```bash
   ssh <username>@sophia.alcf.anl.gov
   ```

1. Start an interactive session. Follow the instructions specified [here](https://docs.alcf.anl.gov/sophia/queueing-and-running-jobs/running-jobs/) to start an interactive job on Sophia. 
In the example command below we request 2 GPUs:
   ```bash
   qsub -I -l select=2 -l walltime=HH:MM:SS -q by-gpu -A <myProjectName> -l filesystems=home:eagle
   ```

1. Load modules
   ```bash
   module load compilers/openmpi
   module load conda  
   ```

1. Follow the [installation instructions on the RAPIDS Docs](https://docs.rapids.ai/install), select the appropriate CUDA Version (you can find it in the output of `nvidia-smi`), and copy the installation command, which should be similar to the one below (replace `/path/to/env/rapids-25.06_sophia` with your preferred path and name for the environment):
   ```bash
   conda create -y -p /path/to/env/rapids-25.06_sophia -c rapidsai -c conda-forge -c nvidia rapids=25.06 python=3.11 'cuda-version>=12.0,<=12.8'
   # activate the environment
   conda activate /path/to/env/rapids-25.06_sophia
   ```

1. *Optional*: Install `jupyterlab` and create a `ipykernel`
   ```bash
   conda install -y ipykernel jupyterlab-nvdashboard dask-labextension
   env=$(basename `echo $CONDA_PREFIX`) && \
   python -m ipykernel install --prefix=${CONDA_PREFIX} --name "$env" --display-name "Python ["$env"]"
   ```


## Start a Dask cluster

The following script, `dask_start.sh`, starts a Dask cluster:

```bash linenums="1" title="dask_start.sh"
#!/bin/bash
# $1 : number of ranks per node (dafault: 8)

TMP_EXE=dask_start_worker.sh
cat > ${TMP_EXE} << EOF
#!/bin/bash
CUDA_VISIBLE_DEVICES=\${OMPI_COMM_WORLD_LOCAL_RANK} dask cuda worker \
  --device-memory-limit 40GB \
  --scheduler-file ~/scheduler.json \
  --protocol tcp \
  >/tmp/dask_worker_\${OMPI_COMM_WORLD_RANK}_\${OMPI_COMM_WORLD_LOCAL_RANK}_\${HOSTNAME}.log 2>&1
EOF
chmod 755 ${TMP_EXE}

# start the scheduler
rm -f ~/scheduler.json
echo "starting the scheduler"
nohup dask scheduler --scheduler-file ~/scheduler.json  >/tmp/dask_scheduler.log 2>&1 &
sleep 10

# start the workers
NUM_NODES=$(cat $PBS_NODEFILE | wc -l)
NRANKS_PER_NODE=${1:-8}
echo "starting" ${NRANKS_PER_NODE} "workers per node on" ${NUM_NODES} "nodes"
mpiexec  -np $((NRANKS_PER_NODE*NUM_NODES)) ./${TMP_EXE}

rm ./${TMP_EXE}
```

1. Copy the script to Sophia and make it executable: `chmod a+x ./dask_start.sh`

1. On a compute node, load modules and activate the Dask conda environment created in [Install RAPIDS and Dask](#install-rapids-and-dask)
   ```bash
   module load compilers/openmpi
   module load conda  
   conda activate /path/to/env/rapids-25.06_sophia
   ```

1. Start the Dask cluster
   ```bash
   ./dask_start.sh <num_gpus> &
   ```
   where `<num_gpus>` is the number of GPUs per node requested (the default is 8).


## Connect to the Dask cluster from python

The following python script, `dask_example.py`, shows how to connect to a running Dask cluster, print the GPU uuid of each worker, and shut down the cluster:

```python linenums="1" title="dask_example.py"
from dask.distributed import Client
from pathlib import Path
client = Client(scheduler_file=f'{Path.home()}/scheduler.json')

def get_gpu_uuid():
    import cupy as cp
    gpu_id = cp.cuda.runtime.getDeviceProperties(0)['uuid'].hex()
    return gpu_id

workers = list(client.scheduler_info()['workers'].values())
num_workers = len(workers)
print(f"{num_workers} workers")

futures = client.map(lambda x: get_gpu_uuid(), range(num_workers))
results = client.gather(futures)
for i, result in enumerate(results):
    print(f"Worker {workers[i]['name']} - GPU uuid: {result}")

client.shutdown()
```

Example output:
```
2 workers
Worker sophia-gpu-11-1 - GPU uuid: 7aa25d84eb04a33caccb57993b5945ad
Worker sophia-gpu-11-0 - GPU uuid: 4cc42dc4974265766c6d26ae48c05f85
```

