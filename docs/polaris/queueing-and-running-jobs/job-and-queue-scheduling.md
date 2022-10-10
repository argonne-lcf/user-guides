## Documentation / Tools
* [The PBS "BigBook"](https://help.altair.com/2022.1.0/PBS%20Professional/PBS2022.1.pdf) - This is really excellent.  We highly suggest you download it and search through it when you have questions.
* [The PBS Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) - This is the users guide.
* [The PBS Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) - If you don't download the Big Book, the Reference Guide is also useful.  It shows every option and gives you details on how to format various elements on the command line.
* [Cobalt qsub options to PBS qsub options](./pbs-qsub-options-table.md) - shows how to map cobalt command line options to PBS command line options.  Can be found at the link above.
* `qsub2pbs`  - Installed on Theta and Cooley.  Pass it a Cobalt command line and it will convert it to a PBS command line.  Add the --directives option and it will output an executable script.  Note that it outputs -l select=system=None.  You would need to change the None to whatever system you wanted to target (polaris, aurora, etc).

## Introduction
At a high level, getting computational tasks run on systems at ALCF is a two step process:

1. You request and get allocated resources (compute nodes) on one or more of the systems.  This is accomplished by interacting with the job scheduler / workload manager.  In the ALCF we use PBS Professional.
2. You execute your tasks on those resources.  This is accomplished in your job script by interacting with various system services (MPI, OpenMP, the HPE PALS job launch system, etc.)

Our documentation is organized in two sections aligned with the two steps described above.

## Obtaining and managing compute resources at ALCF

### Definitions and notes
`chunk`: *A set of resources allocated as a unit to a job. Specified inside a selection directive. All parts of a chunk come from the same host.  In a typical MPI (Message-Passing Interface) job, there is one chunk per MPI process.*

`vnode`: *A virtual node, or vnode, is an abstract object representing a host or a set of resources which form a usable part of an execution host. This could be an entire host,
or a nodeboard or a blade. A single host can be made up of multiple vnodes. Each vnode can be managed and scheduled independently. Each vnode in a complex must have a unique name. Vnodes on a host can share resources, such as node-locked licenses.*  PBS operates on vnodes.  A vnode can, and in ALCF often will, represent an entire host, but it doesn't have to.  For instance, there is a mode on Polaris where we could have each physical host look like four vnodes, each with 16 threads, 1/4 of the RAM and one A100.

`ncpus`: In ALCF, given the way we configure PBS, this equates to a hardware thread.  For example, Polaris has a single socket with a 32 core CPU, each with two threads, so PBS reports that as ncpus=64.

`ngpus`: The number of GPUs.  On Polaris, this will generally be four.  However, if we enable *Multi Instance GPU (MIG)* mode and use cgroups it could be as high as 28.

### The basics
If you are an existing ALCF user and are familiar with Cobalt, you will find the PBS commands very similar though the options to qsub are quite different.  Here are the "Big Four" commands you will use:

1. `qsub` - request resources (compute nodes) to run your job and start your script/executable on the head node.
2. `qstat` - check on the status of your request
3. `qalter` - update your request for resources
4. `qdel` - cancel an uneeded request for resources

#### `qsub` - submit a job to run
* Users Guide, Chapter 2, page UG-11 and Reference Guide Chapter 2, section 2.57, page RG-216

The single biggest difference between Cobalt and PBS is the way you select resources when submitting a job.  In Cobalt, every system had its own Cobalt server and you just specified the number of nodes you wanted (-n).  With PBS, we are planning on running a single "PBS Complex" which means there will be a single PBS server for all systems in the ALCF and you need to specify enough constraints to get your job to run on the resources you want/need.  One advantage of this is that getting resources from two different systems or "co-scheduling" is trivially possible.

##### Resource Selection and Job Placement #####
Section 2.57.2.6 RG-219 Requesting Resources and Placing jobs in the Reference Guide.

Resources come in two flavors:

* Job Wide: Walltime is the most common example of a job wide resource.  You use the `-l` option to specify job wide resources, i.e. `-l walltime=06:00:00`.  All the resources in the job have the same walltime.
  * `-l <resource name>=<value>[,<resource name>=<value> ...]`
* Chunks: (see the definition above) This is how you describe what your needs are to run your job.  You do this with the `-l select=` syntax.  In the ALCF, every node has a resource called `system` which is set to the system name it belongs to (Polaris, Aurora, etc).  This means you can typically get away with the very simple `-l select=128:system=polaris` which will give you 128 complete nodes on Polaris.  
  * `-l select=[<N>:]<chunk>[+[<N>:]<chunk> ...]` where N specifies how many of that chunk and a chunk is of the form:
  * `<resource name>=<value>[:<resource name>=<value> ...]`
  * Here is an example that would select resources from a machine like Polaris (A100s) and a hypothetical vis machine with A40 GPUs.  Note that PBS takes care of co-scheduling the nodes on the two systems for you transparently:
     * `-l select=128:ncpus=64:ngpus=4:gputype=A100+32:ncpus=64:ngpus=2:gputype=A40` 

You also have to tell PBS how you want the chunks distributed across the physical hardware.  You do that via the `-l place` option:

* `-l place=[<arrangement>][: <sharing> ][: <grouping>]` where
  * arrangement is one of `free | pack | scatter | vscatter`
     * unless you have a specific reason to do otherwise, you probably want to set this to `scatter`, otherwise you may not get what you expect.  For instance on a host with ncpus=64, if you requested `-l select=8:ncpus=8` you could end up with all of our chunks on one node.
     * `free` means PBS can distribute them as it sees fit
     * `pack` means all chunks from one host.  Note that this is not the minimum number of hosts, it is one host.  If the chunks can't fit on one host, the qsub will fail.
     * `scatter` means take only one chunk from any given host.
     * `vscatter` means take only one chunk from any given vnode.  If a host has multiple vnodes, you could end up with more than one chunk on the host.
  * sharing is one of `excl | shared | exclhost` where
     * NOTE: Node configuration can override your requested sharing mode.  For instance, in most cases ALCF sets the nodes to `force_exclhost`, so normally you don't have to specify this.
     * `excl` means this job gets the entire vnode
     * `shared` means the vnode could be shared with another job from another user.
     * `exclhost` means this job gets the entire host, even if it has multiple vnodes.
  * group=`<resource name>`
     * Below you will see the rack and dragonfly group mappings.  If you wanted to ensure that all the chunks came from dragonfly group 2, you could specify `group=g2`.  

There is an alternative to using the `group=` syntax.  The downside to `group=` is that you have to specify a specific dragonfly group, when what you may really want is for your chunks to all be in one dragonfly group, but you don't care which one.  On each node, we have defined two resources, one called `tier0` which is equal to the rack the node is in (each rack has a switch in it) and `tier1` which is equal to the dragonfly group it is in.  We have defined *placement sets* for the tier0 and tier1 resources.  If you use placement sets it will *preferentially* choose nodes from the specified resource, but it won't drain or delay your job start.

Here is a heavily commented sample PBS submission script:

```
#!/bin/bash
# UG Section 2.5, page UG-24 Job Submission Options
# Add another # at the beginning of the line to comment out a line
# NOTE: adding a switch to the command line will override values in this file.

# These options are MANDATORY at ALCF; Your qsub will fail if you don't provide them.
#PBS -A <short project name>
#PBS -l walltime=HH:MM:SS


# Highly recommended 
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N <name>
#file systems used by the job
#PBS -l filesystems=home:eagle

# If you need a queue other than the default, which is prod (uncomment to use)
##PBS -q <queue name>

# Controlling the output of your application
# UG Sec 3.3 page UG-42 Managing Output and Error Files
# By default, PBS spools your output on the compute node and then uses scp to move it the
# destination directory after the job finishes.  Since we have globally mounted file systems
# it is highly recommended that you use the -k option to write directly to the destination
# the doe stands for direct, output, error
#PBS -k doe
#PBS -o <path for stdout>
#PBS -e <path for stderr>

# If you want to merge stdout and stderr, use the -j option
# oe=merge stdout/stderr to stdout, eo=merge stderr/stdout to stderr, n=don't merge
#PBS -j n

# Controlling email notifications
# UG Sec 2.5.1, page UG-25 Specifying Email Notification
# When to send email b=job begin, e=job end, a=job abort, j=subjobs (job arrays), n=no mail
#PBS -m be 
# Be default, mail goes to the submitter, use this option to add others (uncomment to use)
##PBS -M <email addresses>

# Setting job dependencies
# UG Section 6.2, page UG-109 Using Job Dependencies
# There are many options for how to set up dependancies;  afterok will give behavior similar
# to Cobalt (uncomment to use)
##PBS depend=afterok:<jobid>:<jobid>

# Environment variables (uncomment to use)
# UG Section 6.12, page UG-126 Using Environment Variables
# RG Sect 2.57.7, page RG-233 Enviornment variables PBS puts in the job environment
##PBS -v <variable list>
## -v a=10, "var2='A,B'", c=20, HOME=/home/zzz
##PBS -V exports all the environment variables in your environnment to the compute node


# The rest is an example of how an MPI job might be set up
echo Working directory is $PBS_O_WORKDIR
cd $PBS_O_WORKDIR

echo Jobid: $PBS_JOBID
echo Running on host `hostname`
echo Running on nodes `cat $PBS_NODEFILE`

NNODES=`wc -l < $PBS_NODEFILE`
NRANKS=1           # Number of MPI ranks per node
NDEPTH=1           # Number of hardware threads per rank, spacing between MPI ranks on a node
NTHREADS=1         # Number of OMP threads per rank, given to OMP_NUM_THREADS

NTOTRANKS=$(( NNODES * NRANKS ))

echo "NUM_OF_NODES=${NNODES}  TOTAL_NUM_RANKS=${NTOTRANKS}  RANKS_PER_NODE=${NRANKS}  THREADS_PER_RANK=${NTHREADS}"

mpiexec --np ${NTOTRANKS} -ppn ${NRANKS} -d ${NDEPTH} -env OMP_NUM_THREADS=${NTHREADS} ./hello_mpi
```
### Specifying Filesystems 

**Note: The `filesystems` attribute is mandatory. If you do not specify a filesystem(s) you will receive the following error message upon submission:**

`qsub: Resource: filesystems is required to be set.` 

Your job submission (`qsub`) should specify which filesystems your job will be using.  In the event that a filesystem becomes unavailable, this information is used to preserve jobs that would use that filesystem while allowing other jobs that are not using an affected filesystem to proceed to run normally. If this is not specified (prior to 10/10/22)
all valid filesystems will be added to the command.  Add the attribute `-l filesystems` and specify a colon-delimited list. Valid filesystems are `home` (or `swift`), `eagle`, and `grand`.  For example, to request the home and eagle filesystems for your job you would add `-l filesystems=home:eagle` to your qsub command. 

If a job is submitted while a filesystem it requested is marked down, the job will be queued but will not run, with a message in the comment field of the job as to why it is not running. Run `qstat -f <jobid>` to see the comment field. For example, if the job requested for eagle and if Eagle is unavailabe, the comment field will have `Can Never Run: Insufficient amount of server resource: eagle_fs (True != False)`).  Once the affected filesystem has been returned to normal operation, and the filesystem is marked as being available, the job will then be scheduled normally. The job cannot run until all filesystems requested by the job are available.

If a job requesting a filesystem that is marked down is already in the queue, the job will be not run until all of it's requested filesystems are available.

An example of a job requesting filesystems:

`qsub -l select=10:ncpus=64,walltime=30:00,filesystems=grand:home -A ProjectX -q prod my_job.sh`

To update the filesystems list for your job, use `qalter`. 

### qsub examples ###

* `qsub -A my_allocation -l select=4:system=polaris -l filesystems=home:eagle -l walltime=30:00 -q debug-scaling -- a.out`
	* run a.out on 4 chunks on polaris with a walltime of 30 minutes in debug-scaling queue; charge my_allocation;
	* Since we allocate full nodes on Polaris, 4 chunks will be 4 nodes.  If we shared nodes, that would be 4 cores.
	* use the -- (dash dash) syntax when directly running an executable.
* `qsub -A my_allocation -l place=scatter  -l filesystems=home:eagle -l select=32:ncpus=32 -q prod -l walltime=30:00 mpi_mm_64.sh`
	* 32 chunks on any system that meets the requirements; each chunk must have 32 HW threads; `place=scatter` means use a different vnode for each chunk, even if you could fit more than one on a vnode; use the queue named prod.

#### `qstat` - Query Job/Queue Status 
* Users Guide Sec. 10.2, page UG-175; Reference Guide Sec. 2.55, page RG-200 
* NOTE: By default, the columns are fixed width and will truncate information.
* The most basic: `qstat` - will show all jobs queued and running in the system
* Only a specific users jobs: `qstat -u <my username>`
* Detailed information about a specific job: `qstat -f <jobid> [<jobid> <jobid>...]`
    * The comment field with the `-f` output can often tell you why your job isn't running or why it failed.
* Display status of a queue: `qstat -Q <queue name>`
* Display details of a queue: `qstat -Qf <queue name>` 
* Display status of a completed job: `qstat -x <jobid> [<jobid> <jobid>...]`
    * This has to be turned on (we have);  It is configured to keep 2 weeks of history.
* Get estimated start time: `qstat -T <jobid>`
* Make output parseable: `qstat -F [json | dsv]`
    * That is `dsv` (delimeter) not `csv`;  The default delimiter is `|`, but -D can change it for instance `-D,` would use a comma instead.

#### `qalter` - Alter a job submission 
* Users Guide Sec. 9.2, page UG-168; Reference Guide Sec. 2.40, page RG-130
* Basically takes the same options as `qsub`;  Say you typoed and set the walltime to 300 minutes instead of 30 minutes.  You could fix it (if the job had not started running) by doing `qalter -l walltime=30:00 <jobid> [<jobid> <jobid>...]` 
* The new value overwrites any previous value.

#### `qdel` - Delete a job: 
*  Users Guide Sec. 9.3, page UG-170; Reference Guide Sec. 2.41, page RG-143
*  `qdel <jobid> [<jobid> <jobid>...]`

#### `qmove` - Move a job to a different queue
* Users Guide Sec. 9.7, page UG-173; Reference Guide Sec. 2.46, page RG-175
* `qmove <new queue> <jobid> [<jobid> <jobid>...]`
* Only works before a job starts running

#### `qhold, qrls` - Place / release a user hold on a job
* Reference Guide Sec 2.44, page RG-150 and Sec 2.50, page RG-183
* `[qhold | qrls] <jobid> [<jobid> <jobid>...]`

#### `qselect` - Query jobids for use in commands
* Users Guide Sec. 10.1, page UG-175; Reference Guide Sec. 2.52, page RG-189
* ```qdel `qselect -N test1` ``` will delete all the jobs that had the job name set to `test1`.

#### `qmsg` Write a message string into one or more output files of the job
* Users Guide Sec. 9.4, page UG-171; Reference Guide Sec. 2.47, page RG-177
* `qmsg -E -O "This is the message" <jobid> [<jobid> <jobid>...]`
* `-E` writes it to standard error, `-O` writes it to standard out

#### `qsig` Send a signal to a job
* Users Guide Sec. 9.5, page UG-172; Reference Guide Sec. 2.53, page RG-195
* `qsig -s <signal> <jobid> [<jobid> <jobid>...]`
* If you don't specify a signal, `SIGTERM` is sent.

#### `tracejob` Get log information about your job
* Reference Guide Sec 2.59, page RG-238
*  `tracejob <jobid>`

## Getting information about the state of the resources ##

### `qstat` Get information about the server or queues ###
* Users Guide Sec. 10.3 & 10.4, page UG-188 - UG-189
* `qstat -B[f]` - Check the server status
* `qstat -Q[f] <queue name>` - Check the queue status 


### `pbsnodes` Get information about the current state of nodes ###
* Reference Guide Sec 2.7 page RG-36
* This is more for admins, but it can tell you what nodes are free (state), how many "CPUs" which is actually the number of threads (ncpus), how many GPUs (ngpus) which with A100s can change depending on the MIG mode, and if the node is shared or not (sharing).

`pbsnodes -av` - Everything there is to know about a node

```
aps-edge-dev-04
     Mom = aps-edge-dev-04.mcp.alcf.anl.gov
     ntype = PBS
     state = free
     pcpus = 128
     resources_available.arch = linux
     resources_available.host = aps-edge-dev-04
     resources_available.mem = 527831088kb
     resources_available.ncpus = 128
     resources_available.ngpus = 1
     resources_available.vnode = aps-edge-dev-04
     resources_assigned.accelerator_memory = 0kb
     resources_assigned.hbmem = 0kb
     resources_assigned.mem = 0kb
     resources_assigned.naccelerators = 0
     resources_assigned.ncpus = 0
     resources_assigned.vmem = 0kb
     resv_enable = True
     sharing = force_exclhost
     license = l
     last_state_change_time = Tue Oct  5 21:58:57 2021
```
`pbsnodes -avSj` - A nice table to see what is free and in use

```
(base) [allcock@edtb-01 20220214-22:53:26]> pbsnodes -avSj
                                                        mem       ncpus   nmics   ngpus
vnode           state           njobs   run   susp      f/t        f/t     f/t     f/t   jobs
--------------- --------------- ------ ----- ------ ------------ ------- ------- ------- -------
edtb-01         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-02         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-03         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-04         free                 0     0      0      0 b/0 b     0/0     0/0     0/0 --
edtb-01[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-01[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-02[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-02[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-03[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-03[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
edtb-04[0]      free                 0     0      0  250gb/250gb   64/64     0/0     1/1 --
edtb-04[1]      free                 0     0      0  251gb/251gb   64/64     0/0     1/1 --
```
`pbsnodes -l` - (lowercase  l) see which nodes are down;  The comment often indicates why it is down

```
[20220217-21:10:31]> pbsnodes -l
x3014c0s19b0n0       offline,resv-exclusive Xid 74 -- GPUs need reseat
x3014c0s25b0n0       offline,resv-exclusive Checking on ConnectX-5 firmware
```

##  Queues
There are three production queues you can target in your qsub (`-q <queue name>`):  

|Queue Name |Node Min |Node Max	| Time Min                    |Time Max | Notes                                              |
|----|----|----|-----------------------------|----|----------------------------------------------------|
|debug|1|2| 10 min (5 min from 10/3/22) |1 hr| max 8 nodes in use by this queue ay any given time |
|debug-scaling|1|10| 10 min (5 min from 10/3/22)|1 hr| max 1 job running/accruing/queued **per-user**     |
|prod|10|496| 30 min (5 min from 10/3/22)|24 hrs| Routing queue; See below                           |

`prod` is routing queue and routes your job to one of the following six execution queues:  

|Queue Name |Node Min |Node Max	| Time Min                     |Time Max | Notes                                  |
|----|----|----|------------------------------|----|----------------------------------------|
|small|10|24| 30 min (5 min from 10/3/22)  |6 hrs||
|medium|25|49| 30 min (5 min from 10/3/22)  |12 hrs||
|large|50|496| 30 min (5 min from 10/3/22)  |24 hrs||
|backfill-small|10|24| 30 min (5 min from 10/3/22)  |6 hrs| low priority, negative project balance |
|backfill-medium|25|49| 30 min (5 min from 10/3/22)  |12 hrs| low priority, negative project balance |
|backfill-large|50|496| 30 min (5 min from 10/3/22)                      |24 hrs| low priority, negative project balance |  

**Note 1:** You cannot submit to these queues directly, you can only submit to the routing queue "prod".  
**Note 2:** All of these queues have a limit of ten (10) jobs running/accruing **per-project**  
**Note 3:** All of these queues have a limit of one hundred (100) jobs queued (not accruing score) **per-project**  

### Scheduling Errors

If you receive a `qsub: Job rejected by all possible destinations` error, then check your submission parameters.
The issue is most likely that your walltime or node count do not fall within the ranges listed above for the production execution queues.
Please see the table above for limits on production queue job sizes.

**NOTE:** For batch submissions, if the parameters within your submission script do not meet the parameters of any of the above queues you might not receive the "Job submission" error on the command line at all.
This can happen because your job is in waiting in a routing queue and has not yet reached the execution queues.
In this case you will receive a jobid back and qsub will exit, however when the proposed job is routed, it will be rejected from the execution queues.
In that case, the job will be deleted from the system and will not show up in the job history for that system.
If you run a qstat on the jobid, it will return `qstat: Unknown Job Id <jobid>`.


### Job Priority
As with all Argonne Leadership Computing Facility production systems, job priority in the queue is based on several criteria:

1. positive balance of your project
2. size (in nodes) of the job, larger jobs receive higher priority
3. the type of project (e.g. INCITE, ALCC, or discretionary)
4. job duration - shorter duration jobs will accumulate priority more quickly, so it is best to specify the job run time as accurately as possible
 
## Polaris specific stuff
### Polaris Rack and Dragonfly group mappings
* Racks contain (7) 6U chassis; Each chassis has 2 nodes for 14 nodes per rack
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

|g0 |g1 |g2	|g3 |g4 |g5 |g6 |g7 |g8 |g9|
|----|----|----|----|----|----|----|----|----|----|
|x3001-g0	|x3005-g1	|x3009-g2	|x3013-g3	|x3101-g4 |x3105-g5 |x3109-g6 |x3201-g7 |x3205-g8 |x3209-g9|
|x3002-g0	|x3006-g1	|x3010-g2	|x3014-g3	|x3102-g4 |x3106-g5 |x3110-g6 |x3202-g7 |x3206-g8 |x3210-g9|
|x3003-g0	|x3007-g1	|x3011-g2	|x3015-g3	|x3103-g4 |x3107-g5 |x3111-g6 |x3203-g7 |x3207-g8 |x3211-g9 |
|x3004-g0	|x3008-g1	|x3012-g2	|x3016-g3	|x3104-g4 |x3108-g5 |x3112-g6 |x3204-g7 |x3208-g8 |x3212-g9

## Controlling the execution on your allocated resources

### Running MPI+OpenMP Applications
Once a submitted job is running calculations can be launched on the compute nodes using `mpiexec` to start an MPI application. Documentation is accessible via `man mpiexec` and some helpful options follow.

* `-n` total number of MPI ranks
* `-ppn` number of MPI ranks per node
* `--cpu-bind` CPU binding for application
* `--depth` number of cpus per rank (useful with `--cpu-bind`)
* `--env` set environment variables (`--env OMP_NUM_THREADS=2`)
* `--hostfile` indicate file with hostnames (the default is `--hostfile $PBS_NODEFILE`)

A sample submission script with directives is below for a 4-node job with 32 MPI ranks on each node and 8 OpenMP threads per rank (1 per CPU).

```
#!/bin/bash
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
### Running GPU-enabled Applications
GPU-enabled applications will similarly run on the compute nodes using the above example script. 

* The environment variable `MPICH_GPU_SUPPORT_ENABLED=1` needs to be set if your application requires MPI-GPU support whereby the MPI library sends and recieves data directly from GPU buffers. In this case, it will be important to have the `craype-accel-nvidia80` module loaded both when compiling your application and during runtime to correctly link against a GPU Transport Layer (GTL) MPI library. Otherwise, you'll likely see `GPU_SUPPORT_ENABLED is requested, but GTL library is not linked` errors during runtime.

* If running on a specific GPU or subset of GPUs is desired, then the `CUDA_VISIBLE_DEVICES` environment variable can be used. For example, if one only wanted an application to access the first two GPUs on a node, then setting `CUDA_VISIBLE_DEVICES=0,1` could be used.

### Binding MPI ranks to GPUs
The Cray MPI on Polaris does not currently support binding MPI ranks to GPUs. For applications that need this support, this instead can be handled by use of a small helper script that will appropriately set `CUDA_VISIBLE_DEVICES` for each MPI rank. One example is available [here](https://github.com/argonne-lcf/GettingStarted/tree/master/Examples/Polaris/affinity_gpu) where each MPI rank is similarly bound to a single GPU with round-robin assignment.

A example `set_affinity_gpu_polaris.sh` script follows where GPUs are assigned round-robin to MPI ranks.
```
#!/bin/bash
num_gpus=4
gpu=$((${PMI_LOCAL_RANK} % ${num_gpus}))
export CUDA_VISIBLE_DEVICES=$gpu
echo “RANK= ${PMI_RANK} LOCAL_RANK= ${PMI_LOCAL_RANK} gpu= ${gpu}”
exec "$@"
```
This script can be placed just before the executable in the `mpiexec` command like so.
```
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} --depth=${NDEPTH} --cpu-bind depth ./set_affinity_gpu_polaris.sh ./hello_affinity
```
Users with different needs, such as assigning multiple GPUs per MPI rank, can modify the above script to suit their needs.

### Using Fakeroot with Singularity
The fakeroot feature (commonly referred as rootless mode) allows an unprivileged user to run a container as a “fake root” user by leveraging user namespace UID/GID mapping.  To request this feature be enabled on your job add the following to your `qsub` command line:

`-l singularity_fakeroot=true`

<!--### Need help from applications people for this section

* Thinking of things like:
  * How do you set affinity
  * Nvidia specific stuff
  * There is a PALS specific thing to tell you what rank you are in a node?
  * Should Chris cook up example running four mpiexec on different GPUs and separate CPUs or just rely on PBS's vnode (discussion at very top here)?-->
