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

Two copies of the model are maintained.  One in CPU memory and one in RDU
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
snconfig
```

The snconfig utility shows the static configuration of the system. The configuration on sn30-r1-h1 for the first RDU is as follows:

```text
# TODO
Platform Name: DataScale SN10-8
Node Name: NODE
Number of XRDUS: 4
XRDU Name: XRDU_0
Number of RDUS: 2
RDU name: RDU_0
Number of TILES: 4
TILE Name: TILE_0
Serial Number : N/A
...
Number of PCIES: 4
PCIE Name: PCIE_0
Bandwidth : 32 GB/s
Speed : 16 GT/s
Width : 16
Serial Number : N/A
...
Number of DDRCHs: 6
DDR CH Name: DDRCH_0
Number of DIMMS: 2
DIMM Name: DIMM_C0
Size : 64.0 GB
DIMM Name: DIMM_C1
Size : 0.0 GB
Serial Number : N/A
Current utilization can be seen with sntilestat. In this example, only
four tiles in one RDU are in use.
TILE %idle %exec %pload %aload %chkpt %quiesce PID USER COMMAND
/XRDU_0/RDU_0/TILE_0 80.4 7.0 10.4 2.2 0.0 0.0 49880 arnoldw python
res_ffn_mnist.py run --pef=pef/res_ffn_mnist/res_ffn_mnist.pef
--num-epochs 100
/XRDU_0/RDU_0/TILE_1 80.5 6.9 11.3 1.3 0.0 0.0 49880 arnoldw python
res_ffn_mnist.py run --pef=pef/res_ffn_mnist/res_ffn_mnist.pef
--num-epochs 100
/XRDU_0/RDU_0/TILE_2 82.1 4.7 11.4 1.8 0.0 0.0 49880 arnoldw python
res_ffn_mnist.py run --pef=pef/res_ffn_mnist/res_ffn_mnist.pef
--num-epochs 100
/XRDU_0/RDU_0/TILE_3 80.1 6.3 11.7 1.9 0.0 0.0 49880 arnoldw python
res_ffn_mnist.py run --pef=pef/res_ffn_mnist/res_ffn_mnist.pef
--num-epochs 100
/XRDU_0/RDU_1/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_0/RDU_1/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_0/RDU_1/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_0/RDU_1/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_0/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_0/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_0/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_0/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_1/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_1/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_1/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_1/RDU_1/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_0/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_0/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_0/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_0/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_1/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_1/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_1/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_2/RDU_1/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_0/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_0/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_0/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_0/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_1/TILE_0 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_1/TILE_1 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_1/TILE_2 100.0 0.0 0.0 0.0 0.0 0.0
/XRDU_3/RDU_1/TILE_3 100.0 0.0 0.0 0.0 0.0 0.0
```

### SambaNova Daemon Service

The following command checks if the SambaNova daemon service is running.

```bash
systemctl status snd
```

The output should look something like this:

```text
# TODO Update this output.
* snd.service - SN Devices Service
   Loaded: loaded (/usr/lib/systemd/system/snd.service; enabled; vendor preset: enabled)
   Active: active (running) since Fri 2022-02-18 11:45:15 CST; 1 months 25 days ago
 Main PID: 3550 (snd)
    Tasks: 10 (limit: 19660)
   CGroup: /system.slice/snd.service
           `-3550 /opt/sambaflow/bin/snd

Warning: Journal has been rotated since the unit was started. Log output is incomplete or unavailable.
```

### Tile status

```bash
sntilestat
watch sntilestat
```

The output shown below is when the system is completely idle.

```text
# TODO Update this output.
TILE                 %idle %exec %pload %aload %chkpt %quiesce    PID     USER COMMAND
/XRDU_0/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_0/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_1/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_2/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_0/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_0 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_1 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_2 100.0   0.0    0.0    0.0    0.0      0.0
/XRDU_3/RDU_1/TILE_3 100.0   0.0    0.0    0.0    0.0      0.0
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
