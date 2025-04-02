# Running Jobs on Polaris

## Queues

*******

There are five production queues you can target in your qsub (`-q <queue name>`):

| Queue Name    | Node Min | Node Max | Time Min | Time Max | Notes                                                                                                |
|---------------|----------|----------|----------|----------|------------------------------------------------------------------------------------------------------|
| debug         | 1        | 2        | 5 min    | 1 hr     | max 24 nodes in use by this queue at any given time; Only 8 nodes are exclusive (see **Note** below) |
| debug-scaling | 1        | 10       | 5 min    | 1 hr     | max 1 job running/accruing/queued **per-user**                                                       |
| prod          | 10       | 496      | 5 min    | 24 hrs   | Routing queue; See below                                                                             |
| *preemptable*   | 1        | 10       | 5 min    | 72 hrs   | ***Please be aware that jobs in the preemptable queue can be killed at any time if jobs are submitted to the demand queue.*** Max 20 jobs running/accruing/queued **per-project**; see **Note** below                              |
| *demand*        | 1        | 56       | 5 min    | 1 hr     | ***By request only***; max 100 jobs running/accruing/queued **per-project**                          |

******

**Note:** Please be aware that jobs in the preemptable queue can be killed at any time if jobs are submitted to the demand queue.
Jobs in the demand queue take priority over jobs in the preemptable queue.
This means jobs in the preemptable queue may be preempted (killed without any warning) if there are jobs in the demand queue.
Unfortunately, there's always an inherent risk of jobs being killed when using the preemptable queue. 
Please use the following command to view details of a queue: `qstat -Qf <queuename>`

To make your job rerunnable, add the following PBS directive: `#PBS -r y`. This will ensure your job will restart once the demand job is complete. 

**Note:** The debug queue has 8 exclusively dedicated nodes.
If there are free nodes in production, then debug jobs can take another 16 nodes for a total of 24.

`prod` is a routing queue and routes your job to one of the following six execution queues:

| Queue Name      | Node Min | Node Max | Time Min | Time Max | Notes                                  |
|-----------------|----------|----------|----------|----------|----------------------------------------|
| small           | 10       | 24       | 5 min    | 3 hrs    ||
| medium          | 25       | 99       | 5 min    | 6 hrs    ||
| large           | 100      | 496      | 5 min    | 24 hrs   ||
| backfill-small  | 10       | 24       | 5 min    | 3 hrs    | low priority, negative project balance |
| backfill-medium | 25       | 99       | 5 min    | 6 hrs    | low priority, negative project balance |
| backfill-large  | 100      | 496      | 5 min    | 24 hrs   | low priority, negative project balance |

- **Note 1:** You cannot submit to these queues directly; you can only submit to the routing queue "`prod`".
- **Note 2:** All of these queues have a limit of ten (10) jobs running/accruing **per-project**.
- **Note 3:** All of these queues have a limit of one hundred (100) jobs queued (not accruing score) **per-project**.
- **Note 4:** As of January 2023, it is recommended to submit jobs with a maximum node count of 476-486 nodes given current rates of downed nodes (larger jobs may sit in the queue indefinitely).

## Interactive Jobs on Compute Nodes

Here is how to submit an interactive job to, for example, edit/build/test an application on Polaris compute nodes:
```bash
qsub -I -l select=1 -l filesystems=home:eagle -l walltime=1:00:00 -q debug -A <project_name>
```

This command requests 1 node for a period of 1 hour in the debug queue, requiring access to the `/home` and `/eagle` filesystems. After waiting in the queue for a node to become available, a shell prompt on a compute node will appear. You may then start building applications and testing GPU affinity scripts on the compute node.

!!! info
    If you want to `ssh` or `scp` to one of your assigned compute nodes, you will need to make sure your `$HOME` directory and your `$HOME/.ssh` directory permissions are both set to `700`.

## Running MPI+OpenMP Applications

Once a submitted job is running, calculations can be launched on the compute nodes using `mpiexec` to start an MPI application. Documentation is accessible via `man mpiexec`, and some helpful options follow.

* `-n` total number of MPI ranks
* `-ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of CPUs per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script with directives is below for a 4-node job with 8 MPI ranks on each node and 8 OpenMP threads per rank. Each hardware thread runs a single OpenMP thread since there are 64 hardware threads on the CPU (2 per core).
You can download and compile `hello_affinity` from this [link](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity).

```bash
#!/bin/bash -l
#PBS -N AFFINITY
#PBS -l select=4:ncpus=256
#PBS -l walltime=0:10:00
#PBS -q debug-scaling
#PBS -A Catalyst  # Replace with your project

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=8 # Number of MPI ranks to spawn per node
NDEPTH=8 # Number of hardware threads per rank (i.e. spacing between MPI ranks)
NTHREADS=8 # Number of software threads per rank to launch (i.e. OMP_NUM_THREADS)

NTOTRANKS=$(( NNODES * NRANKS ))

echo "NUM_OF_NODES= ${NNODES} TOTAL_NUM_RANKS= ${NTOTRANKS} RANKS_PER_NODE= ${NRANKS} THREADS_PER_RANK= ${NTHREADS}"

# Change the directory to work directory, which is the directory you submit the job.
cd $PBS_O_WORKDIR
mpiexec --np ${NTOTRANKS} -ppn ${NRANKS} -d ${NDEPTH} --cpu-bind depth -env OMP_NUM_THREADS=${NTHREADS} ./hello_affinity
```

## Compute Node Access to the Internet

Currently, the only access to the internet is via a proxy. Here are the proxy environment variables for Polaris:

```bash
export http_proxy="http://proxy.alcf.anl.gov:3128"
export https_proxy="http://proxy.alcf.anl.gov:3128"
export ftp_proxy="http://proxy.alcf.anl.gov:3128"
```

In the future, though we don't have a timeline on this because it depends on future features in Slingshot and internal software development, we intend to have public IP addresses be a schedulable resource. For instance, if only your head node needed public access, your select statement might look something like: `-l select=1:pubnet=True+63`.

## Controlling Where Your Job Runs

If you wish to have your job run on specific nodes, form your select like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>...`. Obviously, that gets tedious for large jobs.

If you want to control the location of a few nodes, for example, 2 out of 64, but the rest don't matter, you can do something like this: `-l select=1:vnode=<node name1>+1:vnode=<node name2>+62:system=foo`.

Every node has a PBS resource called `tier0` with a rack identifier and `tier1` with a dragonfly group identifier. If you want all your nodes grouped in a rack, you can add the group specifier `-l select=8:system=foo,place=scatter:group=tier0`. If you wanted everything in the same dragonfly group, replace `tier0` with `tier1`. Note that you have to also explicitly specify the place when you use group. If you wanted a specific rack or dragonfly group instead of any of them, you are back to the select: `-l select 10:tier0=x3001-g0`.

### Network: Rack and Dragonfly Group Mappings

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
* Each compute node will have a PBS resource named `tier0` which will be equal to the values in the table below. This allows you to group your jobs within a rack if you wish. There is also a resource called `tier1` which will be equal to the column headings. This allows you to group your jobs within a dragonfly group if you wish.

|g0 |g1 |g2	|g3 |g4 |g5 |g6 |g7 |g8 |g9|
|----|----|----|----|----|----|----|----|----|----|
|x3001-g0	|x3005-g1	|x3009-g2	|x3013-g3	|x3101-g4 |x3105-g5 |x3109-g6 |x3201-g7 |x3205-g8 |x3209-g9|
|x3002-g0	|x3006-g1	|x3010-g2	|x3014-g3	|x3102-g4 |x3106-g5 |x3110-g6 |x3202-g7 |x3206-g8 |x3210-g9|
|x3003-g0	|x3007-g1	|x3011-g2	|x3015-g3	|x3103-g4 |x3107-g5 |x3111-g6 |x3203-g7 |x3207-g8 |x3211-g9 |
|x3004-g0	|x3008-g1	|x3012-g2	|x3016-g3	|x3104-g4 |x3108-g5 |x3112-g6 |x3204-g7 |x3208-g8 |x3212-g9 |

---