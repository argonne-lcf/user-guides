# Paraview on Polaris

The recommended way of running ParaView on Polaris is in client/server mode. This consists of running the ParaView client on your local resource, and the ParaView server (pvserver) on the Polaris compute nodes. There are two ways to accomplish this, detailed below. In both cases, the ParaView client needs to first be installed on your local resource, and needs to match the version that you run on Polaris.

The most recent production version currently installed on Cooley is ParaView  5.11.1. Binary and source packages for Linux, MacOS, and Windows are available from the [ParaView Download Page](https://www.paraview.org/download/). (Run the 'module avail paraview' command on a Polaris login node to see earlier versions of ParaView that are available.)

As mentioned, there are two ways to run ParaView in client/server mode. For more details, including advantages and disadvantages of each, see the section below on: Trade Offs

The first, and arguably easier, way is to run the ParaView client locally, and have it launch the pvserver on Polaris, and connect back to your local client. For details see the section below on: Automated / Reverse Connection

The other way is to first manually launch the pvserver on Polaris, and then launch the ParaView client locally and connect to your running pvserver. For details see the section below on: Manual / Forward Connection

## Automated / Reverse Connection
This section describes how to launch the pvserver on Polaris from a local ParaView client.

### Start ParaView Client
First, launch the ParaView client on your local resource. In order to launch the pvserver on Polaris and have it connect back to our local client, we will need to configure some server settings in the client. This initial set up should only need to be done once, and can be reused each time you want to run ParaView on Polaris.

### Server Configuration

#### 1. Select Connect
From the ParaView client choose to connect to a server by either clicking on the "Connect" icon in the menu bar, or selecting:

File->Connect

From the main menu:

<figure markdown>
  ![Select connect](images/01-Pv3-connect.png){ width="700" }
  <figcaption>Select connect</figcaption>
</figure>

#### 2. Set Up Servers (first time only)
The first time we want to run a pvserver on Polaris and have it connect to our local ParaView client, we need to set up a Server. Once we set up this server, we can reuse it each time we run the ParaView client with the pvserver on Polaris.

Kitware, the maintainers of ParaView, maintain a database of server configurations, which we can retrieve through the ParaView client. NOTE: At this time, Polaris is not available on the list maintained by Kitware, so you will need to download a file and load it manually.

Download this file [Polaris server configuration](server_polaris.pvsc). This file contains settings in ASCII, so it is safe to download. If your browser renames it, the file extension should be .pvsc

Click "Load Servers" and load the file downloaded in the previous step.

<figure markdown>
  ![Load servers](images/02-pv_gui_server_config.png){ width="700" }
  <figcaption>Load servers</figcaption>
</figure>
















## Setting up Paraview 

From your local client select Connect, either from the File menu, or by clicking on the icon circled below:

![Connect icon](connect-icon.png) 

A new window will open where you can configure a server. Click on Add Server:

![Choose server](choose-server.png)

Give your server a name, select Client/Server, localhost, and a TCP port (8000 in this example)

![Edit server](edit-server.png)

Click "Configure". In the next window there is an option to set up how Paraview server will be launched, and the default is "Manual". Leave it on "Manual" and click "Save".

You will use these settings when establishing the connection.

## Launching the Paraview server on Polaris

You can launch an interactive session on Polaris compute nodes with the following command (adjust parameters as needed to match your allocation, desired number of nodes, queue, walltime, and filesystems):

```shell
qsub -l walltime=01:00:00 -l select=2 -A yourallocation -q debug -I -l filesystems=home:grand
```

When the job starts you will receive a prompt on your head node like this:

```
username@x3005c0s7b0n0:~>
```

Make a note of the node hostname (`x3005c0s7b0n0` in the example above). You can also get this information from `qstat -fx jobID`


Now load the Paraview module

```
username@x3005c0s7b0n0:~> module load paraview

Lmod is automatically replacing "nvhpc/21.9" with "gcc/11.2.0".

----------------------------------------------------------------------------------
     Paraview v5.11.0 successfully loaded
----------------------------------------------------------------------------------

Due to MODULEPATH changes, the following have been reloaded:
  1) cray-mpich/8.1.16
```

and launch the Paraview server with

```
srizzi@x3005c0s7b0n0:~> mpirun -n 8 pvserver --server-port=8000
Waiting for client...
Connection URL: cs://x3005c0s7b0n0:8000
Accepting connection(s): x3005c0s7b0n0:8000
```

In this case `pvserver` will be listening on TCP port 8000 of your head node. You can change this port if you want.

## Creating a tunnel over ssh

We need to establish an ssh tunnel to connect client to server. On your local machine open a new terminal and type:

```
ssh -v -N -L 8000:x3005c0s7b0n0:8000 polaris.alcf.anl.gov
```

where 8000 is a TCP port and `x3005c0s7b0n0` the name of your head node. Adjust these values accordingly.

Among multiple lines with debug information,  you should see something like:

```
debug1: Local connections to LOCALHOST:8000 forwarded to remote address x3005c0s7b0n0:8000
```

Keep this terminal open for the duration of your session to keep the ssh tunnel active.

Now you are ready to launch your Paraview client locally. Keep in mind that client and servers versions must match. The Paraview version currently deployed on Polaris is 5.11.0

## Connecting to Paraview server

Connect your Paraview client to the server configuration you created above. You can select Connect, either from the File menu, or the icon circled in the figure:

![Connect icon](connect-icon.png)  

and selecting the configuration you created in a previous step.

The connection should point to:

```
localhost:8000
```

In the terminal where you launched the server you will see now that the connection is established. Note that Paraview may take a few seconds to connect. This is normal behavior.

```
username@x3005c0s7b0n0:~> mpirun -n 8 pvserver --server-port=8000
Waiting for client...
Connection URL: cs://x3005c0s7b0n0:8000
Accepting connection(s): x3005c0s7b0n0:8000
Client connected.
```

At this point you can use Paraview normally.






