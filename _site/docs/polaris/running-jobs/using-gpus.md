# Using GPUs on Polaris Compute Nodes

Polaris compute nodes each have 4 NVIDIA A100 GPUs; more details are in [Machine Overview](../index.md).

## Discovering GPUs

When on a compute node (for example in an [Interactive Job](./index.md#interactive-jobs-on-compute-nodes)), you can discover information about the available GPUs, and processes running on them, with the command `nvidia-smi`.

## Running GPU-enabled Applications

GPU-enabled applications will run on the compute nodes using PBS submission scripts like the ones in [Running Jobs](./index.md).

Some GPU specific considerations:

- The environment variable `MPICH_GPU_SUPPORT_ENABLED=1` needs to be set if your application requires GPU-aware MPI support whereby the MPI library sends and receives data directly from GPU buffers. In this case, it will be important to have the `craype-accel-nvidia80` module loaded both when compiling your application and during runtime to correctly link against a GPU Transport Layer (GTL) MPI library. Otherwise, you'll likely see `GPU_SUPPORT_ENABLED is requested, but GTL library is not linked` errors during runtime.
- If running on a specific GPU or subset of GPUs is desired, then the `CUDA_VISIBLE_DEVICES` environment variable can be used. For example, if one only wanted an application to access the first two GPUs on a node, then setting `CUDA_VISIBLE_DEVICES=0,1` could be used.

### Binding MPI ranks to GPUs

The Cray MPI on Polaris does not currently support binding MPI ranks to GPUs. For applications that need this support, this instead can be handled by use of a small helper script that will appropriately set `CUDA_VISIBLE_DEVICES` for each MPI rank. One example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu) where each MPI rank is similarly bound to a single GPU with round-robin assignment.

An example `set_affinity_gpu_polaris.sh` script follows where GPUs are assigned round-robin to MPI ranks.

```bash linenums="1" title="set_affinity_gpu_polaris.sh"
#!/bin/bash -l
num_gpus=4
# need to assign GPUs in reverse order due to topology
# See Polaris Device Affinity Information:
# https://www.alcf.anl.gov/support/user-guides/polaris/hardware-overview/machine-overview/index.html
gpu=$((${num_gpus} - 1 - ${PMI_LOCAL_RANK} % ${num_gpus}))
export CUDA_VISIBLE_DEVICES=$gpu
echo "RANK= ${PMI_RANK} LOCAL_RANK= ${PMI_LOCAL_RANK} gpu= ${gpu}"
exec "$@"
```
This script can be placed just before the executable in the `mpiexec` command like so:
```bash
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_polaris.sh ./hello_affinity
```
Users with different needs, such as assigning multiple GPUs per MPI rank, can modify the above script to suit their needs.


### Running Multiple MPI Applications on a Node

Multiple applications can be run simultaneously on a node by launching several `mpiexec` commands and backgrounding them. For performance, it will likely be necessary to ensure that each application runs on a distinct set of CPU resources and/or targets specific GPUs. One can provide a list of CPUs using the `--cpu-bind` option, which when combined with `CUDA_VISIBLE_DEVICES` provides a user with specifying exactly which CPU and GPU resources to run each application on. In the example below, four instances of the application are simultaneously running on a single node. In the first instance, the application is spawning MPI ranks 0-7 on CPUs 24-31 and using GPU 0. This mapping is based on output from the `nvidia-smi topo -m` command and pairs CPUs with the closest GPU.

```bash
export CUDA_VISIBLE_DEVICES=0
mpiexec -n 8 --ppn 8 --cpu-bind list:24:25:26:27:28:29:30:31 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=1
mpiexec -n 8 --ppn 8 --cpu-bind list:16:17:18:19:20:21:22:23 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=2
mpiexec -n 8 --ppn 8 --cpu-bind list:8:9:10:11:12:13:14:15 ./hello_affinity &

export CUDA_VISIBLE_DEVICES=3
mpiexec -n 8 --ppn 8 --cpu-bind list:0:1:2:3:4:5:6:7 ./hello_affinity &

wait
```

## Running Multiple Processes per GPU

There are 2 approaches to launching multiple concurrent processes on a single Nvidia GPU: using the Multi-Process Service (MPS) which will run multiple logical threads per device and using MIG Mode which physically partitions the GPU for thread execution.

### Using MPS on the GPUs

Documentation for the NVIDIA Multi-Process Service (MPS) can be found [here](https://docs.nvidia.com/deploy/mps/index.html)

In the script below, note that if you are going to run this as a multi-node job you will need to do this on every compute node, and you will need to ensure that the paths you specify for `CUDA_MPS_PIPE_DIRECTORY` and `CUDA_MPS_LOG_DIRECTORY` do not "collide" and end up with all the nodes writing to the same place.

An example is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/mps) and discussed below. The local SSDs or `/dev/shm` or incorporation of the node name into the path would all be possible ways of dealing with that issue.

```bash
#!/bin/bash -l
export CUDA_MPS_PIPE_DIRECTORY=</path/writeable/by/you>
export CUDA_MPS_LOG_DIRECTORY=</path/writeable/by/you>
CUDA_VISIBLE_DEVICES=0,1,2,3 nvidia-cuda-mps-control -d
echo "start_server -uid $( id -u )" | nvidia-cuda-mps-control
```

To verify the control service is running:

```bash linenums="1"
nvidia-smi | grep -B1 -A15 Processes
```

And the output should look similar to this:

``` { .bash .no-copy}
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|    0   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    1   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    2   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
|    3   N/A  N/A     58874      C   nvidia-cuda-mps-server             27MiB |
+-----------------------------------------------------------------------------+
```

To shut down the service:

`echo "quit" | nvidia-cuda-mps-control`

To verify the service shut down properly:

`nvidia-smi | grep -B1 -A15 Processes`

And the output should look like this:

``` { .bash .no-copy}
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+
```

In some instances, users may find it useful to set the environment variable `CUDA_MPS_ACTIVE_THREAD_PERCENTAGE` which limits the fraction of the GPU available to a MPS process.  Over provisioning of the GPU is permitted, i.e. the sum across all MPS processes may exceed 100 per cent.

### Using MPS in Multi-node Jobs

As stated earlier, it is important to start the MPS control service on each node in a job that requires it. An example is available in the [Getting Started Repo](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/mps). The helper script `enable_mps_polaris.sh` can be used to start the MPS on a node.

```bash linenums="1"
#!/bin/bash -l

export CUDA_MPS_PIPE_DIRECTORY=/tmp/nvidia-mps
export CUDA_MPS_LOG_DIRECTORY=/tmp/nvidia-log
CUDA_VISIBLE_DEVICES=0,1,2,3 nvidia-cuda-mps-control -d
echo "start_server -uid $( id -u )" | nvidia-cuda-mps-control
```

The helper script `disable_mps_polaris.sh` can be used to disable MPS at appropriate points during a job script, if needed.

```bash linenums="1"
#!/bin/bash -l

echo quit | nvidia-cuda-mps-control
```

In the example job script `submit.sh` below, MPS is first enabled on all nodes in the job using `mpiexec -n ${NNODES} --ppn 1` to launch the enablement script using a single MPI rank on each compute node. The application is then run as normally. If desired, a similar one-rank-per-node `mpiexec` command can be used to disable MPS on all the nodes in a job.

```bash linenums="1"
#!/bin/bash -l
#PBS -l select=1:system=polaris
#PBS -l place=scatter
#PBS -l walltime=0:30:00
#PBS -q debug
#PBS -A Catalyst
#PBS -l filesystems=home:eagle

cd ${PBS_O_WORKDIR}

# MPI example w/ 8 MPI ranks per node spread evenly across cores
NNODES=`wc -l < $PBS_NODEFILE`
NRANKS_PER_NODE=8
NDEPTH=8
NTHREADS=1

NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))
echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS_PER_NODE} THREADS_PER_RANK= ${NTHREADS}"

# Enable MPS on each node allocated to job
mpiexec -n ${NNODES} --ppn 1 ./enable_mps_polaris.sh

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./hello_affinity

mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_polaris.sh ./hello_affinity

# Disable MPS on each node allocated to job
mpiexec -n ${NNODES} --ppn 1 ./disable_mps_polaris.sh
```

### Multi-Instance GPU (MIG) mode

MIG mode can be enabled and configured on Polaris by passing a valid configuration file to `qsub`:
```bash
qsub ... -l mig_config=/home/ME/path/to/mig_config.json ...
```

You can find a concise explanation of MIG concepts and terms at [NVIDIA MIG User Guide](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html#concepts).

#### Configuration

Please study the following example of a valid configuration file:

```json
{
  "group1": {
    "gpus": [0,1],
    "mig_enabled": true,
    "instances": {"7g.40gb": ["4c.7g.40gb", "3c.7g.40gb"] }
  },
  "group2": {
    "gpus": [2,3],
    "mig_enabled": true,
    "instances": {"3g.20gb": ["2c.3g.20gb", "1c.3g.20gb"], "2g.10gb": ["2g.10gb"], "1g.5gb": ["1g.5gb"]}
  }
}
```

#### Notes

- Group names are arbitrary but must be unique.
- `"gpus"` must be an array of integers. If only one physical GPU is being configured in a group, it must still be contained within an array (e.g., `"gpus": [0],`).
- Only groups with `mig_enabled` set to `true` will be configured.
- `instances` denote the MIG GPU instances and the nested compute instances you wish to be configured.
  - Syntax is `{"gpu instance 1": ["cpu instance 1", "cpu instance 2"], ...}`.
  - Valid GPU instances are `1g.5gb`, `1g.10gb`, `2g.10gb`, `3g.20gb`, `4g.20gb`, and `7g.40gb`. The first number denotes the number of slots used out of 7 total, and the second number denotes memory in GB.
  - The default CPU instance for any GPU instance has the same identifier as the GPU instance (in which case it will be the only one configurable).
  - Other CPU instances can be configured with the identifier syntax `Xc.Y`, where `X` is the number of slots available in that GPU instance, and `Y` is the GPU instance identifier string.
  - Some GPU instances cannot be configured adjacently, despite there being sufficient slots/memory remaining (e.g., `3g.20gb` and `4g.20gb`). Please see NVIDIA MIG documentation for further details.
- Currently, MIG configuration is only available in the debug, debug-scaling, and preemptable queues. Submissions to other queues will result in any MIG config files passed being silently ignored.
- Files that do not match the above syntax will be silently rejected, and any invalid configurations in properly formatted files will be silently ignored. Please test any changes to your configuration in an interactive job session before use.
- A basic validator script is available at `/soft/pbs/mig_conf_validate.sh`. It will check for simple errors in your config and print the expected configuration. For example:

```shell
ascovel@polaris-login-02:~> /soft/pbs/mig_conf_validate.sh -h
usage: mig_conf_validate.sh -c CONFIG_FILE
ascovel@polaris-login-02:~> /soft/pbs/mig_conf_validate.sh -c ./polaris-mig/mig_config.json
expected MIG configuration:
GPU     GPU_INST   COMPUTE_INST
-------------------------------
0       7g.40gb    4c.7g.40gb
0       7g.40gb    3c.7g.40gb
1       7g.40gb    4c.7g.40gb
1       7g.40gb    3c.7g.40gb
2       2g.10gb    2g.10gb
2       4g.20gb    2c.4g.20gb
2       4g.20gb    2c.4g.20gb
3       2g.10gb    2g.10gb
3       4g.20gb    2c.4g.20gb
3       4g.20gb    2c.4g.20gb
ascovel@polaris-login-02:~>
```

### Example use of MIG compute instances

The following example demonstrates the use of MIG compute instances via the `CUDA_VISIBLE_DEVICES` environment variable:

```shell
ascovel@polaris-login-02:~/polaris-mig> qsub -l mig_config=/home/ascovel/polaris-mig/mig_config.json -l select=1 -l walltime=60:00 -l filesystems=home:eagle -A Operations -q R639752 -k doe -I
qsub: waiting for job 640002.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov to start
qsub: job 640002.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov ready

ascovel@x3209c0s19b0n0:~> cat ./polaris-mig/mig_config.json
{
  "group1": {
    "gpus": [0,1],
    "mig_enabled": true,
    "instances": {"7g.40gb": ["4c.7g.40gb", "3c.7g.40gb"] }
  },
  "group2": {
    "gpus": [2,3],
    "mig_enabled": true,
    "instances": {"4g.20gb": ["2c.4g.20gb", "2c.4g.20gb"], "2g.10gb": ["2g.10gb"] }
  }
}
ascovel@x3209c0s19b0n0:~> nvidia-smi -L | grep -Po -e "MIG[0-9a-f\-]+"
MIG-63aa1884-acb8-5880-a586-173f6506966c
MIG-b86283ae-9953-514f-81df-99be7e0553a5
MIG-79065f64-bdbb-53ff-89e3-9d35f270b208
MIG-6dd56a9d-e362-567e-95b1-108afbcfc674
MIG-76459138-79df-5d00-a11f-b0a2a747bd9e
MIG-4d5c9fb3-b0e3-50e8-a60c-233104222611
MIG-bdfeeb2d-7a50-5e39-b3c5-767838a0b7a3
MIG-87a2c2f3-d008-51be-b64b-6adb56deb679
MIG-3d4cdd8c-fc36-5ce9-9676-a6e46d4a6c86
MIG-773e8e18-f62a-5250-af1e-9343c9286ce1
ascovel@x3209c0s19b0n0:~> for mig in $( nvidia-smi -L | grep -Po -e "MIG[0-9a-f\-]+" ) ; do CUDA_VISIBLE_DEVICES=${mig} ./saxpy & done 2>/dev/null
ascovel@x3209c0s19b0n0:~> nvidia-smi | tail -n 16
+-----------------------------------------------------------------------------+
| Processes:                                                                  |
|  GPU   GI   CI        PID   Type   Process name                  GPU Memory |
|        ID   ID                                                   Usage      |
|=============================================================================|
|    0    0    0      17480      C   ./saxpy                          8413MiB |
|    0    0    1      17481      C   ./saxpy                          8363MiB |
|    1    0    0      17482      C   ./saxpy                          8413MiB |
|    1    0    1      17483      C   ./saxpy                          8363MiB |
|    2    1    0      17484      C   ./saxpy                          8313MiB |
|    2    1    1      17485      C   ./saxpy                          8313MiB |
|    2    5    0      17486      C   ./saxpy                          8313MiB |
|    3    1    0      17487      C   ./saxpy                          8313MiB |
|    3    1    1      17488      C   ./saxpy                          8313MiB |
|    3    5    0      17489      C   ./saxpy                          8313MiB |
+-----------------------------------------------------------------------------+
ascovel@x3209c0s19b0n0:~>
```

---
