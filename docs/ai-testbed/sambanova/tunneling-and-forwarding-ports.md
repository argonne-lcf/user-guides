# Tunneling and Forwarding Ports

Port forwarding is covered here.  This is specifically for TensorBoard.

## TensorBoard Port-Forwarding

This section describes the steps to be followed to set up port forwarding for applications,
like TensorBoard, which runs on the SambaNova system and binds to one or more ports.
This example uses 6006 and 16006 as port numbers. Using port numbers other than these may
avoid collisions with other users.

### From your local machine

Run

```bash
ssh -v -N -f -L localhost:16006:localhost:16006 ALCFUserID@sambanova.alcf.anl.gov
...
Password: < MobilPass+ code >

ssh ALCFUserID@sambanova.alcf.anl.gov
...
Password: < MobilPass+ code >
```

*replacing* ***ALCFUserID*** *with your ALCF User ID.*

### From **sambanova.alcf.anl.gov**

Below are the commands specific to sm-01. You may replace **sm-01** with **sm-02** when using the appropriate system.

Run

**NOTE:  The full name is sm-01.ai.alcf.anl.gov and it may also be used.**

```bash
ssh -N -f -L localhost:16006:localhost:6006 ALCFUserID@sm-01
ssh ALCFUserID@sm-01
```

### On **sm-01**

Execute the following command:

```bash
ALCFUserID@sm-01:~$ source /software/sambanova/envs/sn_env.sh
(venv) ALCFUserID@sm-01:~$
```

Navigate to the appropriate directory for your model.
Launch your model using **srun** or **sbatch**.

```bash
cd /path/to/your/project
sbatch --output=pef/my_model/output.log submit-my_model-job.sh
```

### On Another sm-01 Terminal Window

The SambaNova system has a bash shell script to setup the required software environment.
This sets up the SambaFlow software stack, the associated environmental variables and activates
a pre-configured virtual environment.

Use

```bash
ALCFUserID@sm-01:~$ source /software/sambanova/envs/sn_env.sh
(venv) ALCFUserID@sm-01:~$
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

The full command line will forward <machine1>:<portA> (local scope) to <machine2>:<portB> (remote scope)
```

Adapted from:  [How can I run Tensorboard on a remote server?](https://stackoverflow.com/questions/37987839/how-can-i-run-tensorboard-on-a-remote-server)
