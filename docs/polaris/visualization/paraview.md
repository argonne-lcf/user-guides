# Paraview on Polaris

At this time, we only support client/server mode where the user must manually launch the server on Polaris.

One can launch an interactive session on Polaris compute nodes with the following command (adjust parameters as needed to match your allocation, desired number of nodes, queue, walltime, and filesystems):

```shell
qsub -l walltime=01:00:00 -l select=2 -A yourallocation -q debug -I -l filesystems=home:grand
```

When the job starts you will receive a prompt on your head node like this:

```
username@x3005c0s37b0n0:~>
```

Make a note of the node hostname (3005c0s37b0n0 in the example above). You can also get this information from qstat -fx jobID


Now load the Paraview module

```
username@x3005c0s37b0n0:~> module load paraview

Lmod is automatically replacing "nvhpc/21.9" with "gcc/11.2.0".

----------------------------------------------------------------------------------
     Paraview v5.11.0 successfully loaded
----------------------------------------------------------------------------------
```

and launch the Paraview server with

```
username@x3005c0s37b0n0:~> mpirun -n 8 pvserver
Waiting for client...
Connection URL: cs://x3005c0s37b0n0:11111
Accepting connection(s): x3005c0s37b0n0:11111
```

In this case pvserver will be listening on TCP port of your head node.

We need to establish an ssh tunnel to connect client to server. On your local machine open a new terminal and type:

```
ssh -v -L 11111:3005c0s37b0n0:11111 polaris.alcf.anl.gov
```

and, among multiple lines with debug information,  you should see something like:

```
debug1: Local connections to LOCALHOST:11111 forwarded to remote address 3005c0s37b0n0:11111
```

Now you are ready to launch your Paraview client locally. Keep in mind that client and servers versions must match

Connect your client to

```
localhost:11111
```








