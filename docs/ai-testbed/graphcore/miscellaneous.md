# Miscellaneous

## Status

### GC-Monitor

```bash
gc-monitor --no-card-info
```

The output will look something like:

```console
+----------------------------------------------------------------------------------------------------------------------+------------------------+-----------------+
|                                         Attached processes in partition p64                                          |          IPU           |      Board      |
+--------+---------------------------------------------------------------------------------------+--------+------------+----+----------+--------+--------+--------+
|  PID   |                                        Command                                        |  Time  |    User    | ID |  Clock   |  Temp  |  Temp  | Power  |
+--------+---------------------------------------------------------------------------------------+--------+------------+----+----------+--------+--------+--------+
| 268524 |                                        python3                                        |  4m1s  |  wilsonb   | 6  | 1850MHz  | 34.5 C | 27.2 C |114.7 W |
| 268524 |                                        python3                                        |  4m1s  |  wilsonb   | 7  | 1850MHz  | 34.8 C |        |        |
+--------+---------------------------------------------------------------------------------------+--------+------------+----+----------+--------+--------+--------+
```

### GC-Info

```bash
gc-info -l
```

The command **gc-info** lists the partition and different IPU Id's along with the multi-IPU configuration IDs.

```console
-+- Id:  [0], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [3]
-+- Id:  [1], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [2]
-+- Id:  [2], target: [Fabric], IPU-M host:  [10.1.5.1], IPU#: [1]
```

One may also check a specific device ID.  The devices a numbered 0-63.  For example,

```bash
gc-info --device-id 0 --device-info
```

See **gc-info --help** for more information.

## How busy is the system?

Use one of

```bash
top
htop
```
