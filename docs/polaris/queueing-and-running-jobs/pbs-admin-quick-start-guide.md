# PBS Admin Quick Start Guide
The single most important thing I can tell you is where to get the [PBS BigBook](https://anl.box.com/s/rj7ac90tv6wxeu20lvfqf5j0cga76owc).  It is very good and a search will usually get you what you need if it isn't in here.

- [PBS Admin Quick Start Guide](#pbs-admin-quick-start-guide)
  - [Checking / Setting Node Status](#node-status)
  - [Troubleshooting](#troubleshooting)
  - [Starting, stopping, restarting, status of the daemons:](#start-stop-daemons)
  - [Starting, stopping scheduling across the entire complex](#start-stop-scheduling)
  - [Starting, stopping queues:](#start-stop-queues)
  - ["Boosting" jobs (running them sooner)](#boosting)
  - [Reservations](#reservations)
  - [MIG Mode](#mig)
  - [Rack and Dragonfly group mappings](#groups)

## <a name="node-status"></a>Checking / Setting Node Status
The `pbsnodes` command is your friend.

* check status
   * `pbsnodes -av` gives you everything; grep will be useful here
   * `pbsnodes -avSj` give you a nice table summary
   * `pbsnodes -l` lists the nodes that are offline
* Taking nodes on and offline
   * `pbsnodes -C <comment> -o <nodelist>` will mark a node offline in PBS (unschedulable)
      * Adding the time and date and why you took it offline in the comment is helpful 
      * `<nodelist>` is space separated 
   * `pbsnodes -r <node list>` will attempt to bring a node back online

## <a name="troubleshooting"></a>Troubleshooting
* PBS_EXEC (where all the executables are): `/opt/pbs/[bin|sbin]`
* PBS_HOME (where all the data is): `/var/spool/pbs`
   * logs: `/var/spool/pbs/[server|mom|sched|comm]_logs`
   * config: `/var/spool/pbs/[server|mom|sched]_priv/`
* `/etc/pbs.conf` - Reference Guide Section 9.1, page RG-371 
* `qstat -[x]f [jobid]`
   * the -x shows jobs that have already completed.  We are currently holding two weeks history.
   * the `comment` field is particularly useful.  It will tell you why it failed, got held, couldn't run, etc..
   * The jobid is optional.  Without it you get all jobs.
* `tracejob <jobid>` 
   * This seems to work better on pbs0, though I haven't completely figured out the rules
   * This does a rudimentary aggregation and filter of the logs for you.
* `qselect` -  Reference Guide Section 2.54 page RG-187.
   * allows you to query and return jobids that meet criteria for instance the command below would delete all the jobs from Yankee Doodle Dandy, username yddandy:
   * ``qdel `qselect -u yddandy` ``
* Error Code Table (Reference Guide Chapter 14, RG-391)
   * If a CLI command (qmgr, qsub, whatever) spits out an error code at you, go look it up in the table, you may well save yourself a good bit of time.
   * We are going to try and either get the error text to come with the code or write a utility to look it up and have that on all the systems.

## <a name="start-stop-daemons"></a>Starting, stopping, restarting, status of the daemons:
* Server: on pbs0 run `systemctl [start | stop |restart | status] pbs`
* MoM:
  * If you only want to restart a single MoM, ssh to the host and issue the same commands as above for ther server.
  * If you want to restart the MoM on every compute node, `ssh admin.polaris` then do: `pdsh -g custom-compute "systemctl [start | stop |restart | status] pbs"`  


## <a name="start-stop-scheduling"></a>Starting, stopping scheduling across the entire complex

`qmgr -c "set server scheduling = [True | False]`

**IMPORTANT NOTE**: If we are running a single PBS complex for all our systems (same server is handling Polaris, Aurora, Cooley2, etc) this will stop scheduling on everything.

To check the current status you may do: `qmgr -c "list server scheduling"`

## <a name="start-stop-queues"></a>Starting, stopping queues:

* **started**: Can you queue a job or not
* **enabled**: Will the scheduler run jobs that are in the queue

So if a queue is started, but not enabled, users can issue qsubs and the job will get queued, but nothing will run until we renable the queue.  Running jobs are unaffected.

`qmgr -c set queue <queue name> started = [True | False]`
`qmgr -c set queue <queue name> enabled = [True | False]`

## <a name="boosting"></a>"Boosting" jobs (running them sooner)
There are two ways you can run a job sooner:

1. `qmove run_next <jobid>` 
    2. Because of the way policy is set for the acceptance testing period, any job in the `run_next` queue will run before jobs in the default `workq` with the exception of jobs that are backfilled.  So by moving the job into the `run_next` queue, you moved it to the front of the line.  There are no restrictions on this, so please do not abuse it.
2. `qorder <jobid> <jobid>`
   3. If you don't necessarily need it to run next, but just want to rearrange the order a bit, you can use `qorder` which swaps the positions of the specified jobids.  So, if one of them was 10th in line and one was 20th, they would switch positions. 

## <a name="reservations"></a>Reservations
Most of the reservation commands are similar to the job commands, but prefixed with `pbs_r` instead of `q`: `pbs_rsub, pbs_rstat, pbs_ralter, pbs_rdel`.  You get the picture.  In general, their behavior is reasonably similar to the equivalent jobs commands.  Note that by default, users can set their own reservations.  We have to use a hook to prevent that. ADD THE HOOK NAME ONCE WE HAVE IT SET.

* There are three types of reservations:
  * Advance and standing reservations - reservations for users;  Note that you typically don't specify the nodes.  You do a resource request like with qsub and PBS will find the nodes for you.
  * job-specific now reservations - we have not used these.  Where they could come in handy is for debugging.  A user gets a job through, we convert it to a job-specific reservation, then if their job dies, they don't have to wait through the queue again, they can keep iterating until the wall time runs out.
  * maintenance reservations. - You can explicitly set which hosts to include in the reservation.
  * Also note that reservations occur in two steps.  The `pbs_rsub` will return with an ID but will say `unconfirmed`.  That means it was syntactically correct, but PBS hasn't figured out if the resources are available yet.  Once it has the resources, it will switch to confirmed.  This normally is done as fast as you can run `pbs_rstat`
* -R (start) -E (end) are in "datetime" format: [[[[CC]YY]MM]DD]hhmm[.SS] 
   * 1315, 171315, 12171315, 2112171315 and 202112171315 would all be Dec 17th, 2021 @ 13:15
     * If that is in the future they are all equivalent and valid
     * If it were Dec 17th, 2021 @ 1400, then 1315 would default to the next day @ 14:00, the rest would be errors because they are in the past. 
     * Be careful or this will bite you.  It will confirm the reservation and you will expect it to start in a few minutes, but it is actually for tomorrow.
* `pbs_rsub -N rsub_test -R 2023 -D 05:00 -l select=4`
  * probably not what you think: `resv_nodes = (edtb-03[0]:ncpus=1)+(edtb-03[0]:ncpus=1)+(edtb-03[0]:ncpus=1)+(edtb-03[0]:ncpus=1)`  It gave me 4 cores on the same node.
* `pbs_rsub -N rsub_test -R 2023 -D 05:00 -l select=2 -l place=scatter`
  * Getting closer: `resv_nodes = (edtb-01[0]:ncpus=1)+(edtb-02[0]:ncpus=1)`
  * The `-l place=scatter` got me two different nodes, but edtb allows sharing, so I got one thread on each node, but there were actually jobs running on those nodes at the time. On Polaris, since the nodes are `force_exclhost` that wouldn't have been an issue.
* `pbs_rsub -N rsub_test -R 2217 -D 05:00 -l select=2:ncpus=64 -l place=scatter:excl` This gave me what I wanted:
	* `resv_nodes = (edtb-03[0]:ncpus=64)+(edtb-04[0]:ncpus=64)`
	* Leaving it to default to `ncpus=1` should work, but asking for them all isn't a bad idea.
* `pbs_rsub -N rsub_test -R 1200 -D 05:00 --hosts x3004c0s1b0n0 x3003c0s25b0n0...`
  *  If you use `--hosts` it makes it a maintenance reservation.  You can't / don't need to add `-l select` or `-l place` on a maintenance reservation.  PBS will set it for you and will make it the entire host and exclusive access.  Nodes don't have to be up.  If jobs are running they will continue to run.  This will override any other reservation.
* `pbs_ralter` You can use this to change attributes of the reservation (start time, end time, how many nodes, which users can access it, etc).  Works just like `qalter` for jobs. 
* `pbs_rdel <reservation id>`  This will kill all running jobs, delete the queue, meaning you lose any jobs that were in the queue, and release all the resources.
* NOTE: once the reservation queue is in place, you use all the normal jobs commands (qsub, qalter, qdel, etc.) to manipulate the jobs in the queue.  On the qsub you have to add `-q <reservation queue name>`

### Giving users access to the reservation
By default, only the person submitting the reservation will be able to submit jobs to the reservation queue.  You change this with the `-U +username@*,+username@*,...`.  You can add this to the initial `pbs_rsub` or use `pbs_ralter` after the fact.  The plus is basically ALLOW. We haven't tested it, but you can also theoretically use a minus for DENY.  If there is a way to specify groups, we are not aware of it.  This is a bit of a hack, but if you want anyone to be able to run you can do `qmgr -c "set queue <reservation queue name> acl_user_enable=False"`

## <a name="mig"></a>MIG mode
* See the [Nvidia Multi-Instance GPU User Guide](https://docs.nvidia.com/datacenter/tesla/mig-user-guide/index.html) for more details.
* `sudo nvidia-smi mig -lgip` List GPU Instance Profiles;  This is how you find the magic numbers used to configure it below.
* `sudo nvidia-smi mig -lgipp` list all the possible placements;  The syntax of the placement is `{<index>}:<GPU Slice Count>` 
* `nvidia-smi --query-gpu=mig.mode.current --format=csv,noheader` - check the status of all the GPUs on the node;  add `-i <GPU number>` to check a specific GPU
* `systemctl stop nvidia-dcgm.service ; systemctl stop nvsm ; sleep 5 ; /usr/bin/nvidia-smi -mig 1` Put the node in MIG mode;  `-mig 0` will take it out of MIG mode.
* `nvidia-smi mig -i 3 -cgi 19,19,19,19,19,19,19 -C` configure GPU #3 to have 7 instances.
* `nvidia-smi mig --destroy-compute-instance; nvidia-smi mig --destroy-gpu-instance` Will free up the resources;  You have to do this before you can change the configuration.

## <a name="groups"></a>Polaris Rack and Dragonfly group mappings
* Racks contain (7) 6U chassis; Each chassis has 2 nodes for 14 nodes per rack
* The hostnames are of the form xRRPPc0sUUb[0|10]n0 where:
    * RR is the row {30, 31, 32}
    * PP is the position in the row {30 goes 01-16, 31 and 32 go 01-12}
    * c is chassis and is always 0 (I wish they would have counted up chasses, oh well)
    * s stands for slot, but in this case is the RU in the rack. Values are {1,7,13,19,25,31,37}
    * b is BMC controller and is 0 or 1 (each node has its own BMC)
    * n is node, but is always 0 since there is only one node per BMC 
* So, 16+12+12 = 40 racks * 14 nodes per rack = 560 nodes.
* Note that in production group 9 (the last 4 racks) will be the designated on-demand racks
* The management racks are x3000 and X3100 and are dragonfly group 10
* The TDS rack is x3200 and is dragonfly group 11 

 
|Group 0| Group 1| Group 2| Group 3|	Group 4| Group 5| Group 6| Group 7| Group 8| Group 9|
|----|----|----|----|----|----|----|----|----|----|
|x3001-g0	|x3005-g1	|x3009-g2	|x3013-g3	|x3101-g4|x3105-g5|x3109-g6|x3201-g7|x3205-g8|x3209-g9|
|x3002-g0	|x3006-g1	|x3010-g2	|x3014-g3	|x3102-g4|x3106-g5|x3110-g6|x3202-g7|x3206-g8|x3210-g9|
|x3003-g0	|x3007-g1	|x3011-g2	|x3015-g3	|x3103-g4|x3107-g5|x3111-g6|x3203-g7|x3207-g8|x3211-g9|
|x3004-g0	|x3008-g1	|x3012-g2	|x3016-g3	|x3104-g4|x3108-g5|x3112-g6|x3204-g7|x3208-g8|x3212-g9|
