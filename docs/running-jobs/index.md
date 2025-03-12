# Running Jobs using PBS

## Documentation / Tools
* [The PBS "BigBook"](https://help.altair.com/2022.1.0/PBS%20Professional/PBS2022.1.pdf): This is really excellent. We highly suggest you download it and search through it when you have questions. However, it is big at about 2000 pages / 40MB and contains a bunch of stuff you don't really need, so you can also download the guides separately here:
    * [The PBS Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf): This is the user's guide.
    * [The PBS Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf): This is the Reference Guide. It shows every option and gives you details on how to format various elements on the command line.
* [Cobalt qsub options to PBS qsub options](./pbs-qsub-options-table.md): Shows how to map Cobalt command line options to PBS command line options. Can be found at the link above.

## Introduction
At a high level, getting computational tasks run on an HPC system is a two-step process:

1. You request and get allocated resources (we allocate at the node level, but some facilities you request number of cores and RAM, etc.) on one or more of the systems. This is accomplished by interacting with the job scheduler / workload manager. In the ALCF, we use PBS Professional.

2. You execute your tasks on those resources. This is accomplished in your job script by interacting with various system services (MPI, OpenMP, the HPE PALS task launch system, etc.)

Our documentation is organized in two sections aligned with the two steps described above.

## Table of Contents
 - **Obtaining and managing compute resources at ALCF - General PBS information common to all systems**
     - [Definitions and Notes](#Definitions-and-Notes)
     - [Quick Start](#Quick-Start)
     - [qsub - submit a job to run](#qsub)
     - [qstat - query the status of jobs/queues](#qstat)
     - [qalter - alter a queued job](#qalter)
     - [qdel - delete a queued or running job](#qdel)
     - [qmove - move a job to a different queue](#qmove)
     - [qhold,qrls - place/release a hold on a job in a queue](#qhold,qrls)
     - [qselect - utility to select jobids that meet criteria](#qselect)
     - [qmsg - write a message into a job's output file](#qmsg)
     - [qsig - send a signal to a job](#qsig)
     - [pbsnodes - Get information about the current state of nodes](#pbsnodes)
    - [Using Fakeroot with Singularity](#Using-Fakeroot-with-Singularity)

## Obtaining and managing compute resources at ALCF

### <a name="Definitions-and-Notes"></a>Definitions and Notes
`chunk`: *A set of resources allocated as a unit to a job. Specified inside a selection directive. All parts of a chunk come from the same host. In a typical MPI (Message-Passing Interface) job, there is one chunk per MPI process.*

`vnode`: *A virtual node, or vnode, is an abstract object representing a host or a set of resources which form a usable part of an execution host. This could be an entire host, or a nodeboard or a blade. A single host can be made up of multiple vnodes. Each vnode can be managed and scheduled independently. Each vnode in a complex must have a unique name. Vnodes on a host can share resources, such as node-locked licenses.* PBS operates on vnodes. A vnode can, and in ALCF often will, represent an entire host, but it doesn't have to. For instance, there is a mode on Polaris where we could have each physical host look like four vnodes, each with 16 threads, 1/4 of the RAM and one A100.

`ncpus`: Number of resources available to execute a program. In ALCF, given the way we configure PBS, this equates to a hardware thread. For example, a single socket node with a 32 core CPU, each with two hardware threads would report that as ncpus=64.

`ngpus`: The number of allocable GPUs on the vnode. For an NVIDIA A100, this could be one, however, if we enable *Multi Instance GPU (MIG)* mode and use cgroups it could be as high as 7.

`job`: A job equates to a qsub. A set of resources allocated to you for a period of time. You will execute one or more `tasks` on those resources during your job.

`task`: A single execution on the resources of your job, often an `mpiexec` invocation launched by PALS or PMIx. You may run one task or many tasks during your job. You may run tasks sequentially or divide your resources up and run several tasks concurrently. Also sometimes referred to as *job steps*.

## <a name="Quick-Start"></a>Quick Start
If you are an ALCF user and are familiar with Cobalt, you will find the PBS commands very similar though the options to qsub are quite different. Here are the "Big Four" commands you will use:

1. `qsub`: request resources (generally compute nodes) to run your job and start your script/executable on the head node. Here is the minimal qsub allowed at the ALCF:
    * `qsub -A <project> -l select=<# of nodes>,walltime=HH:MM:SS,filesystems=fs1:fs2 <your job script>`
    * The `-A`, `walltime`, and `filesystems` are mandatory. You will receive errors if they are not specified.
    * We automatically add `-k doe` for you. This streams your output back rather than spooling it and copying it back at the end of the job. It probably isn't a bad idea to specify it in your script, but we enforce that option, so if you try and change it, you will get an error.
    * It is highly likely you will also want to add `-l place=scatter` so that each of your chunks (`<# of nodes>`) gets its own vnode.
    * If you want to run an executable rather than a script replace `<your jobs script>` in the example above with `-- <your executable>` (that is dash dash)
    * PBS Documentation: Users Guide, Chapter 2, page UG-11 and Reference Guide Chapter 2, section 2.57, page RG-216
2. `qstat`: check on the status of your jobs or queues
    * Try these variations and see which you like best: `qstat`, `qstat -was`, `qstat -was1`, `qstat -wan`, `qstat -wan1`. Add `-x` to see jobs that have completed. We keep two weeks of history.
    * `qstat -Q` will list all the queues in case you forget.
    * PBS Documentation: Users Guide Sec. 10.2, page UG-175; Reference Guide Sec. 2.55, page RG-200
3. `qalter`: update your request for resources
    * Just like qsub, just add a jobid at the end. Only works before the job starts;
    * If you want to change the walltime to 30 minutes: `qalter -l walltime=30:00:00 <jobid>`
    * PBS Documentation: Users Guide Sec. 9.2, page UG-168; Reference Guide Sec. 2.40, page RG-130
4. `qdel`: cancel a job that you don't need. This will also kill a running job
    * `qdel <jobid>`
    * Occasionally, the job will still show up in `qstat` after you try and `qdel` it. When this happens you can try `qdel -W force <jobid>`. If it still won't go away, please send mail to <support@alcf.anl.gov> and one of the administrators can remove it for you. DO NOT just default to using `-W force`. The force does not do all of the clean up and can cause problems of its own.
    * PBS Documentation: Users Guide Sec. 9.3, page UG-170; Reference Guide Sec. 2.41, page RG-143

!!! note

	The page numbers in the PBS guides are unique. If you search for the specified page number it will take you directly to the relevant page.

## <a name="qsub"></a>`qsub`: submit a job to run
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf), Chapter 2, page UG-11 and [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Chapter 2, section 2.57, page RG-216

At the ALCF, your qsub will likely use the following parameters:
```bash
qsub -A <project> -k doe -l select=<#>:system=<name>, walltime=HH:MM:SS, filesystems=fs1:fs2, place=scatter <your job script>
```

Where:

* project is the project name associated with your allocation. What you check the balance of with the `sbank` command. This is a mandatory option at the ALCF. If you don't include it you will get `qsub: Account_Name is required to be set.`
* `-k doe` is telling PBS to stream your output rather than buffer it on the compute nodes and then `scp` it at the end of the job. Note we will automatically add this if you don't specify it. We enforce this option, so if you try and specify any other output handling you will get an error.
*  \# of chunks (typically nodes). Each of our systems has a PBS "*resource*" called `system` defined and set to the system name (`polaris`, `aurora`, etc)
*  `walltime=HH:MM:SS` specifying a wall time is mandatory at the ALCF. Valid wall times depend on the queue you are using. There is a table with the queues for each machine at the end of this section and in the machine specific documentation.
*  `filesystems=fs1:fs2:...` Specifying which filesystems your application uses is mandatory at ALCF. The reason for this is if a filesystem goes down, we have a way of making PBS aware of that and it won't run jobs that need that filesystem. If you don't specify filesystems you will receive the following error: `qsub: Resource: filesystems is required to be set.`
*  `place=scatter` is telling PBS you want each of your chunks on a separate vnode. By default, PBS will pack your chunks to get maximum utilization. If you requested `ncpus=1` and `chunks=64` **without** `place=scatter` on a system with `ncpus=64`, all your chunks would end up on one node.
*  Your job script:  See [Example Job Scripts](./example-job-scripts.md) for more information about how to build your job script. For options that won't change, you do have the option of taking things off the command line and putting them in your job script. For instance the above command line could be simplified to `qsub -l select=<#> <your job script>` if you added the following to the top (the PBS directives have to be before any executable line) of your job script:

```bash linenums="1"
#PBS -A <project>
#PBS -k doe
#PBS -l walltime=HH:MM:SS
#PBS -l filesystems=fs1:fs2
```

Also note that if you want to run an executable directly rather than a script you use two dashes and the executable name in place of your script name like this: `-- /usr/bin/sleep 600`

### More detail:

The single biggest difference between Cobalt and PBS is the way you select resources when submitting a job. In Cobalt, every system had its own Cobalt server and you just specified the number of nodes you wanted (-n). With PBS, we are planning on running a single "PBS Complex" which means there will be a single PBS server for all systems in the ALCF and you need to specify enough constraints to get your job to run on the resources you want/need. One advantage of this is that getting resources from two different systems or "co-scheduling" is trivially possible.

#### Resource Selection and Job Placement
Section 2.57.2.6 RG-219 Requesting Resources and Placing jobs in the [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf).

Resources come in two flavors:

* Job Wide: Walltime is the most common example of a job wide resource. You use the `-l` option to specify job wide resources, i.e. `-l walltime=06:00:00`. All the resources in the job have the same walltime.
  * `-l <resource name>=<value>[,<resource name>=<value> ...]`
* Chunks: (see the definition above) This is how you describe what your needs are to run your job. You do this with the `-l select=` syntax. In the ALCF, we do whole node scheduling and every node has a resource called `system` which is set to the system name it belongs to (Polaris, Aurora, etc). This means you can typically get away with the very simple `-l select=128:system=foo` which will give you 128 complete nodes on system foo.
  * `-l select=[<N>:]<chunk>[+[<N>:]<chunk> ...]` where N specifies how many of that chunk and a chunk is of the form:
  * `<resource name>=<value>[:<resource name>=<value> ...]`
  * Here is a hypothetical example that would select resources with A100s and other resources with A40 GPUs. PBS takes care of co-scheduling the nodes on the two systems for you transparently. Note that in this case since we did not specify `system=` if there were multiple systems that could satisfy a chunk you wouldn't know ahead of time which system you would get.

 `-l select=128:ncpus=64:ngpus=4:gputype=A100+32:ncpus=64:ngpus=2:gputype=A40`

You also have to tell PBS how you want the chunks distributed across the physical hardware. You do that via the `-l place` option:

* `-l place=[<arrangement>][: <sharing> ][: <grouping>]` where
  * arrangement is one of `free | pack | scatter | vscatter`
     * unless you have a specific reason to do otherwise, you probably want to set this to `scatter`, otherwise you may not get what you expect. For instance on a host with ncpus=64, if you requested `-l select=8:ncpus=8` you could end up with all of our chunks on one node.
     * `free` means PBS can distribute them as it sees fit
     * `pack` means all chunks from one host. Note that this is not the minimum number of hosts, it is one host. If the chunks can't fit on one host, the qsub will fail.
     * `scatter` means take only one chunk from any given host.
     * `vscatter` means take only one chunk from any given vnode. If a host has multiple vnodes, you could end up with more than one chunk on the host.
  * sharing is one of `excl | shared | exclhost` where
     * NOTE: Node configuration can override your requested sharing mode. For instance, in most cases ALCF sets the nodes to `force_exclhost`, so normally you don't have to specify this.
     * `excl` means this job gets the entire vnode
     * `shared` means the vnode could be shared with another job from another user.
     * `exclhost` means this job gets the entire host, even if it has multiple vnodes.
  * group=`<resource name>`
     * As an example, for machines that use a dragonfly network topology, we provide a PBS resource named `tier1` indicating which dragonfly group a node is in. If you wanted to ensure that all the chunks came from a single dragonfly group, you could specify `place=group=tier1` as part of your qsub. `tier0` is rack granularity, so `group=tier0` would ensure your nodes all came from one rack. Note that if you requested more nodes than were available in a rack your job would never run and you would see something like `Not Running: Insufficient amount of resource: tier0`.

We have defined *placement sets* for the tier0 and tier1 resources. As a result, if you don't specify a grouping PBS will *preferentially* group your nodes in a placement set, but it won't drain or delay your job start to do so. For example, if you request 10 nodes and don't specify a grouping, if 10 nodes are available in the same rack, all your nodes will be in one rack. If not, but there are 10 nodes in a single dragonfly group, all your nodes will be in one dragonfly group. If you wish to specify a specific rack or dragonfly group, that is accomplished via the select syntax. For instance, `qsub ... -l select=10:tier1=g0` would force your 10 nodes to be in dragonfly group 0.

Here is a heavily commented sample PBS submission script that shows some more of the options, but remember that the PBS manuals referenced at the top of this page are the ultimate resource.

```bash
#!/bin/bash -l
# UG Section 2.5, page UG-24 Job Submission Options
# Add another # at the beginning of the line to comment out a line
# NOTE: adding a switch to the command line will override values in this file.

# These options are MANDATORY at ALCF; Your qsub will fail if you don't provide them.
#PBS -A <short project name>
#PBS -l walltime=HH:MM:SS
#file systems used by the job
#PBS -l filesystems=home:eagle


# Highly recommended
# The first 15 characters of the job name are displayed in the qstat output:
#PBS -N <name>

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
#PBS -M <email addresses>

# Setting job dependencies
# UG Section 6.2, page UG-109 Using Job Dependencies
# There are many options for how to set up dependencies;  afterok will give behavior similar
# to Cobalt (uncomment to use)
##PBS depend=afterok:<jobid>:<jobid>

# Environment variables (uncomment to use)
# UG Section 6.12, page UG-126 Using Environment Variables
# RG Sect 2.57.7, page RG-233 Environment variables PBS puts in the job environment
##PBS -v <variable list>
## -v a=10, "var2='A,B'", c=20, HOME=/home/zzz
##PBS -V exports all the environment variables in your environment to the compute node


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

#### Email Notifications

Users should add ```-M <email address>``` if they want notifications as a best practice.

**Note:** For users with '<username>@alcf.anl.gov' email addresses, PBS will send out an email once the job has ended by default. If you do not want to receive these notifications, you will need to add ```#PBS -m n``` to your script.

#### Specifying Filesystems

**Note:** The `filesystems` attribute is mandatory. If you do not specify a filesystem(s) you will receive the following error message upon submission:

`qsub: Resource: filesystems is required to be set.`

Valid filesystems are `home` and `eagle`. For example, to request the home and Eagle filesystems for your job you would add `-l filesystems=home:eagle` to your `qsub` command.

If a job is submitted while a filesystem it requested is marked down, the job will be queued but will not run, with a message in the comment field of the job as to why it is not running. Run `qstat -f <jobid>` to see the comment field. For example, if the job requested for eagle and if Eagle is unavailable, the comment field will have `Can Never Run: Insufficient amount of server resource: eagle_fs (True != False)`). Once the affected filesystem has been returned to normal operation, and the filesystem is marked as being available, the job will then be scheduled normally. The job cannot run until all filesystems requested by the job are available.

If a job requesting a filesystem that is marked down is already in the queue, the job will be not run until all of its requested filesystems are available.

An example of a job requesting filesystems:

`qsub -l select=10:ncpus=64,walltime=30:00,filesystems=eagle:home -A ProjectX -q prod my_job.sh`

To update the filesystems list for your job, use `qalter`.

### qsub examples

* `qsub -A my_allocation -l select=4:system=polaris -l filesystems=home:eagle -l walltime=30:00 -q debug-scaling -- a.out`
	* run a.out on 4 chunks on polaris with a walltime of 30 minutes in debug-scaling queue; charge my_allocation;
	* Since we allocate full nodes on Polaris, 4 chunks will be 4 nodes. If we shared nodes, that would be 4 threads.
	* use the -- (dash dash) syntax when directly running an executable.
* `qsub -A my_allocation -l place=scatter  -l filesystems=home:eagle -l select=32:ncpus=32 -q prod -l walltime=30:00 mpi_mm_64.sh`
	* 32 chunks on any system that meets the requirements. Each chunk must have 32 HW threads; `place=scatter` means use a different vnode for each chunk, even if you could fit more than one on a vnode. Use the queue named `prod`.

## <a name="qstat"></a>`qstat`: Query the status of jobs/queues
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 10.2, page UG-175; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.55, page RG-200

### Jobs
At its most basic, you just type `qstat` and it will list all the jobs currently running, queued, or held on the system. If you are interested in a specific job or jobs, you can provide a space-separated list on the command line: `qstat job1 job2...`.

```
Job id            Name             User              Time Use S Queue
----------------  ---------------- ----------------  -------- - -----
349726.polaris-p* PDE2             user1                    0 Q prod
336987.polaris-p* inf_clDB         user2                    0 H large
353205.polaris-p* 3d-2.sub         user3             2044:14* R large
```

One of the annoying things about `qstat` is that the output fields are fixed width and it will truncate the output. This is indicated by an asterisk as the last character. You can add `-w` for wide. It doesn't prevent truncation, but makes it less likely. A useful variant is `qstat -was1`. It shows the number of nodes, tasks, the requested walltime, and the comment, all on one line. `qstat -wan` will give you the node list you ran on, just remember that can be long. If you want an estimate of when the job will start, add the `-T` option. Note that start time is not available for all jobs, just the next N jobs that are expected to run. If you want to know everything there is to know about the job, add the `-f` flag.

```
                                                            Req'd  Req'd   Elap
Job ID          Username Queue    Jobname    SessID NDS TSK Memory Time  S Time
--------------- -------- -------- ---------- ------ --- --- ------ ----- - -----
353201.polaris* user1    large    3d-1.sub    34449  60 38*    --  24:00 R 08:25    Job run at Tue Nov 15 at 16:44 on (x3006c0s13b1n0:ngpus=4:ncpus=64)+(x...
353289.polaris* user2    medium   run_mae_l*    --   32 20*    --  12:00 Q   --     Not Running: Job would conflict with reservation or top job
353411.polaris* user3    large    1310W60       --   64  64    --  06:00 Q   --     Not Running: Not enough free nodes available
336990.polaris* user4    large    inf_clDB      --  464 29*    --  01:00 H   --     Job held by user4 on Mon Oct  3 20:16:26 2022
```
The `comment` field is your friend. Wondering why your job isn't running? Check the comment. Wondering about the fate of a finished job? Add the `-x` option to see finished jobs (our history retention is currently set at two weeks) and check the comment. This cannot be stressed enough. Often, when a user ticket comes in about PBS, we answer it by looking at the comment.

If you are familiar with `jq` or some other command line JSON tool, the `-F JSON` option can be quite handy. `grep` is great, but when you grep the `-f` output for something, you probably want to know which node the found lines belong to. With the JSON output that is trivial.

```
allcock@polaris-login-02:~/.ssh>  qstat -fF JSON | jq '.Jobs | map_values(select(.job_state == "R") | {Job_Name, Account_Name, qtime, stime})'
{
  "349710.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov": {
    "Job_Name": "P38",
    "Account_Name": "CompBioAffin",
    "qtime": "Fri Nov  4 11:04:12 2022",
    "stime": "Fri Nov 11 07:52:12 2022"
  },
  "352220.polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov": {
    "Job_Name": "mdsim_10000_run1.pbs",
    "Account_Name": "RL-fold",
    "qtime": "Thu Nov 10 22:41:55 2022",
    "stime": "Fri Nov 11 09:00:12 2022"
  },
```

### Queues
 `qstat -Q` Will show you the names of all the queues and tell you their status. If they are enabled (Ena column), you can queue jobs into them. If they are started (Str column) then the scheduler will try and run jobs from it. There is a `-f` (full) option but that is mostly for admins, though you can find the min and max node count `(resources_[min|max].nodect)` and min and max walltime `(resources_[min|max]walltime)` from the output. Those values are also available in this documentation.

## <a name="qalter"></a>`qalter`: Alter a queued job
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 9.2, page UG-168; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.40, page RG-130

Basically takes the same options as `qsub`;  Say you typoed and set the walltime to 300 minutes instead of 30 minutes. You could fix it (if the job had not started running) by doing `qalter -l walltime=30:00 <jobid> [<jobid> <jobid>...]`
 The new value overwrites any previous value.

## <a name="qdel"></a>`qdel`: Delete a queued or running job:
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 9.3, page UG-170; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.41, page RG-143

`qdel <jobid> [<jobid> <jobid>...]`

Occasionally, the job will still show up in `qstat` after you try and `qdel` it. When this happens you can try `qdel -W force <jobid>`. If it still won't go away, please send mail to <support@alcf.anl.gov>  and one of the administrators can remove it for you. DO NOT just default to using `-W force`. The force does not do all of the clean up and can cause problems of its own.


## <a name="qmove"></a>`qmove`: Move a job to a different queue
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 9.7, page UG-173; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.46, page RG-175

* `qmove <new queue> <jobid> [<jobid> <jobid>...]`
* Only works before a job starts running

## <a name="qhold,qrls"></a>`qhold,qrls`: Place / release a user hold on a job
[Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec 2.44, page RG-150 and Sec 2.50, page RG-183

* `[qhold | qrls] <jobid> [<jobid> <jobid>...]`

## <a name="qselect"></a>`qselect`: Query jobids for use in commands
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 10.1, page UG-175; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.52, page RG-189

* ```qdel `qselect -N test1` ``` will delete all the jobs that had the job name set to `test1`.

## <a name="qmsg"></a>`qmsg` Write a message into a job's output file
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 9.4, page UG-171; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.47, page RG-177

* `qmsg -E -O "This is the message" <jobid> [<jobid> <jobid>...]`
* `-E` writes it to standard error, `-O` writes it to standard out

## <a name="qsig"></a>`qsig` Send a signal to a job
[Users Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSUserGuide2022.1.pdf) Sec. 9.5, page UG-172; [Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec. 2.53, page RG-195

* `qsig -s <signal> <jobid> [<jobid> <jobid>...]`
* If you don't specify a signal, `SIGTERM` is sent.

## <a name="pbsnodes"></a>`pbsnodes` Get information about the current state of nodes ###
[Reference Guide](https://help.altair.com/2022.1.0/PBS%20Professional/PBSReferenceGuide2022.1.pdf) Sec 2.7 page RG-36

This is more for admins, but it can tell you what nodes are free (state), how many "CPUs" which is actually the number of threads (ncpus), how many GPUs (ngpus) which with some GPUs like NVIDIA A100s can change depending on the MIG mode, and if the node is shared or not (sharing).

`pbsnodes <node name>`: Everything there is to know about a node

```bash
> pbsnodes x3002c0s7b1n0
x3002c0s7b1n0
     Mom = x3002c0s7b1n0.hsn.cm.polaris.alcf.anl.gov
     Port = 15002
     pbs_version = 2022.1.1.20220926110806
     ntype = PBS
     state = free
     pcpus = 64
     resources_available.arch = linux
     resources_available.demand = False
     resources_available.gputype = A100
     resources_available.host = x3002c0s7b1n0
     resources_available.mem = 527672492kb
     resources_available.ncpus = 64
     resources_available.ngpus = 4
     resources_available.system = polaris
     resources_available.tier0 = x3002-g0
     resources_available.tier1 = g0
     resources_available.vnode = x3002c0s7b1n0
     resources_assigned.accelerator_memory = 0kb
     resources_assigned.hbmem = 0kb
     resources_assigned.mem = 0kb
     resources_assigned.naccelerators = 0
     resources_assigned.ncpus = 0
     resources_assigned.ngpus = 0
     resources_assigned.vmem = 0kb
     resv_enable = True
     sharing = force_exclhost
     license = l
     last_state_change_time = Tue Nov 15 19:26:39 2022
     last_used_time = Tue Nov 15 19:26:39 2022
     server_instance_id = polaris-pbs-01.hsn.cm.polaris.alcf.anl.gov:15001
```

`pbsnodes -avSj`: A nice table to see what is free and in use

```bash
> pbsnodes -avSj
                                                        mem       ncpus   nmics   ngpus
vnode           state           njobs   run   susp      f/t        f/t     f/t     f/t   jobs
--------------- --------------- ------ ----- ------ ------------ ------- ------- ------- -------
x3014c0s19b0n0  job-exclusive        1     1      0  503gb/503gb   63/64     0/0     4/4 353394
x3014c0s19b1n0  resv-exclusive       0     0      0  503gb/503gb    0/64     0/0     4/4 --
x3014c0s1b0n0   offline              0     0      0  503gb/503gb   64/64     0/0     4/4 --
```

`pbsnodes -avSj | grep free | wc -l`: A quick way to see how many nodes are free

```bash
[20220217-21:09:30]> pbsnodes -avSj | grep free | wc -l
38
```

`pbsnodes -avSj | grep free | awk '{print $1}'`: Lists the free nodes

```bash
[20220217-21:09:30]> pbsnodes -avSj | grep free | awk '{print $1}'
x3201c0s25b0n0
x3209c0s13b0n0
x3209c0s19b0n0
x3209c0s1b1n0
```

`pbsnodes -l`: (lowercase  l) see which nodes are down. The comment often indicates why it is down

```bash
[20220217-21:10:31]> pbsnodes -l
x3014c0s19b0n0       offline,resv-exclusive Xid 74 -- GPUs need reseat
x3014c0s25b0n0       offline,resv-exclusive Checking on ConnectX-5 firmware
```

### Job Priority

In PBS it is not easy to see a priority order for which jobs will run next. The best way is to use the `-T` option on qsub and look at the estimated start times. ALCF runs a custom scheduler algorithm, but in general, the job priority in the queue is based on several criteria:

1. positive balance of your project
2. size (in nodes) of the job, larger jobs receive higher priority
3. the type of project (e.g. INCITE, ALCC, or discretionary)
4. job duration: shorter duration jobs will accumulate priority more quickly, so it is best to specify the job run time as accurately as possible


## <a name="Troubleshooting-Common-Errors"></a>Troubleshooting / Common Errors

If you receive a `qsub: Job rejected by all possible destinations` error, then check your submission parameters. The issue is most likely that your walltime or node count do not fall within the ranges listed above for the production execution queues. Please see the table above for limits on production queue job sizes.

!!! bug "Job missing from queue"

    If you receive a job ID but you cannot find your job with `qstat`, then this may be a submission parameter issue. This can happen for batch submission because the job is being accepted into the routing (`prod`) queue. The routing/`prod` queue's parameters are more broad since it needs to accommodate for all three production queues (`small`, `medium`, & `large`). The prod routing queue accepts the job, generating a job ID. Depending on what is going on with the system, the routing may or may not occur before the `qsub` returns (i.e., if the queues are backed-up the routing queue can't route the job before the `qsub` returns). If the routing is delayed then a job ID is returned, and routing is completed later. Since the `qsub` has ended then there isn't a way to inform the user that this has been rejected by all routing destinations. If you run a `qstat` on the `jobid`, it will return `qstat: Unknown Job Id <jobid>`.

## <a name="Using-Fakeroot-with-Singularity"></a>Using Fakeroot with Singularity
The fakeroot feature (commonly referred to as rootless mode) allows an unprivileged user to run a container as a “fake root” user by leveraging user namespace UID/GID mapping. To request this feature be enabled on your job add the following to your `qsub` command line:

`-l singularity_fakeroot=true`
<!-- Need help from applications people for this section

* Thinking of things like:
  * How do you set affinity
  * NVIDIA specific stuff
  * There is a PALS specific thing to tell you what rank you are in a node?
  * Should Chris cook up example running four mpiexec on different GPUs and separate CPUs or just rely on PBS's vnode (discussion at very top here)?
-->


