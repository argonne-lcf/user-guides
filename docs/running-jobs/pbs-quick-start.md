---
description: "Request and manage compute resources on ALCF systems with PBS's qsub, qstat, qalter, and qdel commands."
---

# PBS Quick Start

At a high level, getting computational tasks run on an HPC system is a two-step process:

1. You request and get allocated resources (we allocate at the node level, but
   some facilities you request number of cores and RAM, etc.) on one or more of
   the systems.
   This is accomplished by interacting with the job scheduler / workload
   manager.
   In the ALCF, we use PBS Professional.
2. You execute your tasks on those resources.
   This is accomplished in your job script by interacting with various system
   services (MPI, OpenMP, the HPE PALS task launch system, etc.)

This page deals with step one: requesting and receiving allocated resources and interacting with the job scheduler.

## Quick Start {#Quick-Start}

Here are the "Big Four" commands you will use:

1. `qsub`: request resources (generally compute nodes) to run your job and start your script/executable on the head node. Here is the minimal qsub allowed at the ALCF:
    - `qsub -A <project> -l select=<# of nodes>,walltime=HH:MM:SS,filesystems=fs1:fs2 <your job script>`
    - The `-A`, `walltime`, and `filesystems` are mandatory. You will receive errors if they are not specified.
    - We automatically add `-k doe` for you. This streams your output back rather than spooling it and copying it back at the end of the job. It probably isn't a bad idea to specify it in your script, but we enforce that option, so if you try and change it, you will get an error.
    - It is highly likely you will also want to add `-l place=scatter` so that each of your chunks (`<# of nodes>`) gets its own vnode.
    - If you want to run an executable rather than a script replace `<your jobs script>` in the example above with `-- <your executable>` (that is dash dash)
    - PBS Documentation: Users Guide, Chapter 2, page UG-11 and Reference Guide Chapter 2, section 2.57, page RG-216
2. `qstat`: check on the status of your jobs or queues
    - Try these variations and see which you like best: `qstat`, `qstat -was`, `qstat -was1`, `qstat -wan`, `qstat -wan1`. Add `-x` to see jobs that have completed. We keep two weeks of history.
    - `qstat -Q` will list all the queues in case you forget.
    - PBS Documentation: Users Guide Sec. 10.2, page UG-175; Reference Guide Sec. 2.55, page RG-200
3. `qalter`: update your request for resources
    - Just like qsub, just add a jobid at the end. Only works before the job starts;
    - If you want to change the walltime to 30 minutes: `qalter -l walltime=30:00:00 <jobid>`
    - PBS Documentation: Users Guide Sec. 9.2, page UG-168; Reference Guide Sec. 2.40, page RG-130
4. `qdel`: cancel a job that you don't need. This will also kill a running job
    - `qdel <jobid>`
    - Occasionally, the job will still show up in `qstat` after you try and `qdel` it. When this happens you can try `qdel -W force <jobid>`. If it still won't go away, please send mail to <support@alcf.anl.gov> and one of the administrators can remove it for you. DO NOT just default to using `-W force`. The force does not do all of the clean up and can cause problems of its own.
    - PBS Documentation: Users Guide Sec. 9.3, page UG-170; Reference Guide Sec. 2.41, page RG-143

/// note

The page numbers in the PBS guides are unique. If you search for the specified page number it will take you directly to the relevant page.
///

### Definitions and Notes {#Definitions-and-Notes}

`chunk`
:    A set of resources allocated as a unit to a job. Specified inside a selection directive. All parts of a chunk come from the same host. In a typical MPI (Message-Passing Interface) job, there is one chunk per MPI process.

`vnode`
:   A virtual node, or vnode, is an abstract object representing a host or a set of resources which form a usable part of an execution host. This could be an entire host, or a nodeboard or a blade. A single host can be made up of multiple vnodes. Each vnode can be managed and scheduled independently. Each vnode in a complex must have a unique name. Vnodes on a host can share resources, such as node-locked licenses. PBS operates on vnodes. A vnode can, and in ALCF often will, represent an entire host, but it doesn't have to. For instance, there is a mode on Polaris where we could have each physical host look like four vnodes, each with 16 threads, 1/4 of the RAM and one A100.

`ncpus`
:   Number of resources available to execute a program. In ALCF, given the way we configure PBS, this equates to a hardware thread. For example, a single socket node with a 32 core CPU, each with two hardware threads would report that as ncpus=64.

`ngpus`
:   The number of allocable GPUs on the vnode.

`job`
:   A job equates to a qsub. A set of resources allocated to you for a period of time. You will execute one or more `tasks` on those resources during your job.

`task`
:   A single execution on the resources of your job, often an `mpiexec` invocation launched by PALS or PMIx. You may run one task or many tasks during your job. You may run tasks sequentially or divide your resources up and run several tasks concurrently. Also sometimes referred to as _job steps_.

## Using Fakeroot with Singularity {#Using-Fakeroot-with-Singularity}

The fakeroot feature (commonly referred to as rootless mode) allows an unprivileged user to run a container as a “fake root” user by leveraging user namespace UID/GID mapping. To request this feature be enabled on your job add the following to your `qsub` command line:

`-l singularity_fakeroot=true`

<!-- Need help from applications people for this section

* Thinking of things like:
  * How do you set affinity
  * NVIDIA specific stuff
  * There is a PALS specific thing to tell you what rank you are in a node?
  * Should Chris cook up example running four mpiexec on different GPUs and separate CPUs or just rely on PBS's vnode (discussion at very top here)?
-->
