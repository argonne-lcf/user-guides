<!---# Connecting to a Tenstorrent Galaxy node--->
# Getting Started

## Join the CELS ttgalaxy project
To access the Tenstorrent Galaxy nodes, you will need to join the Tenstorrent Galaxy CELS project (ttgalaxy).

To join the Tenstorrent Galaxy CELS project, first login to [https://accounts.cels.anl.gov](https://accounts.cels.anl.gov).

To login, you will need your CELS username, your CELS password, and for two factor authentication, Duo Mobile.

Then,

- On the left menu, click on Join Project.
- Find the "ttgalaxy" project
- Select it
- Click on "Request Membership"

## Connection to a Tenstorrent Galaxy node

Connection to one of the **Tenstorrent Galaxy** nodes requires use of a CELS login node. Two-factor authentication is used: your CELS password, and a passcode from Duo Mobiled.<br>
*In the examples below, <strong>replace CELSUserID with your CELS user id.</strong>*<br>
To connect to a Tenstorrent Galaxy node:<br>

ssh to a login node:
```bash
ssh CELSUserID@homes.cels.anl.gov
```

Then ssh to a Tenstorrent Galaxy node, using your CELS password. 

**_NOTE:_**
We have two Tenstorrent Galaxy nodes. One is reserved for testing Interence and the second is reserved for testing Mettalium/Metal - SDK

For inference server testing, 
```bash
ssh ttgalaxy-01.ai.alcf.anl.gov
```

For tt-metal testing, 
```bash
ssh ttgalaxy-02.ai.alcf.anl.gov
```

Each node has additional storage available at `/storage`. 
