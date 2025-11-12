# Tunneling and Forwarding Ports

Port forwarding is covered here.  This is specifically for TensorBoard.

## TensorBoard Port Forwarding

This section describes the steps to be followed to set up port forwarding for applications,
like TensorBoard, which runs on the SambaNova system and binds to one or more ports.
This example uses 6006 and 16006 as port numbers. Using port numbers other than these may
avoid collisions with other users.

### From Your Local Machine

*Replace* ***ALCFUserID*** *with your ALCF User ID.*

Run

```bash
# Forward a port number from sambanova.alcf.anl.gov to your local machine.
ssh -v -N -f -L localhost:16006:localhost:16006 ALCFUserID@sambanova.alcf.anl.gov
...
Password: < MobilePass+ code >

# Connect to sambanova.alcf.anl.gov
ssh ALCFUserID@sambanova.alcf.anl.gov
...
Password: < MobilePass+ code >
```

### From **sambanova.alcf.anl.gov**

Below are the commands specific to sn30-r1-h1. You may replace **sn30-r1-h1** with any other node when using the appropriate system.

Run

> **Note**:  The full name is sn30-r1-h1.ai.alcf.anl.gov and it may also be used.

```bash
# Forward the port.
ssh -N -f -L localhost:16006:localhost:6006 ALCFUserID@sn30-r1-h1
# Connect to the system.
ssh ALCFUserID@sn30-r1-h1
```

### On **sn30-r1-h1**

Activate the venv appropriate to your project.

Navigate to the appropriate directory for your model.
Launch your model using **srun** or **sbatch**.

```bash
cd /path/to/your/project
sbatch --output=pef/my_model/output.log submit-my_model-job.sh
```

### On Another sn30-r1-h1 Terminal Window

The SambaNova system has a bash shell script to setup the required software environment.
This sets up the SambaFlow software stack, the associated environmental variables and activates
a pre-configured virtual environment.

Use the command appropriate for your environment.

For example, if you are using LogReg:

```bash
ALCFUserID@sn30-r1-h1:~$ source /opt/sambaflow/apps/starters/logreg/venv/bin/activate
(venv) ALCFUserID@sn30-r1-h1:~$
```

Navigate to the appropriate directory for your model.

```bash
cd /path/to/your/project
tensorboard --logdir /logs --port 6006
```

### Browser on Local Machine

Then, navigate in your browser to, in this example, [http://localhost:16006](http://localhost:16006) on your local machine.

## Notes

Explanation of **ssh** command:

```text
-N : no remote commands

-f : put ssh in the background

-L <machine1>:<portA>:<machine2>:<portB> :

The full command line will forward <machine2>:<portB> (remote scope) to <machine1>:<portA> (local scope)
```

Adapted from:  [How can I run Tensorboard on a remote server?](https://stackoverflow.com/questions/37987839/how-can-i-run-tensorboard-on-a-remote-server)
