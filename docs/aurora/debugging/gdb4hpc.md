# HPE gdb4hpc on Aurora

The gdb4hpc is not a GPU-aware debugger but can be used to debug general code problems at scale. This debugger will apply commands to all threads in the MPI process group.

## Attaching to a running job

Determine the `jobid` of interest:

```bash
qstat -u $USER
```

``` { .bash .no-copy }
harms@aurora-uan-0009:~/working/all2all> qstat -u $USER

aurora-pbs-0001.hostmgmt.cm.aurora.alcf.anl.gov: 
                                                            Req'd  Req'd   Elap
Job ID          Username Queue    Jobname    SessID NDS TSK Memory Time  S Time
--------------- -------- -------- ---------- ------ --- --- ------ ----- - -----
127750.aurora-* harms    workq    all2all       --    4   4    --  00:30 R   -- 
```

Next find a node the job is running on. Choose the first node in the list of `vnodes`:
```bash linenums="1"
qstat -f 127750 | grep exec_vnode
```

``` { .bash .no-copy }
harms@aurora-uan-0009:~/working/all2all> qstat -f 127750 | grep exec_vnode
exec_vnode = (x4305c2s6b0n0:ncpus=1)+(x4305c2s7b0n0:ncpus=1)+(x4305c4s0b0n0
```

Log in to this node, find your `mpiexec` process id, and run `gdb4hpc`:

```bash linenums="1"
ssh x4305c2s6b0n0
ps -eaf | grep mpiexec
module load gdb4hpc
CTI_WLM_IMPL=ssh gdb4hpc
```

???+ example "Example output"
    ``` { .bash .no-copy}
    harms@aurora-uan-0009:~/working/all2all> ssh x4305c2s6b0n0
    harms@x4305c2s6b0n0:~> ps -eaf | grep mpiexec
    harms    108581 108569  0 16:05 ?        00:00:00 mpiexec -l --no-transfer --line-buffer --np 16 -ppn 4 --cpu-bind core ./a2a-p2p
    harms    109440 109354  0 16:11 pts/4    00:00:00 grep --color=auto mpiexec
    harms@x4305c2s6b0n0:~> module load gdb4hpc
    harms@x4305c2s6b0n0:~> CTI_WLM_IMPL=ssh gdb4hpc
	
    gdb4hpc 4.14.7 - Cray Line Mode Parallel Debugger
    With Cray Comparative Debugging Technology.
    Copyright 2007-2022 Hewlett Packard Enterprise Development LP.
    Copyright 1996-2016 University of Queensland. All Rights Reserved.

    Type "help" for a list of commands.
    Type "help <cmd>" for detailed help about a command.
    dbg all>
	```

Now attach to the `mpiexec` process:

``` { .bash .no-copy }
  dbg all> attach $a <pid>
```

???+ example "Example output"

    ```{ .bash .no-copy }
	dbg all> attach $a 108581
	0/16 ranks connected... (timeout in 299 seconds)
	0/16 ranks connected... (timeout in 298 seconds)
	...
	12/16 ranks connected... (timeout in 300 seconds)
	16/16 ranks connected.
	Created network...
	Connected to application...
	Current rank location:
	a{0}: #0  0x00001472aba12699 in MPIDI_progress_test
	... backtrace ...
	```
