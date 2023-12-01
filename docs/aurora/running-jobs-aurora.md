
# Running Jobs on Aurora

##  <a name="Aurora-Queues"></a>Queues

There is a single routing queue in place called `EarlyAppAccess` which currently has a node count of 2,844, but we recommend a max job size of 2048 or 2560. This will be replaced by new queues during an upcoming PM.

For example, a one-node interactive job can be requested for 30 minutes with the following command, where `[your_ProjectName]` is replaced with an appropriate project name.

```
qsub -l select=1 -l walltime=30:00 -A [your_ProjectName] -q EarlyAppAccess -I
```

Recommended PBSPro options follow.

```
#!/bin/sh
#PBS -A [your_ProjectName]
#PBS -N
#PBS -l walltime=[requested_walltime_value]
#PBS -k doe
#PBS -l place=scatter
#PBS -q EarlyAppAccess
```

## Working Around Node Failures

As Aurora is still a pre-production supercomputer, node failures are a fact of life. If you would like to increase the chances that a large job does not terminate due to a node failure, you may choose to interactively route your MPI job around nodes that fail during your run. To do this, you must run interactively and use must manually adjust your run on the fly to remove nodes that have been marked as failed.

We recommend against useing `-W tolerate_node_failures=all` in your qsub command, but we acknowledge its use can be helpful. However, you MUST MANUALLY VERIFY your job and remove faulted nodes from your mpiexec command YOURSELF!

1. Start your interactive job
2. When the job transitions to Running state, run `pbsnodes -l | grep <jobid>`
3. Manually REMOVE all nodes identified in that output from inclusion in your mpiexec
4. Continue to execute
5. If other nodes go down during your job, it will not be killed, and you can further exclude those nodes from your mpiexec as needed

It is important to note that all nodes marked as faulty by PBX will not be used in subsequent jobs. This mechanism only provides you with a means to execute additional mpiexec commands under the same interactive job after manually removing nodes identified as faulty. Once your PBS job has exited, those faulty nodes will remain offline until further intervention by Aurora staff.

## <a name="Running-MPI+OpenMP-Applications"></a>Running MPI+OpenMP Applications

Once a submitted job is running calculations can be launched on the compute nodes using `mpiexec` to start an MPI application. Documentation is accessible via `man mpiexec` and some helpful options follow.

* `-n` total number of MPI ranks
* `-ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of cpus per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script with directives is below for a 4-node job with 28 MPI ranks on each node and 4 OpenMP threads per rank (1 per CPU core).

```bash
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4
#PBS -l place=scatter
#PBS -l walltime=0:10:00
#PBS -q workq
#PBS -A MYPROJECT

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=28 # Number of MPI ranks to spawn per node
NDEPTH=4 # Number of hardware threads per rank (i.e. spacing between MPI ranks)
NTHREADS=4 # Number of software threads per rank to launch (i.e. OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS} THREADS_PER_RANK= ${NTHREADS}"

cd /home/knight/affinity
mpiexec -n ${NTOTRANKS} -ppn ${NRANKS} --depth=${NDEPTH} --cpu-bind depth -env OMP_NUM_THREADS=${NTHREADS} --env OMP_PLACES=cores ./hello_affinity
```

## <a name="Running-GPU-enabled-Applications"></a>Running GPU-enabled Applications
GPU-enabled applications will similarly run on the compute nodes using the above example script.
- The environment variable `MPICH_GPU_SUPPORT_ENABLED=1` needs to be set if your application requires MPI-GPU support whereby the MPI library sends and receives data directly from GPU buffers.
- If running on a specific GPU or subset of GPUs and/or tiles is desired, then the `CUDA_VISIBLE_DEVICES` environment variable can be used. For example, if one only wanted an application to access the first two GPUs on a node, then setting `CUDA_VISIBLE_DEVICES=0,1` could be used.

### <a name="Binding-MPI-ranks-to-GPUs"></a>Binding MPI ranks to GPUs
Support in MPICH on Aurora to bind MPI ranks to GPUs is currently work-in-progress. For applications that need this support, this instead can be handled by use of a small helper script that will appropriately set `ZE_AFFINITY_MASK` for each MPI rank. One example is where each MPI rank is similarly bound to a single GPU tile with round-robin assignment.

A example `set_affinity_gpu_aurora.sh` script follows where GPU tiles are assigned round-robin to MPI ranks.

```bash
#!/bin/bash -l
num_gpus=6
num_tiles=2
gpu_id=$(( (PALS_LOCAL_RANKID / num_tile ) % num_gpu ))
tile_id=$((PALS_LOCAL_RANKID % num_tile))
unset EnableWalkerPartition
export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1
export ZE_AFFINITY_MASK=$gpu_id.$tile_id
#echo “RANK= ${PMI_RANK} LOCAL_RANK= ${PMI_LOCAL_RANK} gpu= ${gpu}”
exec "$@"
```
This script can be placed just before the executable in the `mpiexec` command like so.
```bash
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_aurora.sh ./hello_affinity
```
Users with different needs, such as assigning multiple GPUs/tiles per MPI rank, can modify the above script to suit their needs. 


## <a name="Interactive-Jobs-on-Compute-Nodes"></a>Interactive Jobs on Compute Nodes

Here is how to submit an interactive job to, for example, edit/build/test an application Polaris compute nodes:
```
qsub -I -l select=1,walltime=1:00:00,place=scatter -A MYPROJECT -q workq
```

This command requests 1 node for a period of 1 hour in the `workq` queue. After waiting in the queue for a node to become available, a shell prompt on a compute node will appear. You may then start building applications and testing gpu affinity scripts on the compute node.

**NOTE:** If you want to ```ssh``` or ```scp``` to one of your assigned compute nodes you will need to make sure your ```$HOME``` directory and your ```$HOME/.ssh``` directory permissions are both set to ```700```.

## <a name="Running-Multiple-MPI-Applications-on-a-node"></a>Running Multiple MPI Applications on a node
Multiple applications can be run simultaneously on a node by launching several `mpiexec` commands and backgrounding them. For performance, it will likely be necessary to ensure that each application runs on a distinct set of CPU resources and/or targets specific GPUs and tiles. One can provide a list of CPUs using the `--cpu-bind` option, which when combined with `ZE_AFFINITY_MASK` provides a user with specifying exactly which CPU and GPU resources to run each application on. In the simple example below, twelve instances of the application are simultaneously running on a single node. In the first instance, the application is spawning MPI ranks 0-3 on CPU cores 0-3 and using GPU 0 tile 0.

```bash
export OMP_NUM_THREADS=1
export ZE_ENABLE_PCI_ID_DEVICE_ORDER=1

export ZE_AFFINITY_MASK=0.0
mpiexec --np 4 --ppn 4 --cpu-bind list:0:1:2:3 ./hello_affinity &

export ZE_AFFINITY_MASK=0.1
mpiexec -n 4 --ppn 4 --cpu-bind list:4:5:6:7 ./hello_affinity &

export ZE_AFFINITY_MASK=1.0
mpiexec -n 4 --ppn 4 --cpu-bind list:8:9:10:11 ./hello_affinity &


export ZE_AFFINITY_MASK=5.1
mpiexec -n 4 --ppn 4 --cpu-bind list:40:41:42:43 ./hello_affinity &    

wait
```
Users will likely find it beneficial to launch processes across CPU cores in both sockets of a node.

## <a name="Compute-Node-Access-to-the-Internet"></a>Compute Node Access to the Internet

Currently, the only access to the internet is via a proxy.  Here are the proxy environment variables for Aurora:

```bash
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
```

#In the future, though we don't have a timeline on this because it depends on future features in slingshot and internal software development, we intend to have public IP addresses be a schedulable resource.  For instance, if only your head node needed public access your select statement might looks something like: `-l select=1:pubnet=True+63`.

## <a name="Controlling-Where-Your-Job-Runs"></a>Controlling Where Your Job Runs
If you wish to have your job run on specific nodes form your select like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>...` . Obviously, that gets tedious for large jobs.

If you want to control the location of a few nodes, for example 2 out of 64, but the rest don't matter, you can do something like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>+62:system=foo`.

## <a name="Rack-and-Dragonfly-Group-Mappings"></a>Network: Rack and Dragonfly Group Mappings
