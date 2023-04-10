
# Running Jobs on Polaris

##  <a name="Polaris-Queues"></a>Queues
There are five production queues you can target in your qsub (`-q <queue name>`):

| Queue Name    | Node Min | Node Max	 | Time Min                    | Time Max | Notes                                                                       |
|---------------|----------|-----------|-----------------------------|----------|-----------------------------------------------------------------------------|
| debug         | 1        | 2         | 10 min (5 min from 10/3/22) | 1 hr     | max 8 nodes in use by this queue ay any given time                          |
| debug-scaling | 1        | 10        | 10 min (5 min from 10/3/22) | 1 hr     | max 1 job running/accruing/queued **per-user**                              |
| prod          | 10       | 496       | 30 min (5 min from 10/3/22) | 24 hrs   | Routing queue; See below                                                    |
| preemptable   | 1        | 10        | 5 min                       | 72 hrs   | max 20 jobs running/accruing/queued **per-project**; see note below         |
| demand        | 1        | 56        | 5 min                       | 1 hr     | ***By request only***; max 100 jobs running/accruing/queued **per-project** |

**Note:** Jobs in the demand queue take priority over jobs in the preemptable queue.
This means jobs in the preemptable queue may be preempted (killed without any warning) if there are jobs in the demand queue.
Please use the following command to view details of a queue: ```qstat -Qf <queuename>```

`prod` is routing queue and routes your job to one of the following six execution queues:

|Queue Name |Node Min |Node Max	| Time Min                     |Time Max | Notes                                  |
|----|----|----|------------------------------|----|----------------------------------------|
|small|10|24| 5 min  |6 hrs||
|medium|25|49| 5 min  |12 hrs||
|large|50|496| 5 min  |24 hrs||
|backfill-small|10|24| 5 min   |6 hrs| low priority, negative project balance |
|backfill-medium|25|49| 5 min  |12 hrs| low priority, negative project balance |
|backfill-large|50|496| 5 min                     |24 hrs| low priority, negative project balance |

- **Note 1:** You cannot submit to these queues directly, you can only submit to the routing queue "prod".
- **Note 2:** All of these queues have a limit of ten (10) jobs running/accruing **per-project**
- **Note 3:** All of these queues have a limit of one hundred (100) jobs queued (not accruing score) **per-project**
- **Note 4:** As of January 2023, it is recommended to submit jobs with a maximum node count of 476-486 nodes given current rates of downed nodes (larger jobs may sit in the queue indefinitely).

## <a name="Running-MPI+OpenMP-Applications"></a>Running MPI+OpenMP Applications
Once a submitted job is running calculations can be launched on the compute nodes using `mpiexec` to start an MPI application. Documentation is accessible via `man mpiexec` and some helpful options follow.

* `-n` total number of MPI ranks
* `-ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of cpus per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script with directives is below for a 4-node job with 32 MPI ranks on each node and 8 OpenMP threads per rank (1 per CPU).

```bash
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4:ncpus=256
#PBS -l walltime=0:10:00
#PBS -q debug-scaling
#PBS -A Catalyst

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=32 # Number of MPI ranks to spawn per node
NDEPTH=8 # Number of hardware threads per rank (i.e. spacing between MPI ranks)
NTHREADS=8 # Number of software threads per rank to launch (i.e. OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS} THREADS_PER_RANK= ${NTHREADS}"

cd /home/knight/affinity
mpiexec --np ${NTOTRANKS} -ppn ${NRANKS} -d ${NDEPTH} --cpu-bind depth -env OMP_NUM_THREADS=${NTHREADS} ./hello_affinity
```

## <a name="Running-GPU-enabled-Applications"></a>Running GPU-enabled Applications
GPU-enabled applications will similarly run on the compute nodes using the above example script.
- The environment variable `MPICH_GPU_SUPPORT_ENABLED=1` needs to be set if your application requires MPI-GPU support whereby the MPI library sends and receives data directly from GPU buffers. In this case, it will be important to have the `craype-accel-nvidia80` module loaded both when compiling your application and during runtime to correctly link against a GPU Transport Layer (GTL) MPI library. Otherwise, you'll likely see `GPU_SUPPORT_ENABLED is requested, but GTL library is not linked` errors during runtime.
- If running on a specific GPU or subset of GPUs is desired, then the `CUDA_VISIBLE_DEVICES` environment variable can be used. For example, if one only wanted an application to access the first two GPUs on a node, then setting `CUDA_VISIBLE_DEVICES=0,1` could be used.

### <a name="Binding-MPI-ranks-to-GPUs"></a>Binding MPI ranks to GPUs
The Cray MPI on Polaris does not currently support binding MPI ranks to GPUs. For applications that need this support, this instead can be handled by use of a small helper script that will appropriately set `CUDA_VISIBLE_DEVICES` for each MPI rank. One example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu) where each MPI rank is similarly bound to a single GPU with round-robin assignment.

A example `set_affinity_gpu_polaris.sh` script follows where GPUs are assigned round-robin to MPI ranks.

```bash
#!/bin/bash -l
num_gpus=4
# need to assign GPUs in reverse order due to topology
# See Polaris Device Affinity Information:
# https://www.alcf.anl.gov/support/user-guides/polaris/hardware-overview/machine-overview/index.html
gpu=$((${num_gpus} - 1 - ${PMI_LOCAL_RANK} % ${num_gpus}))
export CUDA_VISIBLE_DEVICES=$gpu
echo “RANK= ${PMI_RANK} LOCAL_RANK= ${PMI_LOCAL_RANK} gpu= ${gpu}”
exec "$@"
```
This script can be placed just before the executable in the `mpiexec` command like so.
```bash
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_polaris.sh ./hello_affinity
```
Users with different needs, such as assigning multiple GPUs per MPI rank, can modify the above script to suit their needs.


## <a name="Interactive-Jobs-on-Compute-Nodes"></a>Interactive Jobs on Compute Nodes

Here is how to submit an interactive job to, for example, edit/build/test an application Polaris compute nodes:
```
qsub -I -l select=1:filesystems=home:eagle -l walltime=1:00:00 -q debug
```

This command requests 1 node for a period of 1 hour in the debug queue, requiring access to the /home and eagle filesystems. After waiting in the queue for a node to become available, a shell prompt on a compute node will appear. You may then start building applications and testing gpu affinity scripts on the compute node.

**NOTE:** If you want to ```ssh``` or ```scp``` to one of your assigned compute nodes you will need to make sure your ```$HOME``` directory and your ```$HOME/.ssh``` directory permissions are both set to ```700```.

## <a name="Running-Multiple-MPI-Applications-on-a-node"></a>Running Multiple MPI Applications on a node
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


## <a name="Compute-Node-Access-to-the-Internet"></a>Compute Node Access to the Internet

Currently, the only access the internet is via a proxy.  Here are the proxy environment variables for Polaris:

```bash
export http_proxy="http://proxy-01.pub.alcf.anl.gov:3128"
export https_proxy="http://proxy-01.pub.alcf.anl.gov:3128"
export ftp_proxy="http://proxy-01.pub.alcf.anl.gov:3128"
```

In the future, though we don't have a timeline on this because it depends on future features in slingshot and internal software development, we intend to have public IP addresses be a schedulable resource.  For instance, if only your head node needed public access your select statement might looks something like: `-l select=1:pubnet=True+63`.

## <a name="Controlling-Where-Your-Job-Runs"></a>Controlling Where Your Job Runs
If you wish to have your job run on specific nodes form your select like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>...` . Obviously, that gets tedious for large jobs.

If you want to control the location of a few nodes, for example 2 out of 64, but the rest don't matter, you can do something like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>+62:system=foo`

Every node has a PBS resource called `tier0` with a rack identifier and `tier1` with a dragonfly group identifieer.  If you want all your nodes grouped in a rack, you can add the group specifier `-l select=8:system=foo,place=scatter:group=tier0`.  If you wanted everything in the same dragonfly group, replace `tier0` with `tier1`.  Note that you have to also explicitly specify the place when you use group.  If you wanted a specific rack or dragonfly group instead of any of them, you are back to the select: `-l select 10:tier0=x3001-g0`.

### <a name="Rack-and-Dragonfly-Group-Mappings"></a>Network: Rack and Dragonfly Group Mappings
* Racks contain (7) 6U chassis; each chassis has 2 nodes for 14 nodes per rack
* The hostnames are of the form xRRPPc0sUUb[0|1]n0 where:
    * RR is the row {30, 31, 32}
    * PP is the position in the row {30 goes 1-16, 31 and 32 go 1-12}
    * c is chassis and is always 0
    * s stands for slot, but in this case is the RU in the rack and values are {1,7,13,19,25,31,37}
    * b is BMC controller and is 0 or 1 (each node has its own BMC)
    * n is node, but is always 0 since there is only one node per BMC
* So, 16+12+12 = 40 racks * 14 nodes per rack = 560 nodes.
* Note that in production group 9 (the last 4 racks) will be the designated on-demand racks
* The management racks are x3000 and X3100 and are dragonfly group 10
* The TDS rack is x3200 and is dragonfly group 11
* Each compute node will have a PBS resource named `tier0` which will be equal to the values in the table below.  This allows you to group your jobs within a rack if you wish.  There is also a resource called `tier1` which will be equal to the column headings.  This allows you to group your jobs within a dragonfly group if you wish.

|g0 |g1 |g2	|g3 |g4 |g5 |g6 |g7 |g8 |g9|
|----|----|----|----|----|----|----|----|----|----|
|x3001-g0	|x3005-g1	|x3009-g2	|x3013-g3	|x3101-g4 |x3105-g5 |x3109-g6 |x3201-g7 |x3205-g8 |x3209-g9|
|x3002-g0	|x3006-g1	|x3010-g2	|x3014-g3	|x3102-g4 |x3106-g5 |x3110-g6 |x3202-g7 |x3206-g8 |x3210-g9|
|x3003-g0	|x3007-g1	|x3011-g2	|x3015-g3	|x3103-g4 |x3107-g5 |x3111-g6 |x3203-g7 |x3207-g8 |x3211-g9 |
|x3004-g0	|x3008-g1	|x3012-g2	|x3016-g3	|x3104-g4 |x3108-g5 |x3112-g6 |x3204-g7 |x3208-g8 |x3212-g9
