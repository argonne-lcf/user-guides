# VNC on Polaris

## Overview

VNC (Virtual Network Computing) is available on Polaris to run graphics applications. To avoid slowing down login nodes, please run VNC on compute nodes.

## Password for VNC

Create a unique password for VNC and store it in a secure directory.

Setting the password:

```
vncpasswd [file]
```

Omitting `[file]` will save the password to `~/.vnc/passwd`.

## VNC Launch Script

Create a script for your preferred shell (e.g., Bash) with the following commands:

```bash
#!/bin/bash

Xvnc -PasswordFile ~/.vnc/passwd -geometry 1920x1080 :0 &
sleep 2
icewm --display=:0 &
xterm -display :0 &
```

## Launching the Script

1. Start an interactive session using `qsub` with the `-I` option.
2. Run the VNC launch script on the compute node.

## Establishing an SSH Tunnel

On another terminal, type:

```bash
ssh -v -N -L 5900:x3005c0s7b0n0:5900 yourusername@polaris.alcf.anl.gov
```

where `x3005c0s7b0n0` is the name of your Polaris compute node. The default TCP port for VNC is 5900. Edit as needed.

## Connecting to Your VNC Server

1. Install a VNC client on your local computer.
2. Run the client and connect to `localhost:5900`.
3. Enter your VNC password.
4. Launch your graphics application from the xterm.

## Tips

- Keep the SSH tunnel terminal open during your session.
- Adjust the SSH tunnel command as needed for your setup.
- If for any reason you need to restart the VNC server and get error messages, try:

```bash
killall Xvnc icewm xterm
```