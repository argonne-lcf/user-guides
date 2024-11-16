# Miscellaneous

## Status

### GC-Monitor

The command `gc-monitor` is Graphcore's device usage monitor. Run it as follows for ordinary monitoring. See `gc-monitor --help` for other options.

```console
export IPUOF_VIPU_API_HOST=10.1.3.101
gc-monitor --no-card-info --all-partitions
# or watch gc-monitor --no-card-info --all-partitions

```
The IPUOF_VIPU_API_HOST environment variable can conflict with the running of poptorch programs. 
The graphcore nodes have a convenience script that temporarily sets this environment variable.
```console
wrapped_gc_monitor.sh --no-card-info --all-partitions
```

> Note: if there are no partitions active, gc-monitor will core dump: `Segmentation fault (core dumped)`

The output will look something like:

```console
+--------------------------------------------------------------+-----------------------+
|      IPUs in slurm_2616 attached from other namespaces       |         Board         |
+----+------------------------------+--------------+-----------+-----------+-----------+
| ID |       Application host       |    Clock     |   Temp    |   Temp    |   Power   |
+----+------------------------------+--------------+-----------+-----------+-----------+
| 0  |         gc-poplar-02         |   1850MHz    |  24.2 C   |  21.1 C   |  92.3 W   |
+----+------------------------------+--------------+-----------+-----------+-----------+
```

### GC-Info
The command `gc-info` is used to display device information. See `gc-info --help` for more options.

To list devices, 
```bash
gc-info -l
```

The command `gc-info` lists the partition and different IPU Id's along with the multi-IPU configuration IDs.

```console
-+- Id:  [0], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [3]
-+- Id:  [1], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [2]
-+- Id:  [2], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [1]
```

One may also display detailed information for a specific device.  The devices are numbered 0-63.  For example,

```console
gc-info --device-id 0 --device-info
```

See `gc-info --help` for more information.

## How busy is the system?

Use one of

```bash
top
htop
```
