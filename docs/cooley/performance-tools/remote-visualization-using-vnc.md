---
search:
  exclude: true
---

# Remote Visualization on Cooley Using VNC
When running graphics applications on Cooley, it is best to use the client/server mode when available. 

A lightweight client can run on your local resource and connect to a server application running on the Cooley visualization nodes.  For applications that do not support a client/server mode, VNC can be used for remotely accessing such applications running on Cooley, and leveraging its GPUs.

## Setup on Cooley
On cooley.alcf.anl.gov, if you do not have a ~/.vnc/xstartup file, create one like the following:
```
#!/bin/sh
xterm &
twm
```

Be sure to make it executable:
```
> chmod u+x ~/.vnc/xstartup
```

Also, create a VNC password, which you will need to provide each time you connect a remote VNC client to a VNC server running on Cooley:
```
> vncpasswd
```

This will store an obfuscated version in ~/.vnc/passwd

## Start a VNC server on Cooley
Since we want the VNC server to run on a backend node, in order to leverage the GPU, we need to submit a job:
```
> qsub -I -n 1 -t <time> -A <projectID>
```

Once your job starts, you will be logged into a visualization node, where you can launch a VNC server:
```
> x0vncserver --display=:0.0 --NeverShared=1 --geometry=2400x1500+0+0 --PasswordFile=/home/<username>/.vnc/passwd --MaxProcessorUsage=100
```

**Note:** Take note of the hostname where your job is running (in the form cc###). You will need this in the next steps.

- We use x0vncserver so that we can leverage the existing X server running on the node, which uses the GPU.  
- We specify --display=:0.0 to tell it which display to use.
- Because the existing display has a resolution of 4096x4096, we use the --geometry flag to specify a region of that display to use.  This should be set this to a size appropriate for displaying on your local display.  You may also wish to adjust the +0+0 to adjust the portion of the display that is visible.
- Replace <username> with your login name in the path to your VNC PasswordFile.
- Since we will have exclusive use of the node, we set the --MaxProcessorUsage=100 (otherwise the default is 35).

## On Your Local Resource
From a shell on your local resource, establish an ssh tunnel through the Cooley login node to the backend node where you started the VNC server (the cc### noted above.) This will require the use of your OTP token.

```
> ssh -L 5900:cc###:5900 <username>@cooley.alcf.anl.gov
```
  
Once the ssh connection is established, from this shell launch the xstartup script on your visualization node.  

If your default shell is bash, use the following command (this will block, and not return you to a command prompt):
```
ssh cc### “export DISPLAY=:0.0; ~/.vnc/xstartup”
```
  
If your default shell is csh/tcsh, use the following command (this will block, and not return you to a command prompt):
```
ssh cc### “setenv DISPLAY :0.0; ~/.vnc/xstartup”
```
  
Now in a start a vnc viewer on your local resource, for example:
```
> xvncviewer localhost::5900
```

**Notes:**
- Since we are tunneling, set the host to localhost.
- Syntax for VNC clients may vary.  Check the documentation for your specific client to determine appropriate syntax for specifying the host and port.

This should open a VNC viewer with an xterm running in it, where you can launch graphics applications running on the Cooley backend node, and taking advantage of the GPU.

**Additional note:** Because you are likely not using the full 4096x4096 area of the display, it is possible that some applications that automatically place their windows may place them outside of the region that you are viewing.  Some application may provide a mechanism for placing the window at a specific location.  Otherwise, you may need to adjust the +0+0 portion of the --geometry flag when running the x0vncserver executable to adjust the portion of the display that is visible.
  
## Cleaning Up
When you are all done, be sure to clean up:

- Exit the VNC viewer
- Kill the VNC server (cntrl C), and exit the cc### shell
- You may need to cntrl C to exit the ssh command in the shell used to create the tunnel 
- Then exit that shell to close the tunnel
  
  
