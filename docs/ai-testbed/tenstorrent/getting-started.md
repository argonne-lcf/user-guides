<!---# Connecting to a Tenstorrent Galaxy node--->
# Getting Started

## Connection to a Tenstorrent Galaxy node

Connection to one of the **Tenstorrent Galaxy** nodes requires use of a CELS login node. Two-factor authentication is used: your CELS password, and a code from Duo Mobiled.<br>
*In the examples below, <strong>replace CELSUserID with your CELS user id.</strong>*<br>
To connect to a Tenstorrent Galaxy node:<br>

ssh to a login node:
```bash
ssh CELSUserID@homes.cels.anl.gov
```

Then ssh to a Tenstorrent Galaxy node, using your CELS password. 

For inference server testing, 
```bash
ssh ttgalaxy-01.ai.alcf.anl.gov
```

For tt-metal testing, 
```bash
ssh ttgalaxy-02.ai.alcf.anl.gov
```
