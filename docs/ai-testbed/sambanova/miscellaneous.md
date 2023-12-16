# Miscellaneous

## SDK Version

To find the SDK version, run the following commands

```console
# TODO
(venv) ALCFUserID@sn30-r1-h1:~$ python
Python 3.7.6 (default, Feb 18 2020, 21:28:31)
[GCC 9.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import sambaflow
>>> sambaflow.__version__
'1.11.5'
>>>
```

## OMP_NUM_THREADS

The OMP_NUM_THREADS environment variable sets the number of threads to use for parallel regions.

The value of this environment variable must be a list of positive integer values. The values of the list set the number of threads to use for parallel regions at the corresponding nested levels.

For the SambaNova system it, is usually set to one.

```bash
export OMP_NUM_THREADS=16
```

## Where is the Model?

Two copies of the model are maintained.  One in host CPU memory and one in RDU
memory. They do not interfere with each other unless you explicitly sync
the model/parameter in between using:

```text
SambaTensor.rdu() # Moves the CPU model to the RDU
SambaTensor.cpu() # Moves the RDU model to the CPU
```

In order to run the model on the CPU, you can simply use the PyTorch model
as if there is no RDU.
In order to run the model on RDU, you would need to use **session.run()**.

## Useful Commands

### SN Configuration

```bash
snconfig show Node static
```

The snconfig utility shows the static configuration of the system. The configuration for the first **node** is as follows:

```text
======================================================
=======                NODE Info               =======
======================================================
=======                Static Info             =======
Timestamp: 2023-03-16 17:00:04
Platform Name: DataScale SN30-8
Node Name: NODE
    Number of XRDUS: 4
    XRDU Name: XRDU_0
        Number of RDUS: 2
        RDU name: RDU_0
            Serial Number     : 205057B469B35895
            Number of TILES: 8
            TILE Name: TILE_0
                Serial Number     : N/A
            TILE Name: TILE_1
                Serial Number     : N/A


...


                    Size              : 128.0 GB
                    Serial Number     : 1F5BC22
            DDR CH Name: DDRCH_6
                Number of DIMMS: 1
                DIMM Name: DIMM_L0
                    Size              : 128.0 GB
                    Serial Number     : 1F5BC99
            DDR CH Name: DDRCH_7
                Number of DIMMS: 1
                DIMM Name: DIMM_M0
                    Size              : 128.0 GB
                    Serial Number     : 1F5BB68
        Total XRDU_3 memory size (GB): 2048.0
```

### SambaNova Daemon Service

The following command checks if the SambaNova daemon service is running.

```bash
systemctl status snd
```

The output should look something like this:

```text
● snd.service - SN Devices Service
     Loaded: loaded (/lib/systemd/system/snd.service; enabled; vendor preset: enabled)
    Drop-In: /etc/systemd/system/snd.service.d
             └─override.conf
     Active: active (running) since Fri 2023-01-27 04:03:14 UTC; 1 months 18 days ago
   Main PID: 5635 (snd)
      Tasks: 9 (limit: 629145)
     Memory: 156.8M
     CGroup: /system.slice/snd.service
             └─5635 /opt/sambaflow/bin/snd

Warning: some journal files were not opened due to insufficient permissions.
```

### Tile status

```bash
sntilestat
watch sntilestat
```

The output shown below is when the system is completely idle.

```text
TILE                 %idle %exec %pload %aload %chkpt %quiesce    PID     USER COMMAND
/XRDU_0/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_4 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_5 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_6 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_7 100.0   0.0    0.0    0.0    0.0      0.0
```

### Finding Hung Tiles

```bash
snconfig show Node dynamic | grep perfect
```

### How busy is the system?

Use one of

```bash
top
htop
```
