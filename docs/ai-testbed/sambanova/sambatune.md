# Profiling and performance tuning with SambaTune

This section covers how to use the SambaTune profiling performance tuning tool, and the SambaTune UI for viewing the results.

##

SambaTune uses a yaml file that describes how to profile an application.</br>
There are samples in `/opt/sambaflow/sambatune/configs`. </br>
This section shows how to run the simplest sample, a linear net.

First, ssh into one of the nodes in the SN30 cluster. <br>
Next, start a slurm interative job reserving a full node (8 RDUs):
```console
$ /usr/local/bin/srun --gres=rdu:8 --pty bash
```
Record the hostname:
```console
$ hostname
sn30-r1-h1
```

Next, set an environment variable indicating where the profiling information should be stored:
```console
#export DUMP_ROOT=/srv/projects/datascience/arnoldw/Sambatune
export DUMP_ROOT=~/Sambatune
```

If running a large model, the profiling information can be hundreds of gigabytes or more, and the DUMP_ROOT should be set to some location with more storage than your home directory (which has a quota).<br>
E.g. somewhere in ```/srv/projects```

Next, run sambatune using a sample sambatune yaml configuration file:
```console
$ sambatune --modes benchmark instrument run -- /opt/sambaflow/sambatune/configs/linear_net.yaml
```

This will take a while to run, particularly if the yaml for a larger model is used.

Then, run `sambatune_ui`:
```console
$ export ST_PORT=8576
$ sambatune_ui --directory $DUMP_ROOT/artifact_root/sambatune_gen --port $ST_PORT
```

Copy the password shown (e.g. to your clipboard). The userid is always admin. The password is different for every sambatune_ui run. 

In a fresh console on your working machine where you will run the browser, set up a two-hop ssh tunnel to the target node:
```console
$ export ST_PORT=8576
$ ssh -L $ST_PORT:localhost:$ST_PORT arnoldw@sambanova.alcf.anl.gov  -t ssh -L $ST_PORT:localhost:$ST_PORT -N sn30-r1-h1
```

Put localhost:8576 in the url bar of a Chrome-family browser. (Chrome, Brave, Vivaldi, Opera tested.)</br>
A login prompt for the sambatune ui should show.<br>
Enter admin and the password copied previously.<br>
You should now see the SambaTune UI. <br>

If the browser does not show a login prompt, or if any previous step complains about a port conflict, try another value for ST_PORT on both the target node and for the ssh tunnel command, e.g. 8577.

When complete:<br>
Break the ssh tunnel with ctrl-c or equivalent.<br>
Stop the sambatune_ui server on the target node with ctrl-c or equivalent.<br>
Exit the interactive slurm job to release the resources.

