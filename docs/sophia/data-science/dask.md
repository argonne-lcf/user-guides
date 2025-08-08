# Dask

[Dask](https://www.dask.org/) is a Python library for parallel and distributed computing. A Dask cluster is composed of one scheduler that coordinates the job of many workers, which can have access to CPU or GPU resources. Here we show how to install Dask in a conda environment on Sophia and how to start a cluster with GPU workers.

## Login to Sophia:

`ssh username@sophia.alcf.anl.gov`

## Follow the instructions specified [here](https://docs.alcf.anl.gov/sophia/compiling-and-linking/compiling-and-linking-overview/) to start an interactive job on Sophia:
`qsub -I -l select=1 -l walltime=HH:MM:SS -q by-gpu -A <myProjectName> -l filesystems=home:eagle`

## Load modules and install Dask:

```bash linenums="1"
module use /soft/modulefiles
module load conda
conda activate
cd /path/to/work/directory/
```

### Make sure Dask is installed:

`pip install dask`

## Start Dask Scheduler:

`dask scheduler --scheduler-file ~/scheduler.json`

## Start Dask Workers:

```bash linenums="1" title="dask_worker.sh"
#!/bin/bash
echo $1 #this is the name of gpu must be unique - my-gpu-node1
for i in {0..7}; do
  CUDA_VISIBLE_DEVICES=$i \
    dask cuda worker --scheduler-file ~/scheduler.json \
      --protocol tcp \
      --nthreads 1 \
      --device-memory-limit 40GB \
      --name $1-$i &
done
```

`chmod a+x ./dask_worker.sh`

Start the dask workers on each node 
`./dask_worker.sh workers_node1`

`workers_node1` is the unique name of the worker on one node.
This script will spawn 8 threads one for each GPU on a node.

## Dask services:

The file `scheduler.json` contains information about services that lists additional endpoints 
For e.g., the Bokeh dashboard port would be listed as follows:

```json
"services": {
    "dashboard": 8787
  }
```

## Connect your client in Python with:

```python linenums="1"
from dask.distributed import Client
client = Client(scheduler_file='/full/path/to/scheduler.json')
print("Total number of worker threads =", len(client.nthreads()))
```
 and Dask away!
